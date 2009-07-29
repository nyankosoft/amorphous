#include "PostProcessManager.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"

#include "Support/StringAux.hpp"
#include "Support/Log/DefaultLog.hpp"


using namespace std;


// Vertex declaration for post-processing
const D3DVERTEXELEMENT9 PPVERT::Decl[4] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
    { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0 },
    { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  1 },
    D3DDECL_END()
};


#ifndef V
#define V(x)           { hr = x; }
#endif

#ifndef V_RETURN
//#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return hr; } }
#endif

HRESULT CPostProcess::Init( LPDIRECT3DDEVICE9 pDev, const std::string& filename )
{
    HRESULT hr;

    DWORD dwShaderFlags = 0;
    #ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
    #ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif

	m_ShaderFilename = filename;

    hr = D3DXCreateEffectFromFile( pDev,
                                    filename.c_str(),
                                    NULL,
                                    NULL,
                                    dwShaderFlags,
                                    NULL,
                                    &m_pEffect,
                                    NULL );
    if( FAILED( hr ) )
        return hr;

    // Get the PostProcess technique handle
    m_hTPostProcess = m_pEffect->GetTechniqueByName( "PostProcess" );

    // Obtain the handles to all texture objects in the effect
    m_hTexScene[0] = m_pEffect->GetParameterByName( NULL, "g_txSceneColor" );
    m_hTexScene[1] = m_pEffect->GetParameterByName( NULL, "g_txSceneNormal" );
    m_hTexScene[2] = m_pEffect->GetParameterByName( NULL, "g_txScenePosition" );
    m_hTexScene[3] = m_pEffect->GetParameterByName( NULL, "g_txSceneVelocity" );
    m_hTexSource[0] = m_pEffect->GetParameterByName( NULL, "g_txSrcColor" );
    m_hTexSource[1] = m_pEffect->GetParameterByName( NULL, "g_txSrcNormal" );
    m_hTexSource[2] = m_pEffect->GetParameterByName( NULL, "g_txSrcPosition" );
    m_hTexSource[3] = m_pEffect->GetParameterByName( NULL, "g_txSrcVelocity" );

    // Find out what render targets the technique writes to.
    D3DXTECHNIQUE_DESC techdesc;
    if( FAILED( m_pEffect->GetTechniqueDesc( m_hTPostProcess, &techdesc ) ) )
        return D3DERR_INVALIDCALL;

    for( DWORD i = 0; i < techdesc.Passes; ++i )
    {
        D3DXPASS_DESC passdesc;
        if( SUCCEEDED( m_pEffect->GetPassDesc( m_pEffect->GetPass( m_hTPostProcess, i ), &passdesc ) ) )
        {
            D3DXSEMANTIC aSem[MAXD3DDECLLENGTH];
            UINT uCount;
            if( SUCCEEDED( D3DXGetShaderOutputSemantics( passdesc.pPixelShaderFunction, aSem, &uCount ) ) )
            {
                // Semantics received. Now examine the content and
                // find out which render target this technique
                // writes to.
                while( uCount-- )
                {
                    if( D3DDECLUSAGE_COLOR == aSem[uCount].Usage &&
                            RT_COUNT > aSem[uCount].UsageIndex )
                        m_bWrite[uCount] = true;
                }
            }
        }
    }

    // Obtain the render target channel
    D3DXHANDLE hAnno;
    hAnno = m_pEffect->GetAnnotationByName( m_hTPostProcess, "nRenderTarget" );
    if( hAnno )
        m_pEffect->GetInt( hAnno, &m_nRenderTarget );

    // Obtain the handles to the changeable parameters, if any.
    for( int i = 0; i < NUM_PARAMS; ++i )
    {
        char szName[64];

//		StringCchPrintfA( szName, 32, "Parameter%d", i );
        sprintf( szName, "Parameter%d", i );
        hAnno = m_pEffect->GetAnnotationByName( m_hTPostProcess, szName );
        LPCSTR szParamName;
        if( hAnno &&
            SUCCEEDED( m_pEffect->GetString( hAnno, &szParamName ) ) )
        {
            m_ahParam[i] = m_pEffect->GetParameterByName( NULL, szParamName );
            MultiByteToWideChar( CP_ACP, 0, szParamName, -1, m_awszParamName[i], MAX_PATH );
        }

        // Get the parameter description
//		StringCchPrintfA( szName, 32, "Parameter%dDesc", i );
        sprintf( szName, "Parameter%dDesc", i );
        hAnno = m_pEffect->GetAnnotationByName( m_hTPostProcess, szName );
        if( hAnno &&
            SUCCEEDED( m_pEffect->GetString( hAnno, &szParamName ) ) )
        {
            MultiByteToWideChar( CP_ACP, 0, szParamName, -1, m_awszParamDesc[i], MAX_PATH );
        }

        // Get the parameter size
//		StringCchPrintfA( szName, 32, "Parameter%dSize", i );
		sprintf( szName, "Parameter%dSize", i );
        hAnno = m_pEffect->GetAnnotationByName( m_hTPostProcess, szName );
        if( hAnno )
            m_pEffect->GetInt( hAnno, &m_anParamSize[i] );

        // Get the parameter default
//		StringCchPrintfA( szName, 32, "Parameter%dDef", i );
        sprintf( szName, "Parameter%dDef", i );
        hAnno = m_pEffect->GetAnnotationByName( m_hTPostProcess, szName );
        if( hAnno )
            m_pEffect->GetVector( hAnno, &m_avParamDef[i] );
    }

    return S_OK;
}


HRESULT CPostProcess::OnLostDevice()
{
	assert( m_pEffect );
	m_pEffect->OnLostDevice();
	return S_OK;
}


HRESULT CPostProcess::OnResetDevice( DWORD dwWidth, DWORD dwHeight )
{
    assert( m_pEffect );
    m_pEffect->OnResetDevice();

    // If one or more kernel exists, convert kernel from
    // pixel space to texel space.

    // First check for kernels.  Kernels are identified by
    // having a string annotation of name "ConvertPixelsToTexels"
    D3DXHANDLE hParamToConvert;
    D3DXHANDLE hAnnotation;
    UINT uParamIndex = 0;
    // If a top-level parameter has the "ConvertPixelsToTexels" annotation,
    // do the conversion.
    while( NULL != ( hParamToConvert = m_pEffect->GetParameter( NULL, uParamIndex++ ) ) )
    {
        if( NULL != ( hAnnotation = m_pEffect->GetAnnotationByName( hParamToConvert, "ConvertPixelsToTexels" ) ) )
        {
            LPCSTR szSource;
            m_pEffect->GetString( hAnnotation, &szSource );
            D3DXHANDLE hConvertSource = m_pEffect->GetParameterByName( NULL, szSource );

            if( hConvertSource )
            {
                // Kernel source exists. Proceed.
                // Retrieve the kernel size
                D3DXPARAMETER_DESC desc;
                m_pEffect->GetParameterDesc( hConvertSource, &desc );
                // Each element has 2 floats
                DWORD cKernel = desc.Bytes / (2 * sizeof(float));
                D3DXVECTOR4 *pvKernel = new D3DXVECTOR4[cKernel];
                if( !pvKernel )
                    return E_OUTOFMEMORY;
                m_pEffect->GetVectorArray( hConvertSource, pvKernel, cKernel );
                // Convert
                for( DWORD i = 0; i < cKernel; ++i )
                {
                    pvKernel[i].x = pvKernel[i].x / dwWidth;
                    pvKernel[i].y = pvKernel[i].y / dwHeight;
                }

                // Copy back
                m_pEffect->SetVectorArray( hParamToConvert, pvKernel, cKernel );

                delete[] pvKernel;
            }
        }
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some 
// minimum set of capabilities, and rejects those that don't pass by returning false.
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Skip backbuffer formats that don't support alpha blending
	IDirect3D9* pD3D = DIRECT3D9.GetD3D(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    // Check 32 bit integer format support
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, D3DUSAGE_RENDERTARGET,
                                         D3DRTYPE_CUBETEXTURE, D3DFMT_A8R8G8B8 ) ) )
        return false;

    // Must support pixel shader 2.0
    if( pCaps->PixelShaderVersion < D3DPS_VERSION( 2, 0 ) )
        return false;

    return true;
}

/*
//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the 
// application to modify the device settings. The supplied pDeviceSettings parameter 
// contains the settings that the framework has selected for the new device, and the 
// application can make any desired changes directly to this structure.  Note however that 
// DXUT will not correct invalid device settings so care must be taken 
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.  
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
    // Turn vsync off
    pDeviceSettings->pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    g_SettingsDlg.GetDialogControl()->GetComboBox( DXUTSETTINGSDLG_PRESENT_INTERVAL )->SetEnabled( false );

    // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
    // then switch to SWVP.
    if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
         pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
    {
        pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    // Debugging vertex shaders requires either REF or software vertex processing 
    // and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
    if( pDeviceSettings->DeviceType != D3DDEVTYPE_REF )
    {
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;                            
        pDeviceSettings->BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
#endif
#ifdef DEBUG_PS
    pDeviceSettings->DeviceType = D3DDEVTYPE_REF;
#endif

    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( pDeviceSettings->DeviceType == D3DDEVTYPE_REF )
            DXUTDisplaySwitchingToREFWarning();
    }

    return true;
}*/

HRESULT CPostProcess::SetScale( float scale_x, float scale_y )
{
	if( !m_pEffect )
		return E_FAIL;

	HRESULT hr;
    UINT cPasses, p;
    hr = m_pEffect->SetTechnique( "PostProcess" );
    hr = m_pEffect->Begin( &cPasses, 0 );
    for( p = 0; p < cPasses; ++p )
	{
		D3DXHANDLE hPass = m_pEffect->GetPass( m_hTPostProcess, p );

		D3DXHANDLE hExtentScaleX = m_pEffect->GetAnnotationByName( hPass, "fScaleX" );
		if( hExtentScaleX )
			m_pEffect->SetFloat( hExtentScaleX, scale_x );
		else
			hr = m_pEffect->SetFloat( "fScaleX", scale_x );

		D3DXHANDLE hExtentScaleY = m_pEffect->GetAnnotationByName( hPass, "fScaleY" );
		if( hExtentScaleY )
			m_pEffect->SetFloat( hExtentScaleY, scale_y );
		else
			hr = m_pEffect->SetFloat( "fScaleY", scale_y );

		hr = m_pEffect->BeginPass( p );
		hr = m_pEffect->EndPass();
	}
    hr = m_pEffect->End();

	return S_OK;
}



//====================================================================================
// CPostProcessManager
//====================================================================================

CPostProcessManager::CPostProcessManager()
:
m_iNumPostProcesses(0),
m_nScene(0),
m_pEffect(NULL)
{
	for( int i=0; i<RT_COUNT; i++ )
		m_pSceneSave[i] = NULL;

	m_hTRenderNoLight = NULL;
}


/*
HRESULT CPostProcessManager::Init( const char *pFilename )
{
}*/


Result::Name CPostProcessManager::OnCreateDevice( const std::string& shader_filename )
{
    HRESULT hr;

	m_ShaderFilename = shader_filename;

	IDirect3DDevice9* pd3dDevice = DIRECT3D9.GetDevice();

	// retrieve the back buffer size
	D3DSURFACE_DESC back_buffer_desc;
	IDirect3DSurface9 *pBackBuffer;
	pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pBackBuffer->GetDesc( &back_buffer_desc );

	const D3DSURFACE_DESC* pBackBufferSurfaceDesc = &back_buffer_desc;

    // Query multiple RT setting and set the num of passes required
	m_nPasses = 1;
	m_nRtUsed = 1;

    D3DCAPS9 Caps;
    pd3dDevice->GetDeviceCaps( &Caps );
/*	if( Caps.NumSimultaneousRTs > 2 )
	{
        // One pass of 3 RTs
        m_nPasses = 1;
        m_nRtUsed = 3;
    }
    else
    if( Caps.NumSimultaneousRTs > 1 )
    {
        // Two passes of 2 RTs. The 2nd pass uses only one.
        m_nPasses = 2;
        m_nRtUsed = 2;
    }
    else
    {
        // Three passes of single RT.
        m_nPasses = 3;
        m_nRtUsed = 1;
    }
*/
    // Determine which of D3DFMT_A16B16G16R16F or D3DFMT_A8R8G8B8
    // to use for scene-rendering RTs.
    IDirect3D9* pD3D;
    pd3dDevice->GetDirect3D( &pD3D );
    D3DDISPLAYMODE DisplayMode;
    pd3dDevice->GetDisplayMode( 0, &DisplayMode );

    if( FAILED( pD3D->CheckDeviceFormat( Caps.AdapterOrdinal, Caps.DeviceType,
                    DisplayMode.Format, D3DUSAGE_RENDERTARGET, 
                    D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F ) ) )
        m_TexFormat = D3DFMT_A8R8G8B8;
    else
        m_TexFormat = D3DFMT_A16B16G16R16F;

    SAFE_RELEASE( pD3D );

    // Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the 
    // shader debugger. Debugging vertex shaders requires either REF or software vertex 
    // processing, and debugging pixel shaders requires REF.  The 
    // D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug experience in the 
    // shader debugger.  It enables source level debugging, prevents instruction 
    // reordering, prevents dead code elimination, and forces the compiler to compile 
    // against the next higher available software target, which ensures that the 
    // unoptimized shaders do not exceed the shader model limitations.  Setting these 
    // flags will cause slower rendering since the shaders will be unoptimized and 
    // forced into software.  See the DirectX documentation for more information about 
    // using the shader debugger.
    DWORD dwShaderFlags = 0;
    #ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
    #ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif


	LPD3DXBUFFER pCompileErrors;
    // If this fails, there should be debug output as to 
    // they the .fx file failed to compile
    V( D3DXCreateEffectFromFile( pd3dDevice, shader_filename.c_str(), NULL, NULL, dwShaderFlags, 
                                        NULL, &m_pEffect, &pCompileErrors ) );

	if( FAILED(hr) )
	{
		if( pCompileErrors )
		{
			char *pBuffer = (char *)pCompileErrors->GetBufferPointer();
			pCompileErrors->Release();
		}
		return Result::UNKNOWN_ERROR;
	}

    // Initialize the postprocess objects
/*	for( int i = 0; i < PPCOUNT; ++i )
    {
        hr = m_aPostProcess[i].Init( pd3dDevice, dwShaderFlags, g_aszFxFile[i] );
        if( FAILED( hr ) )
            return hr;
    }*/

	m_hTRenderNoLight = m_pEffect->GetTechniqueByName( "RenderNoLight" );


	// Create vertex declaration for post-process
	if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( PPVERT::Decl, &m_pVertDeclPP ) ) )
	{
		return Result::UNKNOWN_ERROR;
	}

	return Result::SUCCESS;
}


int CPostProcessManager::AddPostProcessShader( const std::string& shader_filename )
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	if( NUM_MAX_PPCOUNT <= m_iNumPostProcesses )
	{
		LOG_PRINT_ERROR( fmt_string("Too many post-process effect files: (%d)", m_iNumPostProcesses ) );
		return -1;
	}

	HRESULT hr;
	
	hr = m_aPostProcess[m_iNumPostProcesses].Init( pd3dDevice, shader_filename.c_str() );

	IDirect3DSurface9 *pBackBuffer;
	hr = pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	D3DSURFACE_DESC back_bufffer_desc;
	hr = pBackBuffer->GetDesc( &back_bufffer_desc );

	// TODO: CPostProcess::OnResetDevice() should be called as a callback function
	hr = m_aPostProcess[m_iNumPostProcesses].OnResetDevice( back_bufffer_desc.Width,
		                                                    back_bufffer_desc.Height );

	m_iNumPostProcesses++;

	if( FAILED(hr) )
		return -1;

	return m_iNumPostProcesses - 1;
}



//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// reset, which will happen after a lost device scenario. This is the best location to 
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever 
// the device is lost. Resources created here should be released in the OnLostDevice 
// callback. 
//--------------------------------------------------------------------------------------
HRESULT CPostProcessManager::OnResetDevice()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	D3DSURFACE_DESC back_buffer_desc;
	IDirect3DSurface9 *pBackBuffer;
	pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pBackBuffer->GetDesc( &back_buffer_desc );
	const D3DSURFACE_DESC* pBackBufferSurfaceDesc = &back_buffer_desc;

    HRESULT hr;

	if( m_pEffect )
        V_RETURN( m_pEffect->OnResetDevice() );

	int num_pps = GetNumPostProcesses();
	for( int p = 0; p < num_pps; ++p )
		V_RETURN( m_aPostProcess[p].OnResetDevice( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height ) );

	// Setup the camera's projection parameters
/*	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Camera.SetProjParams( D3DX_PI/4, fAspectRatio, 0.1f, 1000.0f );
	m_pEffect->SetMatrix( "g_mProj", g_Camera.GetProjMatrix() );
	g_Camera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
*/
    // Create scene save texture
    for( int i = 0; i < RT_COUNT; ++i )
    {
        V_RETURN( pd3dDevice->CreateTexture( pBackBufferSurfaceDesc->Width,
                                             pBackBufferSurfaceDesc->Height,
                                             1,
                                             D3DUSAGE_RENDERTARGET,
                                             m_TexFormat,
                                             D3DPOOL_DEFAULT,
                                             &m_pSceneSave[i],
                                             NULL ) );

        // Create the textures for this render target chains
        IDirect3DTexture9 *pRT[2];
        ZeroMemory( pRT, sizeof(pRT) );
        for( int t = 0; t < 2; ++t )
        {
            V_RETURN( pd3dDevice->CreateTexture( pBackBufferSurfaceDesc->Width,
                                                 pBackBufferSurfaceDesc->Height,
                                                 1,
                                                 D3DUSAGE_RENDERTARGET,
                                                 D3DFMT_A8R8G8B8,
                                                 D3DPOOL_DEFAULT,
                                                 &pRT[t],
                                                 NULL ) );
        }
        m_RTChain[i].Init( pRT );
        SAFE_RELEASE( pRT[0] );
        SAFE_RELEASE( pRT[1] );
    }

    // Initialize the render target table based on how many simultaneous RTs
    // the card can support.

	// we consider only the color channel for the moment
    IDirect3DSurface9* pSurf;
	m_pSceneSave[0]->GetSurfaceLevel( 0, &pSurf );
	m_aRtTable[0].pRT[0] = pSurf;
	m_aRtTable[0].pRT[1] = m_aRtTable[0].pRT[2] = NULL;
	m_aRtTable[1].pRT[0] = m_aRtTable[1].pRT[1] = m_aRtTable[1].pRT[2] = NULL;
	m_aRtTable[2].pRT[0] = m_aRtTable[2].pRT[1] = m_aRtTable[2].pRT[2] = NULL;

/*	switch( m_nPasses )
	{
		case 1:
			m_pSceneSave[0]->GetSurfaceLevel( 0, &pSurf );
			m_aRtTable[0].pRT[0] = pSurf;
			m_pSceneSave[1]->GetSurfaceLevel( 0, &pSurf );
			m_aRtTable[0].pRT[1] = pSurf;
			m_pSceneSave[2]->GetSurfaceLevel( 0, &pSurf );
			m_aRtTable[0].pRT[2] = pSurf;
			// Passes 1 and 2 are not used
			m_aRtTable[1].pRT[0] = NULL;
			m_aRtTable[1].pRT[1] = NULL;
			m_aRtTable[1].pRT[2] = NULL;
			m_aRtTable[2].pRT[0] = NULL;
			m_aRtTable[2].pRT[1] = NULL;
			m_aRtTable[2].pRT[2] = NULL;
			break;
		case 2:
			m_pSceneSave[0]->GetSurfaceLevel( 0, &pSurf );
			m_aRtTable[0].pRT[0] = pSurf;
			m_pSceneSave[1]->GetSurfaceLevel( 0, &pSurf );
			m_aRtTable[0].pRT[1] = pSurf;
			m_aRtTable[0].pRT[2] = NULL;  // RT 2 of pass 0 not used
			m_pSceneSave[2]->GetSurfaceLevel( 0, &pSurf );
			m_aRtTable[1].pRT[0] = pSurf;
			// RT 1 & 2 of pass 1 not used
			m_aRtTable[1].pRT[1] = NULL;
			m_aRtTable[1].pRT[2] = NULL;
			// Pass 2 not used
			m_aRtTable[2].pRT[0] = NULL;
			m_aRtTable[2].pRT[1] = NULL;
			m_aRtTable[2].pRT[2] = NULL;
			break;
		case 3:
			m_pSceneSave[0]->GetSurfaceLevel( 0, &pSurf );
			m_aRtTable[0].pRT[0] = pSurf;
			// RT 1 & 2 of pass 0 not used
			m_aRtTable[0].pRT[1] = NULL;
			m_aRtTable[0].pRT[2] = NULL;
			m_pSceneSave[1]->GetSurfaceLevel( 0, &pSurf );
			m_aRtTable[1].pRT[0] = pSurf;
			// RT 1 & 2 of pass 1 not used
			m_aRtTable[1].pRT[1] = NULL;
			m_aRtTable[1].pRT[2] = NULL;
			m_pSceneSave[2]->GetSurfaceLevel( 0, &pSurf );
			m_aRtTable[2].pRT[0] = pSurf;
			// RT 1 & 2 of pass 2 not used
			m_aRtTable[2].pRT[1] = NULL;
			m_aRtTable[2].pRT[2] = NULL;
			break;
	}*/

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: PerformSinglePostProcess()
// Desc: Perform post-process by setting the previous render target as a
//       source texture and rendering a quad with the post-process technique
//       set.
//       This method changes render target without saving any. The caller
//       should ensure that the default render target is saved before calling
//       this.
//       When this method is invoked, m_dwNextTarget is the index of the
//       rendertarget of this post-process.  1 - m_dwNextTarget is the index
//       of the source of this post-process.
HRESULT CPostProcessManager::PerformSinglePostProcess( IDirect3DDevice9 *pd3dDevice,
                                  CPostProcess &PP,
                                  CPProcInstance &Inst,
                                  IDirect3DVertexBuffer9 *pVB,
                                  PPVERT *aQuad,
                                  float &fExtentX,
                                  float &fExtentY )
{
    HRESULT hr;

    //
    // The post-process effect may require that a copy of the
    // originally rendered scene be available for use, so
    // we initialize them here.
    //

    for( int i = 0; i < RT_COUNT; ++i )
        PP.m_pEffect->SetTexture( PP.m_hTexScene[i], m_pSceneSave[i] );

    //
    // If there are any parameters, initialize them here.
    //

	for( int i = 0; i < CPostProcess::NUM_PARAMS; ++i )
        if( PP.m_ahParam[i] )
            PP.m_pEffect->SetVector( PP.m_ahParam[i], &Inst.m_avParam[i] );

    // Render the quad
    if( true /*SUCCEEDED( pd3dDevice->BeginScene() )*/ )
    {
        V( PP.m_pEffect->SetTechnique( "PostProcess" ) );

        // Set the vertex declaration
        hr = pd3dDevice->SetVertexDeclaration( m_pVertDeclPP );

        // Draw the quad
        UINT cPasses, p;
        PP.m_pEffect->Begin( &cPasses, 0 );
        for( p = 0; p < cPasses; ++p )
        {
            bool bUpdateVB = false;  // Inidicates whether the vertex buffer
                                     // needs update for this pass.

            //
            // If the extents has been modified, the texture coordinates
            // in the quad need to be updated.
            //

            if( aQuad[1].tu != fExtentX )
            {
                aQuad[1].tu = aQuad[3].tu = fExtentX;
                bUpdateVB = true;
            }
            if( aQuad[2].tv != fExtentY )
            {
                aQuad[2].tv = aQuad[3].tv = fExtentY;
                bUpdateVB = true;
            }

            //
            // Check if the pass has annotation for extent info.  Update
            // fScaleX and fScaleY if it does.  Otherwise, default to 1.0f.
            //

			float fScaleX = 1.0f, fScaleY = 1.0f;
/*			D3DXHANDLE hPass = PP.m_pEffect->GetPass( PP.m_hTPostProcess, p );
			D3DXHANDLE hExtentScaleX = PP.m_pEffect->GetAnnotationByName( hPass, "fScaleX" );
			if( hExtentScaleX )
				PP.m_pEffect->GetFloat( hExtentScaleX, &fScaleX );
			D3DXHANDLE hExtentScaleY = PP.m_pEffect->GetAnnotationByName( hPass, "fScaleY" );
			if( hExtentScaleY )
				PP.m_pEffect->GetFloat( hExtentScaleY, &fScaleY );
*/
			fScaleX = Inst.m_fScaleX;
			fScaleY = Inst.m_fScaleY;

            //
            // Now modify the quad according to the scaling values specified for
            // this pass
            //
            if( fScaleX != 1.0f )
            {
                aQuad[1].x = (aQuad[1].x + 0.5f) * fScaleX - 0.5f;
                aQuad[3].x = (aQuad[3].x + 0.5f) * fScaleX - 0.5f;
                bUpdateVB = true;
            }
            if( fScaleY != 1.0f )
            {
                aQuad[2].y = (aQuad[2].y + 0.5f) * fScaleY - 0.5f;
                aQuad[3].y = (aQuad[3].y + 0.5f) * fScaleY - 0.5f;
                bUpdateVB = true;
            }

            if( bUpdateVB )
            {
                LPVOID pVBData;
                // Scaling requires updating the vertex buffer.
                if( SUCCEEDED( pVB->Lock( 0, 0, &pVBData, D3DLOCK_DISCARD ) ) )
                {
                    CopyMemory( pVBData, aQuad, 4 * sizeof(PPVERT) );
                    pVB->Unlock();
                }
            }
            fExtentX *= fScaleX;
            fExtentY *= fScaleY;
			//
            // Set up the textures and the render target
            //

			// set up the texture for the source image
            for( int i = 0; i < RT_COUNT; ++i )
            {
                // If this is the very first post-process rendering,
                // obtain the source textures from the scene.
                // Otherwise, initialize the post-process source texture to
                // the previous render target.
                //
                if( m_RTChain[i].m_bFirstRender )
                    hr = PP.m_pEffect->SetTexture( PP.m_hTexSource[i], m_pSceneSave[i] );
                else
                    hr = PP.m_pEffect->SetTexture( PP.m_hTexSource[i], m_RTChain[i].GetNextSource() );
            }

			m_pEffect->CommitChanges();

            //
            // Set up the new render target
            //
            IDirect3DTexture9 *pTarget = m_RTChain[PP.m_nRenderTarget].GetNextTarget();
            IDirect3DSurface9 *pTexSurf;
            hr = pTarget->GetSurfaceLevel( 0, &pTexSurf );
            if( FAILED( hr ) )
				return hr;	// return DXUT_ERR( L"GetSurfaceLevel", hr );

            hr = pd3dDevice->SetRenderTarget( 0, pTexSurf );
            pTexSurf->Release();
            // We have output to this render target. Flag it.
            m_RTChain[PP.m_nRenderTarget].m_bFirstRender = false;

            //
            // Clear the render target
            //
            pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L );
            //
            // Render
            //
            PP.m_pEffect->BeginPass( p );
            pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof(PPVERT) );
            pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
            PP.m_pEffect->EndPass();

            // Update next rendertarget index
            m_RTChain[PP.m_nRenderTarget].Flip();
        }
        PP.m_pEffect->End();

        // End scene
        // pd3dDevice->EndScene();
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
// PerformPostProcess()
// Perform all active post-processes in order.
HRESULT CPostProcessManager::PerformPostProcess()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	HRESULT hr;

	//
	// Swap the chains
	//
	int i;
	for(i = 0; i < RT_COUNT; ++i )
		m_RTChain[i].Flip();

	// Reset all render targets used besides RT 0
	for( i = 1; i < m_nRtUsed; ++i )
		V( pd3dDevice->SetRenderTarget( i, NULL ) );

	//
	// Extents are used to control how much of the rendertarget is rendered
	// during postprocess. For example, with the extent of 0.5 and 0.5, only
	// the upper left quarter of the rendertarget will be rendered during
	// postprocess.
	//
	float fExtentX = 1.0f, fExtentY = 1.0f;
	//	const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
	IDirect3DSurface9 *pBackBuffer;
	pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	D3DSURFACE_DESC back_bufffer_desc;
	pBackBuffer->GetDesc( &back_bufffer_desc );

	//
	// Set up our quad
	//
	PPVERT Quad[4] =
	{
		{ -0.5f,                        -0.5f,                          1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{ back_bufffer_desc.Width-0.5f, -0.5,                           1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f },
		{ -0.5,                          back_bufffer_desc.Height-0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ back_bufffer_desc.Width-0.5f,  back_bufffer_desc.Height-0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }
	};

	//
	// Create a vertex buffer out of the quad
	//
	IDirect3DVertexBuffer9 *pVB;
	hr = pd3dDevice->CreateVertexBuffer( sizeof(PPVERT) * 4,
										 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
										 0,
										 D3DPOOL_DEFAULT,
										 &pVB,
										 NULL );
	if( FAILED( hr ) )
	{
		assert( !"CreateVertexBuffer failed()" );
		return hr;
	}
//        return DXUT_ERR( L"CreateVertexBuffer", hr );

	// Fill in the vertex buffer
	LPVOID pVBData;
	if( SUCCEEDED( pVB->Lock( 0, 0, &pVBData, D3DLOCK_DISCARD ) ) )
	{
		CopyMemory( pVBData, Quad, sizeof(Quad) );
		pVB->Unlock();
	}

	// Clear first-time render flags
	for( int i = 0; i < RT_COUNT; ++i )
		m_RTChain[i].m_bFirstRender = true;

	// Perform post processing

	// The last (blank) item has special purpose so do not process it.
	size_t num_ppinstances = m_vecPPInstance.size();
	for( size_t nEffIndex = 0; nEffIndex < num_ppinstances; ++nEffIndex )
	{
		CPProcInstance& rInstance = m_vecPPInstance[nEffIndex];

		if( rInstance.m_nFxIndex < 0 )
		{
			continue;
//				assert( !"an invalid post-process instance (nEffIndex < 0)" );
		}

		PerformSinglePostProcess( pd3dDevice,
                                m_aPostProcess[rInstance.m_nFxIndex],
                                rInstance,
                                pVB,
                                Quad,
                                fExtentX,
                                fExtentY );
    }

    // Release the vertex buffer
    pVB->Release();

    return S_OK;
}


/*
   /// ========= using CPostProcessManager in the rendering routine =========

    HRESULT hr;
    UINT cPass, p;

	// Clear the render target and the zbuffer 
	V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0) );

	/// CPostProcessManager::SetTextureRenderTarget() does not save & restore the original render target
	/// the user is responsible for saving & restoring the render target

	// save render target 0 so we can restore it later
    IDirect3DSurface9 *pOldRT;
    pd3dDevice->GetRenderTarget( 0, &pOldRT );


	/// in order to use post-process functionality, you need to call CPostProcessManager::SetTextureRenderTarget( pass )
	/// at the beginning of each pass.
    LPD3DXMESH pMeshObj = g_SceneMesh[m_nScene].GetMesh();
    V( g_pEffect->Begin( &cPass, 0 ) );
    for( p = 0; p < cPass; ++p )
    {
        /// Set the render target(s) for this pass
		g_pPPManager->SetTextureRenderTarget( p );

        V( g_pEffect->BeginPass( p ) );

        // Iterate through each subset and render with its texture
        for( DWORD m = 0; m < g_SceneMesh[m_nScene].m_dwNumMaterials; ++m )
        {
            V( g_pEffect->SetTexture( "g_txScene", g_SceneMesh[m_nScene].m_pTextures[m] ) );
            V( g_pEffect->CommitChanges() );
            V( pMeshObj->DrawSubset( m ) );
        }

        V( g_pEffect->EndPass() );
    }
    V( g_pEffect->End() );


	/// actually, when you are only interested in color channel-related pp effects,
	/// call the CPostProcessManager::SetTextureRenderTarget( 0 ) at the beginning of the rendering
	g_pPPManager->SetTextureRenderTarget( 0 );
    V( g_pEffect->Begin( &cPass, 0 ) );
    for( p = 0; p < cPass; ++p )
    {
        V( g_pEffect->BeginPass( p ) );
		...
        V( g_pEffect->EndPass() );
    }
    V( g_pEffect->End() );



    // Perform post-processes
	g_pPPManager->PerformPostProcess( pd3dDevice );

    // Restore old render target 0 (back buffer)
    V( pd3dDevice->SetRenderTarget( 0, pOldRT ) );
    SAFE_RELEASE( pOldRT );

	// render the result of the post-processed image
	g_pPPManager->RenderPostProcess( pd3dDevice );
*/



// Get the final result image onto the backbuffer
//HRESULT CPostProcessManager::RenderPostProcess( IDirect3DDevice9 *pd3dDevice )

/// render the textured rectangle of the scene with post process effects
HRESULT CPostProcessManager::DrawSceneWithPostProcessEffects()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	HRESULT hr;

//	if( GetNumPostProcesses() == 0 )
//		return E_FAIL;

//	const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
	IDirect3DSurface9 *pBackBuffer;
	hr = pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	D3DSURFACE_DESC back_bufffer_desc;
	hr = pBackBuffer->GetDesc( &back_bufffer_desc );

//	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
//	{
		// Render a screen-sized quad
		PPVERT quad[4] =
		{
			{ -0.5f,                          -0.5f,                            0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
			{ back_bufffer_desc.Width - 0.5f, -0.5f,                            0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f },
			{ -0.5f,                           back_bufffer_desc.Height - 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f },
			{ back_bufffer_desc.Width - 0.5f,  back_bufffer_desc.Height - 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f }
		};
		V( pd3dDevice->SetVertexDeclaration( m_pVertDeclPP ) );

		// set a post-processed scene as a texture for the quad
		IDirect3DTexture9 *pPrevTarget;
		size_t i, num = m_vecPPInstance.size();
		for( i=0; i<num; i++ )
			if( 0 <= m_vecPPInstance[i].m_nFxIndex ) break;

		pPrevTarget = ( i < num ) ? m_RTChain[0].GetPrevTarget() : m_pSceneSave[0];

		// set a technique to render a lit, textured geometry
		V( m_pEffect->SetTechnique( m_hTRenderNoLight ) );

		V( m_pEffect->SetTexture( "g_txScene", pPrevTarget ) );

		m_pEffect->CommitChanges();

		UINT p, cPasses;
		V( m_pEffect->Begin( &cPasses, 0 ) );
		for( p = 0; p < cPasses; ++p )
		{
			V( m_pEffect->BeginPass( p ) );
			V( pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, quad, sizeof(PPVERT) ) );
			V( m_pEffect->EndPass() );
		}
		V( m_pEffect->End() );

		// debug
		C2DRect rect( 0, 0, 100, 100, 0xFF0000FF );
		rect.Draw();

//		V( pd3dDevice->EndScene() );
//	}
//	else
//	{
//		assert( !"BeginScene() failed." );
//	}

	return S_OK;
}


/*
//--------------------------------------------------------------------------------------
// Inserts the postprocess effect identified by the index nEffectIndex into the
// active list.
void InsertEffect( int nEffectIndex )
{
    int nInsertPosition = g_SampleUI.GetListBox( IDC_ACTIVELIST )->GetSelectedIndex();
    if( nInsertPosition == -1 )
        nInsertPosition = g_SampleUI.GetListBox( IDC_ACTIVELIST )->GetSize() - 1;

    // Create a new CPProcInstance object and set it as the data field of the
    // newly inserted item.
    CPProcInstance *pNewInst = new CPProcInstance;

    if( pNewInst )
    {
        pNewInst->m_nFxIndex = nEffectIndex;
        ZeroMemory( pNewInst->m_avParam, sizeof( pNewInst->m_avParam ) );
        for( int p = 0; p < NUM_PARAMS; ++p )
            pNewInst->m_avParam[p] = m_aPostProcess[pNewInst->m_nFxIndex].m_avParamDef[p];

        g_SampleUI.GetListBox( IDC_ACTIVELIST )->InsertItem( nInsertPosition, g_aszPpDesc[nEffectIndex], pNewInst );

        // Set selection to the item after the inserted one.
        int nSelected = g_SampleUI.GetListBox( IDC_ACTIVELIST )->GetSelectedIndex();
        if( nSelected >= 0 )
            g_SampleUI.GetListBox( IDC_ACTIVELIST )->SelectItem( nSelected + 1 );
    }
}*/

/*
//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
        case IDC_ACTIVELIST:
        {
            switch( nEvent )
            {
                case EVENT_LISTBOX_SELECTION:
                {
                    // Selection changed in the active list.  Update the parameter
                    // controls.

                    int nSelected = ((CDXUTListBox*)pControl)->GetSelectedIndex();

                    if( nSelected >= 0 && nSelected < (int)((CDXUTListBox*)pControl)->GetSize() - 1 )
                    {
                        DXUTListBoxItem *pItem = ((CDXUTListBox*)pControl)->GetSelectedItem();
                        CPProcInstance *pInstance = (CPProcInstance *)pItem->pData;
                        CPostProcess &PP = m_aPostProcess[pInstance->m_nFxIndex];

                        if( pInstance && PP.m_awszParamName[0][0] != L'\0' )
                        {
                            g_SampleUI.GetStatic( IDC_PARAM0NAME )->SetText( PP.m_awszParamName[0] );

                            // Fill the editboxes with the parameter values
                            for( int p = 0; p < NUM_PARAMS; ++p )
                            {
                                if( PP.m_awszParamName[p][0] != L'\0' )
                                {
                                    // Enable the label and editbox for this parameter
                                    g_SampleUI.GetControl( IDC_PARAM0 + p )->SetEnabled( true );
                                    g_SampleUI.GetControl( IDC_PARAM0 + p )->SetVisible( true );
                                    g_SampleUI.GetControl( IDC_PARAM0NAME + p )->SetEnabled( true );
                                    g_SampleUI.GetControl( IDC_PARAM0NAME + p )->SetVisible( true );

                                    WCHAR wszParamText[512] = L"";

                                    for( int i = 0; i < PP.m_anParamSize[p]; ++i )
                                    {
                                        WCHAR wsz[512];
                                        StringCchPrintf( wsz, 512, L"%.5f ", pInstance->m_avParam[p][i] );
                                        StringCchCat( wszParamText, 512, wsz );
                                    }

                                    // Remove trailing space
                                    if( wszParamText[lstrlenW(wszParamText)-1] == L' ' )
                                        wszParamText[lstrlenW(wszParamText)-1] = L'\0';
                                    g_SampleUI.GetEditBox( IDC_PARAM0 + p )->SetText( wszParamText );
                                }
                            }
                        } else
                        {
                            g_SampleUI.GetStatic( IDC_PARAM0NAME )->SetText( L"Selected effect has no parameters." );

                            // Disable the edit boxes and 2nd parameter static
                            g_SampleUI.GetControl( IDC_PARAM0 )->SetEnabled( false );
                            g_SampleUI.GetControl( IDC_PARAM0 )->SetVisible( false );
                            g_SampleUI.GetControl( IDC_PARAM1 )->SetEnabled( false );
                            g_SampleUI.GetControl( IDC_PARAM1 )->SetVisible( false );
                            g_SampleUI.GetControl( IDC_PARAM1NAME )->SetEnabled( false );
                            g_SampleUI.GetControl( IDC_PARAM1NAME )->SetVisible( false );
                        }
                    } else
                    {
                        g_SampleUI.GetStatic( IDC_PARAM0NAME )->SetText( L"Select an active effect to set its parameter." );

                        // Disable the edit boxes and 2nd parameter static
                        g_SampleUI.GetControl( IDC_PARAM0 )->SetEnabled( false );
                        g_SampleUI.GetControl( IDC_PARAM0 )->SetVisible( false );
                        g_SampleUI.GetControl( IDC_PARAM1 )->SetEnabled( false );
                        g_SampleUI.GetControl( IDC_PARAM1 )->SetVisible( false );
                        g_SampleUI.GetControl( IDC_PARAM1NAME )->SetEnabled( false );
                        g_SampleUI.GetControl( IDC_PARAM1NAME )->SetVisible( false );
                    }

                    break;
                }
            }
            break;
        }

        case IDC_PARAM0:
        case IDC_PARAM1:
        {
            if( nEvent == EVENT_EDITBOX_CHANGE )
            {
                int nParamIndex;
                switch( nControlID )
                {
                    case IDC_PARAM0: nParamIndex = 0; break;
                    case IDC_PARAM1: nParamIndex = 1; break;
                    default: return;
                }

                DXUTListBoxItem *pItem = g_SampleUI.GetListBox( IDC_ACTIVELIST )->GetSelectedItem();
                CPProcInstance *pInstance = NULL;
                if( pItem )
                    pInstance = (CPProcInstance *)pItem->pData;

                if( pInstance )
                {
                    D3DXVECTOR4 v;
                    ZeroMemory( &v, sizeof( v ) );
                    ParseFloatList( ((CDXUTEditBox *)pControl)->GetText(), (float*)&v );

                    // We parsed the values. Now save them in the instance data.
                    pInstance->m_avParam[nParamIndex] = v;
                }
            }
            break;
        }

        case IDC_PREBLUR:
        {
            // Clear the list
            ClearActiveList();

            // Insert effects
            InsertEffect( 9 );
            InsertEffect( 2 );
            InsertEffect( 3 );
            InsertEffect( 2 );
            InsertEffect( 3 );
            InsertEffect( 10 );
            break;
        }

        case IDC_PREBLOOM:
        {
            // Clear the list
            ClearActiveList();

            // Insert effects
            InsertEffect( 9 );
            InsertEffect( 9 );
            InsertEffect( 6 );
            InsertEffect( 4 );
            InsertEffect( 5 );
            InsertEffect( 4 );
            InsertEffect( 5 );
            InsertEffect( 10 );
            InsertEffect( 12 );
            break;
        }

    }
}*/



//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// entered a lost state and before IDirect3DDevice9::Reset is called. Resources created
// in the OnResetDevice callback should be released here, which generally includes all 
// D3DPOOL_DEFAULT resources. See the "Lost Devices" section of the documentation for 
// information about lost devices.
//--------------------------------------------------------------------------------------
//void CALLBACK OnLostDevice( void* pUserContext )
/*
void CPostProcessManager::OnLostDevice( void* pUserContext )
{
    if( m_pEffect )
        m_pEffect->OnLostDevice();

	int num_pps = GetNumPostProcesses();
    for( int p = 0; p < num_pps; ++p )
        m_aPostProcess[p].OnLostDevice();

    // Release the scene save and render target textures
    for( int i = 0; i < RT_COUNT; ++i )
    {
        SAFE_RELEASE( m_pSceneSave[i] );
        m_RTChain[i].Cleanup();
    }

    // Release the RT table's references
    for( int p = 0; p < RT_COUNT; ++p )
        for( int rt = 0; rt < RT_COUNT; ++rt )
            SAFE_RELEASE( m_aRtTable[p].pRT[rt] );

}
*/

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// been destroyed, which generally happens as a result of application termination or 
// windowed/full screen toggles. Resources created in the OnCreateDevice callback 
// should be released here, which generally includes all D3DPOOL_MANAGED resources. 
//--------------------------------------------------------------------------------------
void CPostProcessManager::OnDestroyDevice()
{
    SAFE_RELEASE( m_pEffect );
    SAFE_RELEASE( m_pVertDeclPP );

	int num_pps = GetNumPostProcesses();
	for( int p = 0; p < num_pps; ++p )
		m_aPostProcess[p].Cleanup();
}


void CPostProcessManager::ReleaseGraphicsResources()
{
	OnDestroyDevice();
}


void CPostProcessManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	OnCreateDevice( m_ShaderFilename );

	int num_pps = GetNumPostProcesses();
//	for( int p = 0; p < num_pps; ++p )
//		m_aPostProcess[p].Reload();
}
