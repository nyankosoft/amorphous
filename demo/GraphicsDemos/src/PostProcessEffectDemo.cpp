#include "PostProcessEffectDemo.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "amorphous/Graphics/GraphicsResourceManager.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/CommonShaders.hpp"
#include "amorphous/Graphics/Font/FontBase.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/SkyboxMisc.hpp"
#include "amorphous/Graphics/FogParams.hpp"
#include "amorphous/Graphics/Direct3D/Direct3D9.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Input.hpp"

using std::string;
using namespace boost;


const float PostProcessEffectDemo::m_fBlurFactor = 0.02f;

void PostProcessEffectDemo::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case GIC_F6:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			PostProcessEffectFilter::ms_SaveFilterResultsAtThisFrame = 1;
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

	case 'P':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_EnablePostProcessEffects = !m_EnablePostProcessEffects;
		}
		break;

	case 'H':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( !m_pPostProcessEffectManager )
				return;

//			LoadHDRParamValues();

			if( !m_pPostProcessEffectManager->IsEnabled(PostProcessEffect::TF_HDR_LIGHTING) )
			{
				m_pPostProcessEffectManager->EnableHDRLighting( true );
				m_pPostProcessEffectManager->SetHDRLightingParams(
					HDRLightingParams::KEY_VALUE
				   |HDRLightingParams::TONE_MAPPING
				   |HDRLightingParams::LUMINANCE_ADAPTATION_RATE,
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

			if( !m_pPostProcessEffectManager->IsEnabled(PostProcessEffect::TF_BLUR) )
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

			if( !m_pPostProcessEffectManager->IsEnabled(PostProcessEffect::TF_MONOCHROME_COLOR) )
				m_pPostProcessEffectManager->EnableEffect( PostProcessEffect::TF_MONOCHROME_COLOR );
			else
				m_pPostProcessEffectManager->DisableEffect( PostProcessEffect::TF_MONOCHROME_COLOR );

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

	case 'J':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_BlurStrength == 0 )
				return;

			m_BlurStrength -= 1;

			if( m_pPostProcessEffectManager )
				m_pPostProcessEffectManager->SetBlurStrength( (float)m_BlurStrength * m_fBlurFactor );
		}
		break;

	case 'K':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_BlurStrength += 1;

			if( m_pPostProcessEffectManager )
				m_pPostProcessEffectManager->SetBlurStrength( (float)m_BlurStrength * m_fBlurFactor );
		}
		break;

	case 'U':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_fLuminanceAdaptationRate <= 0.01f )
				return;

			m_fLuminanceAdaptationRate -= 0.01f;
			m_HDRLightingParams.luminance_adaptation_rate = m_fLuminanceAdaptationRate;

			if( m_pPostProcessEffectManager )
				m_pPostProcessEffectManager->SetHDRLightingParams( HDRLightingParams::LUMINANCE_ADAPTATION_RATE, m_HDRLightingParams );
		}
		break;

	case 'I':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_fLuminanceAdaptationRate += 0.01f;
			m_HDRLightingParams.luminance_adaptation_rate = m_fLuminanceAdaptationRate;

			if( m_pPostProcessEffectManager )
				m_pPostProcessEffectManager->SetHDRLightingParams( HDRLightingParams::LUMINANCE_ADAPTATION_RATE, m_HDRLightingParams );
		}
		break;

	case GIC_PAGE_UP:
	case GIC_PAGE_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( !m_pPostProcessEffectManager )
				return;

			bool page_up = (input.iGICode == GIC_PAGE_UP);

			float delta = 0.01f;
			if( m_pPostProcessEffectManager->IsEnabled(PostProcessEffect::TF_HDR_LIGHTING) )
			{
				delta *= page_up ? 1.0f : -1.0f;
				m_HDRLightingParams.key_value += delta;
				m_pPostProcessEffectManager->SetHDRLightingParams( HDRLightingParams::KEY_VALUE, m_HDRLightingParams );
			}
//			else if( m_pPostProcessEffectManager->IsEnabled(PostProcessEffect::TF_BLUR) )
//			{
//				delta = 0.05f;
//				delta *= page_up ? 1.0f : -1.0f;
//				m_fBlurStrength += delta;
//				clamp( m_fBlurStrength, 0.05f, 10.0f );
//			}
		}
		break;

		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( !m_pPostProcessEffectManager )
				return;

			m_HDRLightingParams.key_value -= 0.01f;
			m_pPostProcessEffectManager->SetHDRLightingParams( HDRLightingParams::KEY_VALUE, m_HDRLightingParams );
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
		CGraphicsTestBase::HandleInput( input );
		break;
	}
}


PostProcessEffectDemo::PostProcessEffectDemo()
:
m_BlurStrength(50),
m_fLuminanceAdaptationRate(0.08f),
m_PPEffectFlags(0),
//m_fKeyValue(0.5f)
m_EnablePostProcessEffects(true)
//m_fBlurStrength(1.0f)
{
	m_HDRLightingParams.key_value = 3.5f;

	LoadHDRParamValues();

	SetBackgroundColor( SFloatRGBAColor::Magenta() );
}


PostProcessEffectDemo::~PostProcessEffectDemo()
{
}


void PostProcessEffectDemo::RenderMeshes()
{
	GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );

	for( int i=0; i<4; i++ )
	{
//		hr = pd3dDevice->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
//		hr = pd3dDevice->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
//		hr = pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
//		hr = pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );
		GraphicsDevice().SetSamplingParameter( i, SamplingParameter::MAG_FILTER, TextureFilter::LINEAR );
		GraphicsDevice().SetSamplingParameter( i, SamplingParameter::MIN_FILTER, TextureFilter::LINEAR );
		GraphicsDevice().SetSamplingParameter( i, SamplingParameter::TEXTURE_WRAP_AXIS_0, TextureAddressMode::REPEAT );
		GraphicsDevice().SetSamplingParameter( i, SamplingParameter::TEXTURE_WRAP_AXIS_1, TextureAddressMode::REPEAT );
	}

	// alpha-blending settings 
//	GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );
	GraphicsDevice().Disable( RenderStateType::ALPHA_BLEND );
//	GraphicsDevice().SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//	GraphicsDevice().SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	GraphicsDevice().Enable( RenderStateType::ALPHA_TEST );
//	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,  (DWORD)0x00000001 );
//	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );	// draw a pixel if its alpha value is greater than or equal to '0x00000001'
	GraphicsDevice().SetReferenceAlphaValue( 1.0f );
	GraphicsDevice().SetAlphaFunction( CompareFunc::GREATER_THAN_OR_EQUAL_TO );
//	pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );

/*	ShaderManager *pShaderManager = m_Shader.GetShaderManager();
	if( pShaderManager )
	{
		pShaderManager->SetParam( "m_vEyePos", GetCurrentCamera().GetPosition() );
//		hr = pEffect->SetValue( "m_vEyePos", &(GetCurrentCamera().GetPosition()), sizeof(float) * 3 );

		ShaderTechniqueHandle tech_handle;
//		tech_handle.SetTechniqueName( "Default" );
//		tech_handle.SetTechniqueName( "NullShader" );
		tech_handle.SetTechniqueName( "QuickTest" );
		pShaderManager->SetTechnique( tech_handle );
//		hr = pEffect->SetTechnique( "QuickTest" );
	}*/

//	if( FAILED(hr) )
//		return;

	size_t i, num_meshes = m_vecMesh.size();

	if( 0 < num_meshes )
	{
		RenderAsSkybox( m_vecMesh[0], GetCurrentCamera().GetPose() );
	}

//	DWORD fog_colors[] =
//	{
//		0xFFFFFFFF, // place holder
//		0xFFDDDFDE, // terrain mesh
//		0xFF101010  // underground
//	};

	SFloatRGBAColor fog_colors[] =
	{
		SFloatRGBAColor(1.0f,  1.0f,  1.0f,  1.0f), // place holder
		SFloatRGBAColor(0.867f,0.874f,0.871f,1.0f), // terrain mesh
		SFloatRGBAColor(0.063f,0.063f,0.063f,1.0f),  // underground
	};
	
	FogParams fog_params;
	fog_params.Density = 0.66f;   // For exponential modes
	fog_params.Start   = 0.50f;   // For linear mode
	fog_params.End     = 480.0f;  //0.95f;

	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( !pShaderMgr )
		return;

	pShaderMgr->SetWorldTransform( Matrix44Identity() );

	for( i=1; i<num_meshes; i++ )
	{
		shared_ptr<BasicMesh> pMesh = m_vecMesh[i].GetMesh();

//		hr = pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, fog_colors[i] );
		fog_params.Color = fog_colors[i];
		GraphicsDevice().SetFogParams( fog_params );

		if( pMesh )
		{
			for( int i=0; i<pMesh->GetNumMaterials(); i++ )
			{
				for( int j=0; j<(int)pMesh->GetMaterial(i).Texture.size(); j++ )
				{
					shared_ptr<GraphicsResourceEntry> pEntry = pMesh->GetMaterial(i).Texture[j].GetEntry();
					if( !pEntry )
						continue;

//					pEntry->GetTextureResource()->SetSamplingParameter( SamplingParameter::MAG_FILTER, TextureFilter::LINEAR );
//					pEntry->GetTextureResource()->SetSamplingParameter( SamplingParameter::MIN_FILTER, TextureFilter::LINEAR );
//					pEntry->GetTextureResource()->SetSamplingParameter( SamplingParameter::TEXTURE_WRAP_AXIS_0, TextureAddressMode::REPEAT );
//					pEntry->GetTextureResource()->SetSamplingParameter( SamplingParameter::TEXTURE_WRAP_AXIS_1, TextureAddressMode::REPEAT );
				}
			}

//			pMesh->Render();
			pMesh->Render( *pShaderMgr );
		}
	}
}


void PostProcessEffectDemo::UpdateShaderParams()
{
}


void PostProcessEffectDemo::RenderScene()
{
//	RenderSkybox();

	RenderMeshes();
}


void PostProcessEffectDemo::Update( float dt )
{
}


void PostProcessEffectDemo::Render()
{
//	DIRECT3D9.GetDevice()->BeginScene();
	UpdateShaderParams();
//	DIRECT3D9.GetDevice()->EndScene();

	if( m_pPostProcessEffectManager && m_EnablePostProcessEffects )
//	if( false )
	{
		m_pPostProcessEffectManager->BeginRender();

		RenderScene();

		m_pPostProcessEffectManager->EndRender();
		m_pPostProcessEffectManager->RenderPostProcessEffects();
	}
	else
	{
		RenderScene();
	}

	// render the text info
	string text = fmt_string( "gray mid value: %f\nblur strength: %f\nluminace adaptation rate: %f",
		m_HDRLightingParams.key_value,
		(float)m_BlurStrength * m_fBlurFactor,
		m_fLuminanceAdaptationRate
		);

	m_pFont->DrawText( text.c_str(), Vector2(20,100), 0xFFFFFFFF );
}

// mesh 0: skybox
// mesh 1 to n: meshes
bool PostProcessEffectDemo::LoadModels()
{
	m_vecMeshFilepath.resize( 3 );
	m_vecMeshFilepath[0] = ""; // skybox
//	m_vecMeshFilepath[1] = "PostProcessEffectDemo/models/compact_000-r01-low_poly.msh";
//	m_vecMeshFilepath[1] = "PostProcessEffectDemo/models/underground_entrance.msh";
	m_vecMeshFilepath[1] = "PostProcessEffectDemo/models/uge-terrain.msh";
	m_vecMeshFilepath[2] = "PostProcessEffectDemo/models/uge-underground.msh";

	m_vecMesh.resize( 3 );
	MeshResourceDesc skybox_mesh_desc;
	shared_ptr<BoxMeshGenerator> pBoxMeshGenerator( new BoxMeshGenerator );
	pBoxMeshGenerator->SetPolygonDirection( MeshPolygonDirection::INWARD );
	pBoxMeshGenerator->SetEdgeLengths( Vector3(1,1,1) );
	pBoxMeshGenerator->SetTexCoordStyleFlags( TexCoordStyle::LINEAR_SHIFT_INV_Y );
	pBoxMeshGenerator->SetPolygonDirection( MeshPolygonDirection::INWARD );
	pBoxMeshGenerator->SetTexturePath( "PostProcessEffectDemo/models/textures/skygrad_slim_01.jpg" );
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


int PostProcessEffectDemo::Init()
{
	CreateParamFileIfNotFound( "PostProcessEffectDemo/params.txt",
		"load_embedded_effect_shader    1\n"\
		"hdr.tone_mapping_key_value     3.0\n"\
		"hdr.luminance_adaptation_rate  0.02\n" );

	if( GetCameraController() )
		GetCameraController()->SetPosition( Vector3( 0.0f, 15.0f, -1.0f ) );

	m_Shader = GetNoLightingShader();

	ShaderManager *pShaderManager = m_Shader.GetShaderManager();
	if( !pShaderManager )
		return 1;

	// set the world matrix to the identity
	pShaderManager->SetWorldTransform( Matrix44Identity() );

/*
	// initialize the light for the shader
	m_ShaderLightManager.Init();

	// create a light for the scene
	HemisphericDirLight light;
	light.UpperColor.SetRGBA( 1.00f, 1.00f, 1.00f, 1.00f );
	light.LowerColor.SetRGBA( 0.25f, 0.25f, 0.25f, 1.00f );
	Vector3 vDir = Vector3(-1.0f,-3.0f,-1.5f);
	Vec3Normalize( vDir, vDir );
	light.vDirection = vDir;
	m_ShaderLightManager.SetLight( 0, light );
*/

//	D3DSURFACE_DESC back_buffer_desc;
//	IDirect3DSurface9 *pBackBuffer;
//	DIRECT3D9.GetDevice()->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
//	pBackBuffer->GetDesc( &back_buffer_desc );

//	SetDefaultLinearFog();
	GraphicsDevice().Enable( RenderStateType::FOG );

	// init post process effect manager
	bool test_ppeffect_mgr = true;
	if( test_ppeffect_mgr )
	{
		m_pPostProcessEffectManager.reset( new PostProcessEffectManager );

		int load_embedded_effect_shader = 1;
		LoadParamFromFile( "PostProcessEffectDemo/params.txt", "load_embedded_effect_shader", load_embedded_effect_shader );

		Result::Name res = Result::SUCCESS;
		if( load_embedded_effect_shader )
			res = m_pPostProcessEffectManager->Init();
		else
			res = m_pPostProcessEffectManager->Init( "shaders" );

		if( res != Result::SUCCESS )
			return -1;
	}

	// load models
	bool loaded = LoadModels();

	return 0;
}


void PostProcessEffectDemo::LoadHDRParamValues()
{
	ParamLoader pl( "PostProcessEffectDemo/params.txt" );
	pl.LoadParam( "hdr.tone_mapping_key_value",    m_HDRLightingParams.key_value );
	pl.LoadParam( "hdr.luminance_adaptation_rate", m_HDRLightingParams.luminance_adaptation_rate );
//	pl.LoadBoolParam( "hdr.tone_mapping", m_HDRLightingParams.tone_mapping );
}
