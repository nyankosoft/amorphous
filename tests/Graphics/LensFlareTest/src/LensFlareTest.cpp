#include "LensFlareTest.h"
#include "3DMath/Matrix34.h"
#include "3DCommon/all.h"

using namespace std;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new CLensFlareTest();
}


extern const std::string GetAppTitle()
{
	return string("Lens Flare Test");
}

/*
void CLensFlareTestInputHandler::ProcessInput(SInputData& input)
{
	switch( input.iGICode )
	{
	case '1':
	case 'R':
		m_pSynthTest->m_pSynthesizer->SetRootPose( Matrix34Identity() );
		break;

	default:
		break;
	}
}
*/

CLensFlareTest::CLensFlareTest()
{
//	m_pInputHandler = CInputHandlerSharedPtr( new CLensFlareTestInputHandler(this) );

//	INPUTHUB.SetInputHandler( m_pInputHandler.get() );
}


CLensFlareTest::~CLensFlareTest()
{
}


void CLensFlareTest::InitLensFlare( const string& strPath )
{
	m_pLensFlare = shared_ptr<CLensFlare>( new CLensFlare() );

//	m_pLensFlare->AddTexture( "./textures/LensFlareTex/flare00.dds", 0, 1, 1 );
	m_pLensFlare->AddTexture( "./data/textures/lens_flares/flare01.dds", 0, 1, 1 );
	m_pLensFlare->AddTexture( "./data/textures/lens_flares/flare02.dds", 1, 1, 1 );
	m_pLensFlare->AddTexture( "./data/textures/lens_flares/flare06.dds", 2, 1, 1 );
//	m_pLensFlare->AddTexture( "./textures/LensFlareTex/flare07.dds", 2, 1, 1 );

	m_pLensFlare->AddLensFlareRect( 0.09f, 1.0f, 0.70f, 0x40FF5030, 0 );
	m_pLensFlare->AddLensFlareRect( 0.08f, 1.0f, 1.50f, 0x28FFD040, 0 );
	m_pLensFlare->AddLensFlareRect( 0.07f, 1.0f, 0.47f, 0x30F03060, 0 );
	m_pLensFlare->AddLensFlareRect( 0.05f, 1.0f, 1.40f, 0x202040F0, 0 );
	m_pLensFlare->AddLensFlareRect( 0.04f, 1.0f, 0.80f, 0x3260D080, 0 );
	m_pLensFlare->AddLensFlareRect( 0.05f, 1.0f, 0.60f, 0x10FF2020, 0 );
	m_pLensFlare->AddLensFlareRect( 0.04f, 1.0f, 0.42f, 0x302020FF, 0 );
	m_pLensFlare->AddLensFlareRect( 0.10f, 1.0f, 0.35f, 0x20FFFFFF, 1 );
	m_pLensFlare->AddLensFlareRect( 0.03f, 1.0f, 1.20f, 0x20C0C0C0, 2 );
	m_pLensFlare->AddLensFlareRect( 0.04f, 1.0f, 0.19f, 0x30F0F0F0, 2 );

//	m_pLensFlare->UpdateScreenSize( GetWindowWidth(), GetWindowHeight() );
}


int CLensFlareTest::Init()
{
	InitLensFlare( "" );

	m_pFont = CFontSharedPtr( new CFont( "Bitstream Vera Sans Mono", 16, 16 ) );

	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NullShader" );

	// initialize shader
	m_pShaderManager = shared_ptr<CShaderManager>( new CShaderManager() );
	m_pShaderManager->LoadShaderFromFile( "./data/shaders/LensFlareTest.fx" );

	// load the terrain mesh
	CMeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = "./data/models/terrain06.msh";
	mesh_desc.MeshType     = CMeshType::BASIC;
	m_TerrainMesh.Load( mesh_desc );

	m_TestTexture.Load( "./data/textures/lens_flares/flare02.dds" );

	m_LightPosIndicator.Init();

	return 0;
}


void CLensFlareTest::Update( float dt )
{
	m_pLensFlare->UpdateLensFlares();

	m_pLensFlare->SetLightPosition( Vector3( 170, 250, 200 ) );
}


void CLensFlareTest::Render()
{
	C2DRect rect( Vector2( 80, 80 ), Vector2( 100, 100 ), 0xFFFF0000 );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	m_pShaderManager->SetWorldTransform( matWorld );

	m_pShaderManager->SetTechnique( m_MeshTechnique );
	
	m_TerrainMesh.GetMesh().get()->Render( *m_pShaderManager );

	// display light position
	Matrix34 light_pose( m_pLensFlare->GetLightPosition(), Matrix33Identity() );
	light_pose.GetRowMajorMatrix44( (Scalar *)&matWorld );
	m_pShaderManager->SetWorldTransform( matWorld );
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matWorld );
	m_LightPosIndicator.Draw();

	// lens flares

	m_pShaderManager->SetTechnique( m_DefaultTechnique );

	rect.Draw();

	m_pLensFlare->Render( *m_pShaderManager );
}


void CLensFlareTest::UpdateViewTransform( const D3DXMATRIX& matView )
{
	m_pLensFlare->UpdateViewTransform( matView );
}


void CLensFlareTest::UpdateProjectionTransform( const D3DXMATRIX& matProj )
{
	m_pLensFlare->UpdateProjectionTransform( matProj );
}


#if 0

//-----------------------------------------------------------------------------
// File: D3DAppTestMain.cpp
//-----------------------------------------------------------------------------

#include "3DCommon/all.h"

#include "GameCommon/Timer.h"

#include "Support/CameraController_Win32.h"
#include "Support/FileOpenDialog_Win32.h"
#include "Support/fnop.h"
#include "Support/memory_helpers.h"

#include "SGPreviewMeshExporter.h"

#include <stdio.h>

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

CFont g_Font;

CCameraController_Win32 g_CameraController;


void InitSkySphere( const string& strPath )
{
	g_pSkySphere = new CD3DXMeshObject;
	string filename = strPath + "sphere.x";
	HRESULT hr = g_pSkySphere->LoadFromFile( filename.c_str() );

	if( FAILED(hr) )
	{
		MessageBox( NULL, "skybox mesh was not found", "error", MB_OK|MB_ICONWARNING );
	}

//	MessageBox( NULL, filename.c_str(), "filename", MB_OK|MB_ICONWARNING );

	filename = strPath + "sky.dds";
	hr = D3DXCreateTextureFromFile( DIRECT3D9.GetDevice(), filename.c_str(), &g_pSkyTexture );

	if( FAILED(hr) )
		MessageBox( NULL, "unable to load a texture file", "error", MB_OK|MB_ICONWARNING );
}


void RenderSkySphere()
{
	if( !g_pSkySphere || !g_pSkySphere->GetBaseMesh() || !g_pSkyTexture )
	{
		MessageBox( NULL, "no skybox mesh / texture", "error", MB_OK|MB_ICONWARNING );
		assert( !"no sky object" );
		return;
	}

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	const D3DXVECTOR3 vPos = g_CameraController.GetPosition();
	matWorld._41 = vPos.x;
	matWorld._42 = vPos.y;
	matWorld._43 = vPos.z;
	matWorld._44 = 1;

	g_ShaderManager.SetWorldTransform( matWorld );

	HRESULT hr;

	LPD3DXEFFECT pEffect = g_ShaderManager.GetEffect();
	if( !pEffect )
		return;

	hr = pEffect->SetTechnique( "SkyBox" );

	if( FAILED(hr) )
	{
		assert( !"invalid shader technique" );
		return;
	}

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	pd3dDevice->SetFVF( TEXTUREVERTEX::FVF );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	g_ShaderManager.SetTexture( 0, g_pSkyTexture );

	pEffect->CommitChanges();

	UINT cPasses;
	pEffect->Begin( &cPasses, 0 );	

	for( UINT p = 0; p < cPasses; ++p )
	{
		pEffect->BeginPass( p );

		// Draw the mesh subset
		// we assume that the sky box model has only one subset
		g_pSkySphere->GetBaseMesh()->DrawSubset( 0 );

		pEffect->EndPass();
	}

	pEffect->End();

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );


	// render the position of the light to check the lens flare

	memcpy( &(matWorld._41), &g_vLightPosition, sizeof(float) * 3 );
	D3DXMATRIX matScale;
	D3DXMatrixScaling( &matScale, 0.05f, 0.05f, 0.05f );
	matWorld = matScale * matWorld;
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	pEffect->SetTechnique( "NoShader" );
	pd3dDevice->SetTexture( 0, NULL);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetFVF( g_pSkySphere->GetBaseMesh()->GetFVF() );
	pEffect->CommitChanges();
	pEffect->Begin( &cPasses, 0 );	
	for( UINT p = 0; p < cPasses; ++p )
	{
		pEffect->BeginPass( p );
		g_pSkySphere->GetBaseMesh()->DrawSubset( 0 );
		pEffect->EndPass();
	}
	pEffect->End();
}


void RenderLensFlare()
{
	LPD3DXEFFECT pEffect = g_ShaderManager.GetEffect();
	if( !pEffect )
		return;

	HRESULT hr;
	hr = pEffect->SetTechnique( "NullShader" );

	g_LensFlare.UpdateLensFlares();

	g_LensFlare.Render();

/*	UINT cPasses;
	pEffect->Begin( &cPasses, 0 );
	pEffect->BeginPass( 0 );
	
	g_LensFlare.Render();

	pEffect->EndPass();
	pEffect->End();*/
}


void LoadXMeshForTest( const char *pFilename )
{
	g_vecpMeshObject.resize( 1, NULL );
	g_vecpMeshObject[0] = new CD3DXMeshObject;
	g_vecpMeshObject[0]->LoadFromFile( pFilename );
}


bool CreateMeshes( const char *pFilename )
{
	CSGPreviewMeshExporter mesh_exporter;

	int num_meshes;

	if( strncmp( pFilename + strlen(pFilename) - 4, ".lwo", 4 ) == 0 )
	{
		if( !mesh_exporter.BuildMeshModels( pFilename ) )
			return false;

		num_meshes = mesh_exporter.GetNumMeshesCreated();
	}
	else
	{
		// load already compiled mesh archives that has successive numbered filenames
		// keep loading until no further numbered filenames are found
		num_meshes = 99;
	}

	HRESULT hr;

	char acFilename[512];
	for( int i=0; i<num_meshes; i++ )
	{
		sprintf( acFilename, "layer%02d.msh", i );

//		g_vecpMeshObject.push_back( new CD3DXSMeshObject );
		g_vecpMeshObject.push_back( new CD3DXMeshObject );

		hr = g_vecpMeshObject.back()->LoadFromFile( acFilename );

		if( FAILED(hr) )
		{
//			MessageBox( NULL, "unable to load a mesh file", "error", MB_OK|MB_ICONWARNING );
			SafeDelete( g_vecpMeshObject.back() );
			g_vecpMeshObject.erase( g_vecpMeshObject.begin() + i ); 
			if( i == 0 )
                return false;
			else
				return true;
		}
	}

	return true;
}


void RenderMeshes()
{
	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	// alpha-blending settings 
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,  (DWORD)0x00000001 );
	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );	// draw a pixel if its alpha value is greater than or equal to '0x00000001'

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	g_ShaderManager.SetWorldTransform( matWorld );

	LPD3DXEFFECT pEffect = g_ShaderManager.GetEffect();
	if( !pEffect )
		return;


	D3DXATTRIBUTERANGE attrib_table[128];
	DWORD attrib_table_size;
	if( g_vecpMeshObject[0] && g_vecpMeshObject[0]->GetBaseMesh() )
		g_vecpMeshObject[0]->GetBaseMesh()->GetAttributeTable( attrib_table, &attrib_table_size );


	hr = pEffect->SetValue( "g_vEyePos", &(g_CameraController.GetPosition()), sizeof(float) * 3 );

//	hr = pEffect->SetTechnique( "Default" );
//	hr = pEffect->SetTechnique( "NullShader" );
	hr = pEffect->SetTechnique( "QuickTest" );

	if( FAILED(hr) )
		return;

	UINT cPasses;
	pEffect->Begin( &cPasses, 0 );


	size_t i, num_meshes = g_vecpMeshObject.size();
	for( i=0; i<num_meshes; i++ )
	{
//		CD3DXSMeshObject *pMeshObject = g_vecpMeshObject[i];
		CD3DXMeshObject *pMeshObject = g_vecpMeshObject[i];

		if( !pMeshObject )
			continue;

	//	LPD3DXPMESH pPMesh = pMeshObject->GetPMesh();
		LPD3DXBASEMESH pMesh = pMeshObject->GetBaseMesh();
		if( !pMesh )
			return;


		pMesh->GetAttributeTable( attrib_table, &attrib_table_size );


		pd3dDevice->SetVertexDeclaration( pMeshObject->GetVertexDeclaration() );

		LPDIRECT3DTEXTURE9 pTex;

		// Meshes are divided into subsets by materials. Render each subset in a loop
		DWORD mat, num_materials = pMeshObject->GetNumMaterials();
		for( mat=0; mat<num_materials; mat++ )
		{
			// Set the material and texture for this subset
	//		pd3dDevice->SetMaterial( &pMeshObject->GetMaterial(i) );

			pTex = pMeshObject->GetTexture(mat);
			if( pTex )
                g_ShaderManager.SetTexture( 0, pTex );
			else
				g_ShaderManager.SetTexture( 0, g_pDefaultTexture );

/*			LPDIRECT3DTEXTURE9 pNMTex = NULL;
			if( pNMTex = pMeshObject->GetNormalMapTexture(i) )
				g_ShaderManager.SetTexture( 1, pNMTex );
*/
			pEffect->CommitChanges();

			for( UINT p = 0; p < cPasses; ++p )
			{
				pEffect->BeginPass( p );

				// Draw the mesh subset
				pMesh->DrawSubset( mat );

				pEffect->EndPass();
			}
		}
	}

	pEffect->End();
}


void ReleaseGraphicsResources()
{
	size_t i, num_meshes = g_vecpMeshObject.size();

	for( i=0; i<num_meshes; i++ )
	{
		SafeDelete( g_vecpMeshObject[i] );
	}

	g_vecpMeshObject.clear();

	SAFE_RELEASE( g_pDefaultTexture );

	g_LensFlare.Release();
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );


    // clear the backbuffer to a blue color
    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

    // begin the scene
    pd3dDevice->BeginScene();

	if( CShader::Get()->GetCurrentShaderManager() )
	{
		g_ShaderManager.SetViewTransform( g_CameraController.GetCameraMatrix() );
		g_LensFlare.UpdateViewTransform( g_CameraController.GetCameraMatrix() );

		RenderSkySphere();

		RenderMeshes();

		RenderLensFlare();
	}

	// rendering
	char acStr[256];
	sprintf( acStr, "%f", TIMER.GetFPS() );
	g_Font.DrawText( acStr, D3DXVECTOR2(20,20), 0xFFFFFFFF );

    // end the scene
    pd3dDevice->EndScene();

    // present the backbuffer contents to the display
    pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	g_CameraController.HandleMessage( hWnd, msg, wParam, lParam );

	D3DXMATRIX matWorld, matProj;

	switch( msg )
	{
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;
		case WM_KEYDOWN:
			switch( wParam )
			{
			case VK_ADD:
				g_FOV -= 0.01f;
				if( g_FOV <= 0.01f )
					g_FOV = 0.0f;
				D3DXMatrixPerspectiveFovLH( &matProj, g_FOV, 640.0f / 480.0f, 0.5f, 200.0f );
				DIRECT3D9.GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );
				g_ShaderManager.SetProjectionTransform( matProj );
				break;
			case VK_SUBTRACT:
				g_FOV += 0.01f;
				D3DXMatrixPerspectiveFovLH( &matProj, g_FOV, 640.0f / 480.0f, 0.5f, 200.0f );
				DIRECT3D9.GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );
				g_ShaderManager.SetProjectionTransform( matProj );
				break;
			}
			break;
		default:
			break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


bool Init()
{
	if( !g_ShaderManager.LoadShaderFromFile( "StaticGeometryViewer.fx" ) )
		return false;

	CShader::Get()->SetShaderManager( &g_ShaderManager );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	g_ShaderManager.SetWorldTransform( matWorld );

    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 640.0f / 480.0f, 0.5f, 320.0f );
    DIRECT3D9.GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );
	g_ShaderManager.SetProjectionTransform( matProj );

	g_LensFlare.UpdateProjectionTransform( matProj );

	g_ShaderLightManager.Init();

	CHemisphericDirLight light;
	light.UpperColor.SetRGBA( 1.00f, 1.00f, 1.00f, 1.00f );
	light.LowerColor.SetRGBA( 0.25f, 0.25f, 0.25f, 1.00f );
	Vector3 vDir = Vector3(1.0f,-3.0f,1.5f);
	Vec3Normalize( vDir, vDir );
	light.vDirection = vDir;
	g_ShaderLightManager.SetLight( 0, light );

//	g_vLightPosition = - vDir * 20.0f;
//	g_vLightPosition = D3DXVECTOR3(0,30,0);
	g_vLightPosition = D3DXVECTOR3(-250,60,-500) * 500.0f;
	g_LensFlare.SetLightPosition( g_vLightPosition );

	DWORD dwColor = 0xFFF0F0F0;
//	DWORD dwColor = 0xFF8080FF;
	CTextureTool::CreateTexture( &dwColor, 1, 1, &g_pDefaultTexture );

	return true;
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, INT )
{
    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "D3D Test", NULL };
    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( "D3D Test", "D3D Application Test",
                              WS_OVERLAPPEDWINDOW, 100, 100, 640, 480,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );


    // Show the window
    ShowWindow( hWnd, SW_SHOWDEFAULT );
    UpdateWindow( hWnd );

	// Initialize Direct3D
	if( !DIRECT3D9.InitD3D( hWnd ) )
		return 0;

	if( !Init() )
		return 0;

	string filename;
	if( !GetFilename( filename ) )
		return 0;

	if( !CreateMeshes( filename.c_str() ) )
	{
		ReleaseGraphicsResources();
		return 0;
	};

	string strPath;
	fnop::get_path( strPath, filename );

	// load the sky sphere model & the sky texture
	InitSkySphere( strPath + "\\" );

	// load textures for lens flare
	InitLensFlare( strPath + "\\" );

	// init font
	g_Font.InitFont( "ÇlÇr ÉSÉVÉbÉN", 8, 16 );

	// Enter the message loop
	MSG msg;
	ZeroMemory( &msg, sizeof(msg) );

	while( msg.message!=WM_QUIT )
	{
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
		{
			TIMER.UpdateFrameTime();

			g_CameraController.UpdateCameraPosition( TIMER.GetFrameTime() );
			g_CameraController.SetCameraMatrix();

			Render();

			Sleep( 5 );
		}
    }


    // Clean up everything and exit the app
    UnregisterClass( "D3D Test", wc.hInstance );
    return 0;
}

#endif