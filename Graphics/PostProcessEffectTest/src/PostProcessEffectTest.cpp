#include "PostProcessEffectTest.hpp"
#include "gds/Graphics/Mesh/BasicMesh.hpp"
#include "gds/Graphics/MeshGenerators.hpp"
#include "gds/Graphics/GraphicsResourceManager.hpp"
#include "gds/Graphics/Shader/ShaderManager.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Graphics/SkyboxMisc.hpp"
#include "gds/Support/CameraController_Win32.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Input.hpp"

using std::string;
using namespace boost;


extern CPlatformDependentCameraController g_CameraController;


const std::string GetAppTitle()
{
	return string("PostProcessEffectTest");
}


CGraphicsTestBase *CreateTestInstance()
{
	return new CPostProcessEffectTest();
}

/*
void CPostProcessEffectTestInputHandler::ProcessInput( SInputData& input )
{
	m_pTest->ProcessInput( input );
}
*/

void CPostProcessEffectTest::HandleInput( const SInputData& input )
{
	switch( input.iGICode )
	{
	case GIC_F6:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			CFilter::ms_SaveFilterResultsAtThisFrame = 1;
		}
		break;

	case GIC_F9:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			GraphicsResourceManager().Refresh();
//			m_pPostProcessEffectManager->InitHDRLightingFilter();
		}
		break;

//	case 'R':
//		m_pSynthTest->m_pSynthesizer->SetRootPose( Matrix34Identity() );
		break;

	case 'H':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( !m_pPostProcessEffectManager )
				return;

			if( !m_pPostProcessEffectManager->IsEnabled(CPostProcessEffect::TF_HDR_LIGHTING) )
			{
				m_pPostProcessEffectManager->EnableHDRLighting( true );
				m_pPostProcessEffectManager->SetHDRLightingParams(
					CHDRLightingParams::KEY_VALUE
				   |CHDRLightingParams::TONE_MAPPING
				   |CHDRLightingParams::LUMINANCE_ADAPTATION_RATE,
					m_HDRLightingParams
					);
			}
			else
				m_pPostProcessEffectManager->EnableHDRLighting( false );
		}
		break;

	case 'B':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( !m_pPostProcessEffectManager )
				return;

			if( !m_pPostProcessEffectManager->IsEnabled(CPostProcessEffect::TF_BLUR) )
				m_pPostProcessEffectManager->EnableBlur( true );
			else
				m_pPostProcessEffectManager->EnableBlur( false );

			// set the blur effect
		}
		break;

	case 'M':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( !m_pPostProcessEffectManager )
				return;

			if( !m_pPostProcessEffectManager->IsEnabled(CPostProcessEffect::TF_MONOCHROME_COLOR) )
				m_pPostProcessEffectManager->EnableEffect( CPostProcessEffect::TF_MONOCHROME_COLOR );
			else
				m_pPostProcessEffectManager->DisableEffect( CPostProcessEffect::TF_MONOCHROME_COLOR );

			// set the blur effect
		}
		break;

	case 'N':
/*		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}*/
		break;

	case 'V':
/*		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}*/
		break;

	case 'K':
/*		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}*/
		break;

	case 'L':
/*		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}*/
		break;

	case 'G':
/*		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}*/
		break;

	case 'C':
/*		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}*/
		break;

	case GIC_PAGE_UP:
	case GIC_PAGE_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( !m_pPostProcessEffectManager )
				return;

			bool page_up = (input.iGICode == GIC_PAGE_UP);

			float delta = 0.01f;
			if( m_pPostProcessEffectManager->IsEnabled(CPostProcessEffect::TF_HDR_LIGHTING) )
			{
				delta *= page_up ? 1.0f : -1.0f;
				m_HDRLightingParams.key_value += delta;
				m_pPostProcessEffectManager->SetHDRLightingParams( CHDRLightingParams::KEY_VALUE, m_HDRLightingParams );
			}
			else if( m_pPostProcessEffectManager->IsEnabled(CPostProcessEffect::TF_BLUR) )
			{
				delta = 0.05f;
				delta *= page_up ? 1.0f : -1.0f;
				m_fBlurStrength += delta;
				clamp( m_fBlurStrength, 0.05f, 10.0f );
				m_pPostProcessEffectManager->SetBlurStrength( m_fBlurStrength );
			}
		}
		break;

		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( !m_pPostProcessEffectManager )
				return;

			m_HDRLightingParams.key_value -= 0.01f;
			m_pPostProcessEffectManager->SetHDRLightingParams( CHDRLightingParams::KEY_VALUE, m_HDRLightingParams );
		}
		break;

/*	case VK_PRIOR:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_aPostProcessEffect[PP_BLOOM] )
			{
				float& luminance = m_pPPManager->GetPostProcessInstance()[m_aFilterIndex[SF_BRIGHTPASS]].m_avParam[0].x;
				luminance += 0.01f;
				if( 2.0f < luminance )
					luminance = 2.0f;
			}
		}
		break;

	case VK_NEXT:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_aPostProcessEffect[PP_BLOOM] )
			{
				float& luminance = m_pPPManager->GetPostProcessInstance()[m_aFilterIndex[SF_BRIGHTPASS]].m_avParam[0].x;
				luminance -= 0.01f;
				if( luminance < 0.01f )
					luminance = 0.01f;
			}
		}
		break;
*/
	default:
		break;
	}
}


CPostProcessEffectTest::CPostProcessEffectTest()
:
m_fBlurFactor(2.0f),
m_fLuminance(0.08f),
m_PPEffectFlags(0),
//m_fKeyValue(0.5f)
m_fBlurStrength(1.0f)
{
	m_HDRLightingParams.key_value = 3.5f;

	CParamLoader pl( "params.txt" );
	pl.LoadParam( "tone_mapping_key_value",    m_HDRLightingParams.key_value );
	pl.LoadParam( "luminance_adaptation_rate", m_HDRLightingParams.luminance_adaptation_rate );
//	pl.LoadBoolParam( "tone_mapping", m_HDRLightingParams.tone_mapping );
}


CPostProcessEffectTest::~CPostProcessEffectTest()
{
}


void CPostProcessEffectTest::RenderMeshes()
{
	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );

	for( int i=0; i<4; i++ )
	{
		hr = pd3dDevice->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		hr = pd3dDevice->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		hr = pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
		hr = pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );
	}

	// alpha-blending settings 
//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,  (DWORD)0x00000001 );
	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );	// draw a pixel if its alpha value is greater than or equal to '0x00000001'
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );

/*	CShaderManager *pShaderManager = m_Shader.GetShaderManager();
	if( pShaderManager )
	{
		pShaderManager->SetParam( "m_vEyePos", g_Camera.GetPosition() );
//		hr = pEffect->SetValue( "m_vEyePos", &(g_Camera.GetPosition()), sizeof(float) * 3 );

		CShaderTechniqueHandle tech_handle;
//		tech_handle.SetTechniqueName( "Default" );
//		tech_handle.SetTechniqueName( "NullShader" );
		tech_handle.SetTechniqueName( "QuickTest" );
		pShaderManager->SetTechnique( tech_handle );
//		hr = pEffect->SetTechnique( "QuickTest" );
	}*/

//	if( FAILED(hr) )
//		return;

	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetPixelShader( NULL );

	size_t i, num_meshes = m_vecMesh.size();

	if( 0 < num_meshes )
	{
		RenderAsSkybox( m_vecMesh[0], g_CameraController.GetPosition() );
	}

	// reset the world transform matrix
//	CD3DFixedFunctionPipelineManager ffp_mgr;
	FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );

	DWORD fog_colors[] =
	{
		0xFFFFFFFF, // place holder
		0xFFDDDFDE, // terrain mesh
		0xFF101010  // underground
	};

	for( i=1; i<num_meshes; i++ )
	{
		shared_ptr<CBasicMesh> pMesh = m_vecMesh[i].GetMesh();

		hr = pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, fog_colors[i] );

		if( pMesh )
			pMesh->Render();
	}
}


void CPostProcessEffectTest::UpdateShaderParams()
{
/*	float weight[7];

	float x2;
	float s2 = m_fBlurFactor * m_fBlurFactor;
	for( int i=0; i<7; i++ )
	{
		x2 = (float)(i*i);
		weight[i] = pow( 2.72f, - 0.5f * x2 / s2 ); 
//		weight[i] = 0.2f * pow( 2.72f, - 0.5f * x2 / s2 ); 
	}

	float sum = weight[0];
	for( int i=1; i<7; i++ )
		sum += weight[i] * 2.0f;

	for( int i=0; i<7; i++ )
		weight[i] /= sum;

	LPD3DXEFFECT pEffect[2];
	pEffect[0] = m_pPPManager->GetPostProcess(m_aPPEffectIndex[PP_COLOR_GBLUR_H]).GetEffect();
	pEffect[1] = m_pPPManager->GetPostProcess(m_aPPEffectIndex[PP_COLOR_GBLUR_V]).GetEffect();

	HRESULT hr;
	char str[32];
	for( int j=0; j<2; j++ )
	{
		if( !pEffect[j] )
			continue;

		pEffect[j]->SetFloat( "BlurWeights[6]", weight[0] );

		for( int i=1; i<6; i++ )
		{
			sprintf( str, "BlurWeights[%d]", 6-i );
			hr = pEffect[j]->SetFloat( str, weight[i] );

			sprintf( str, "BlurWeights[%d]", 6+i );
			hr = pEffect[j]->SetFloat( str, weight[i] );
		}
	}

//	float up_scale = m_fBlurFactor * 2.0f;
//	float down_scale = 1.0f / up_scale;

	float up_scale = ( m_fBlurFactor + 1.0f ) * 2.0f;
	float down_scale = 1.0f / up_scale;

//	m_pPPManager->GetPostProcess(m_aPPEffectIndex[PP_COLOR_DOWNFILTER4]).SetScale( down_scale, down_scale );
//	m_pPPManager->GetPostProcess(m_aPPEffectIndex[PP_COLOR_UPFILTER4]).SetScale( up_scale, up_scale );

	if( m_aPostProcessEffect[PP_GBLUR2] )
	{
		m_pPPManager->GetPostProcessInstance()[m_aFilterIndex[SF_DOWNFILTER_FOR_BLUR]].SetScale( down_scale, down_scale );
		m_pPPManager->GetPostProcessInstance()[m_aFilterIndex[SF_UPFILTER_FOR_BLUR]].SetScale( up_scale, up_scale );
	}*/
}


void CPostProcessEffectTest::RenderScene()
{
//	RenderSkybox();

	RenderMeshes();
}


void CPostProcessEffectTest::Update( float dt )
{
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
void CPostProcessEffectTest::Render()
{
//	DIRECT3D9.GetDevice()->BeginScene();
	UpdateShaderParams();
//	DIRECT3D9.GetDevice()->EndScene();

//	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
//	HRESULT hr;

	// update the camera matrix
	//	m_CameraController.SetCameraMatrix();
	//	D3DXMATRIX matCamera;
	//	m_CameraController.GetCameraMatrix( matCamera );
	//	m_pShaderManager->SetViewTransform( matCamera );

	if( m_pPostProcessEffectManager )
		m_pPostProcessEffectManager->BeginRender();

	RenderScene();

	if( m_pPostProcessEffectManager )
	{
		m_pPostProcessEffectManager->EndRender();
		m_pPostProcessEffectManager->RenderPostProcessEffects();
	}

//	D3DXMatrixIdentity( &matCamera );
//	m_CameraController.GetCameraMatrix( matCamera );
//	m_pShaderManager->SetViewTransform( matCamera );


	// render the text info
	string text = fmt_string( "gray mid value: %f", m_HDRLightingParams.key_value );
	m_pFont->DrawText( text.c_str(), Vector2(20,100), 0xFFFFFFFF );
}

// mesh 0: skybox
// mesh 1 to n: meshes
bool CPostProcessEffectTest::LoadModels()
{
	m_vecMeshFilepath.resize( 3 );
	m_vecMeshFilepath[0] = ""; // skybox
//	m_vecMeshFilepath[1] = "./models/compact_000-r01-low_poly.msh";
//	m_vecMeshFilepath[1] = "./models/underground_entrance.msh";
	m_vecMeshFilepath[1] = "./models/uge-terrain.msh";
	m_vecMeshFilepath[2] = "./models/uge-underground.msh";

	m_vecMesh.resize( 3 );
	CMeshResourceDesc skybox_mesh_desc;
	shared_ptr<CBoxMeshGenerator> pBoxMeshGenerator = shared_ptr<CBoxMeshGenerator>( new CBoxMeshGenerator );
	pBoxMeshGenerator->SetPolygonDirection( MeshPolygonDirection::INWARD );
	pBoxMeshGenerator->SetEdgeLengths( Vector3(1,1,1) );
	pBoxMeshGenerator->SetTexCoordStyleFlags( TexCoordStyle::LINEAR_SHIFT_INV_Y );
	pBoxMeshGenerator->SetPolygonDirection( MeshPolygonDirection::INWARD );
	pBoxMeshGenerator->SetTexturePath( "models/textures/skygrad_slim_01.jpg" );
	skybox_mesh_desc.pMeshGenerator = pBoxMeshGenerator;
	bool loaded = m_vecMesh[0].Load( skybox_mesh_desc );

	for( size_t i=1; i<m_vecMeshFilepath.size(); i++ )
		loaded = m_vecMesh[i].Load( m_vecMeshFilepath[i] );

	return true;
}


void SetDefaultLinearFog()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	float fStart   = 0.50f;    // For linear mode
	float fEnd     = 480.0f;//0.95f;
	float fDensity = 0.66f;   // For exponential modes

	DWORD fog_color = 0xFFDDDFDE;

	HRESULT hr;
	hr = pd3dDev->SetRenderState( D3DRS_FOGENABLE, TRUE );
	hr = pd3dDev->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
	hr = pd3dDev->SetRenderState( D3DRS_FOGCOLOR, fog_color);
	hr = pd3dDev->SetRenderState( D3DRS_FOGSTART, *(DWORD *)(&fStart));
	hr = pd3dDev->SetRenderState( D3DRS_FOGEND,   *(DWORD *)(&fEnd));

//	hr = pd3dDev->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_EXP);
//	hr = pd3dDev->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&fDensity));
}


int CPostProcessEffectTest::Init()
{
//	m_pShaderManager
//		= shared_ptr<CShaderManager>( new CShaderManager() );

	g_CameraController.SetPosition( Vector3(0,50,0) );

	bool loaded = m_Shader.Load( "shaders/mesh.fx" );

//	if( !m_pShaderManager->LoadShaderFromFile( "shaders/mesh.fx" ) )
	if( !loaded )
		return 1;

	CShaderManager *pShaderManager = m_Shader.GetShaderManager();

//	CShader::Get()->SetShaderManager( &m_pShaderManager );

	// set the world matrix to the identity
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matWorld );
	pShaderManager->SetWorldTransform( Matrix44Identity() );

	// set the projection matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 640.0f / 480.0f, 0.5f, 320.0f );
    DIRECT3D9.GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );
	pShaderManager->SetProjectionTransform( Matrix44PerspectiveFoV_LH( D3DX_PI / 4, 640.0f / 480.0f, 0.5f, 320.0f ) );
/*
	// initialize the light for the shader
	m_ShaderLightManager.Init();

	// create a light for the scene
	CHemisphericDirLight light;
	light.UpperColor.SetRGBA( 1.00f, 1.00f, 1.00f, 1.00f );
	light.LowerColor.SetRGBA( 0.25f, 0.25f, 0.25f, 1.00f );
	Vector3 vDir = Vector3(-1.0f,-3.0f,-1.5f);
	Vec3Normalize( vDir, vDir );
	light.vDirection = vDir;
	m_ShaderLightManager.SetLight( 0, light );
*/
	// create default texture
//	DWORD dwColor = 0xFFF0F0F0;
//	CTextureTool::CreateTexture( &dwColor, 1, 1, &m_pDefaultTexture );

	D3DSURFACE_DESC back_buffer_desc;
	IDirect3DSurface9 *pBackBuffer;
	DIRECT3D9.GetDevice()->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pBackBuffer->GetDesc( &back_buffer_desc );

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
//	HRESULT hr;

	SetDefaultLinearFog();

	// init post process effect manager
	bool test_ppeffect_mgr = true;
	if( test_ppeffect_mgr )
	{
		m_pPostProcessEffectManager = shared_ptr<CPostProcessEffectManager>( new CPostProcessEffectManager );

		Result::Name res = m_pPostProcessEffectManager->Init( "shaders" );
		if( res != Result::SUCCESS )
			return -1;
	}

	// load models
	loaded = LoadModels();

	m_pFont = CreateDefaultBuiltinFont();

	return 0;
}
