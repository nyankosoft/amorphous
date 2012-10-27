#include "ResolutionChangeTest.hpp"
#include "gds/Graphics/Camera.hpp"
#include "gds/Graphics/Mesh/BasicMesh.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Graphics/2DPrimitive/2DRect.hpp"
#include "gds/Graphics/MeshGenerators.hpp"
#include "gds/Graphics/MeshUtilities.hpp"
#include "gds/Graphics/HemisphericLight.hpp"
#include "gds/Graphics/Shader/GenericShaderGenerator.hpp"
#include "gds/Graphics/Shader/ShaderManager.hpp"
#include "gds/Graphics/Shader/ShaderLightManager.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
//#include "gds/Graphics/SkyboxMisc.hpp"
#include "gds/App/GameWindowManager.hpp"
#include "gds/Support/CameraController.hpp"

using std::string;
using std::vector;
using namespace boost;


extern CCamera g_Camera;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new CResolutionChangeTest();
}


extern const std::string GetAppTitle()
{
	return string("Resolution Change Test");
}



CResolutionChangeTest::CResolutionChangeTest()
:
m_CurrentlyEnteredDigits(-1)
{
}


CResolutionChangeTest::~CResolutionChangeTest()
{
}


int CResolutionChangeTest::Init()
{
	vector<CAdapterMode>  adapter_modes;
	GraphicsDevice().GetAdapterModesForDefaultAdapter( adapter_modes );

	for( size_t i=0; i<adapter_modes.size(); i++ )
	{
		if( adapter_modes[i].Format == TextureFormat::X8R8G8B8 )
		{
			const int num_display_modes = (int)adapter_modes[i].vecDisplayMode.size();
			for( int j=0; j<num_display_modes; j++ )
			{
				const CDisplayMode& display_mode = adapter_modes[i].vecDisplayMode[j];
				float h = (float) display_mode.Height;
				float w = (float) display_mode.Width;
				if( 0.001 < fabs( h / w - 9.0f / 16.0f)
				 && 0.001 < fabs( h / w - 3.0f /  4.0f) )
					continue; // Neither 16:9 nor 4:3.

				m_AvailableDisplayModes.push_back( display_mode );
			}
		}
	}


	m_pFont = CreateDefaultBuiltinFont();

//	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
//	m_DefaultTechnique.SetTechniqueName( "NullShader" );

	// initialize shader
//	bool shader_loaded = m_Shader.Load( "./shaders/ResolutionChangeTest.fx" );
	CShaderResourceDesc shader_desc;
	CGenericShaderDesc gs_desc;

	gs_desc.ShaderLightingType = CShaderLightingType::PER_PIXEL;
	gs_desc.LightingTechnique  = CLightingTechnique::HEMISPHERIC;
	gs_desc.Specular           = CSpecularSource::UNIFORM;
	shader_desc.pShaderGenerator.reset( new CGenericShaderGenerator(gs_desc) );
	m_Shader.Load( shader_desc );

	// load skybox mesh
//	m_SkyboxMesh = CreateSkyboxMesh( "./textures/skygrad_slim_01.jpg" );

	// load the terrain mesh
//	CMeshResourceDesc mesh_desc;
//	mesh_desc.ResourcePath = "./models/terrain06.msh";
//	mesh_desc.MeshType     = CMeshType::BASIC;
//	m_TerrainMesh.Load( mesh_desc );

//	m_TestTexture.Load( "./textures/flare02.dds" );
	m_TestTextures.resize( 2 );
	m_TestTextures[0].Load( "./textures/Bio-Energy.png" );
	m_TestTextures[1].Load( "./textures/Bio-Energy.jpg" );

	// Create meshes

	m_Meshes.resize( NUM_MESHES );

	CMeshResourceDesc sphere_mesh_desc;
	CSphereDesc sphere_desc;
	sphere_desc.radii[0] = sphere_desc.radii[1] = sphere_desc.radii[2] = 0.5f;
	sphere_desc.num_segments = 32;
	sphere_desc.num_sides    = 64;
	sphere_mesh_desc.pMeshGenerator.reset( new CSphereMeshGenerator(sphere_desc) );

//	bool sphere_mesh_loaded = m_SphereMesh.Load( sphere_mesh_desc );
	bool sphere_mesh_loaded = m_Meshes[MESH_SPHERE].Load( sphere_mesh_desc );

	m_Meshes[MESH_BOX] = CreateBoxMesh( Vector3(1,1,1) );

	m_Meshes[MESH_TEXTURED_BOX] = CreateBoxMesh( Vector3(1,1,1), SFloatRGBAColor::White(), Matrix34Identity(), "./textures/test_texture.png" );

	return 0;
}


void CResolutionChangeTest::Update( float dt )
{
}


void CResolutionChangeTest::Render2DRects()
{
	const int num_rects = 14;
	C2DRect rect[num_rects];
	SFloatRGBAColor colors[5] =
	{ 
		SFloatRGBAColor::Black(),
		SFloatRGBAColor::White(),
		SFloatRGBAColor::Red(),
		SFloatRGBAColor::Green(),
		SFloatRGBAColor::Blue()
	};

	// non-textured rects
	rect[ 0] = C2DRect( Vector2( 200,   50 ), Vector2( 250-11, 100-11 ), colors[0] );
	rect[ 1] = C2DRect( Vector2( 250,   50 ), Vector2( 300-11, 100-11 ), colors[1] );
	rect[ 2] = C2DRect( Vector2( 300,   50 ), Vector2( 350-11, 100-11 ), colors[2] );
	rect[ 3] = C2DRect( Vector2( 350,   50 ), Vector2( 400-11, 100-11 ), colors[3] );
	rect[ 4] = C2DRect( Vector2( 400,   50 ), Vector2( 450-11, 100-11 ), colors[4] );
	for( int i=0; i<5; i++ )
		colors[i].fAlpha = 0.5f;
	rect[ 5] = C2DRect( Vector2( 200,  100 ), Vector2( 250-11, 150-11 ), colors[0] );
	rect[ 6] = C2DRect( Vector2( 250,  100 ), Vector2( 300-11, 150-11 ), colors[1] );
	rect[ 7] = C2DRect( Vector2( 300,  100 ), Vector2( 350-11, 150-11 ), colors[2] );
	rect[ 8] = C2DRect( Vector2( 350,  100 ), Vector2( 400-11, 150-11 ), colors[3] );
	rect[ 9] = C2DRect( Vector2( 400,  100 ), Vector2( 450-11, 150-11 ), colors[4] );

	// textured rects
	rect[10] = C2DRect( Vector2( 200, 150 ), Vector2( 250-11, 200-11 ), SFloatRGBAColor::White() );
	rect[11] = C2DRect( Vector2( 250, 150 ), Vector2( 300-11, 200-11 ), SFloatRGBAColor::White() );
	rect[12] = C2DRect( Vector2( 200, 200 ), Vector2( 250-11, 250-11 ), SFloatRGBAColor(1.0f, 1.0f, 1.0f, 0.5f) );
	rect[13] = C2DRect( Vector2( 250, 200 ), Vector2( 300-11, 250-11 ), SFloatRGBAColor(1.0f, 1.0f, 1.0f, 0.5f) );
	for( int i=10; i<14; i++ )
		rect[i].SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );

	for( int i=0; i<num_rects-2; i++ )
		rect[i].Draw();

	if( 2 <= m_TestTextures.size() )
	{
		rect[10].Draw( m_TestTextures[0] );
		rect[11].Draw( m_TestTextures[1] );
		rect[12].Draw( m_TestTextures[0] );
		rect[13].Draw( m_TestTextures[1] );
	}
}


void CResolutionChangeTest::Render()
{
	Matrix44 matWorld = Matrix44Identity();
	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
//	if( !pShaderMgr )
//		return;

	CShaderManager& shader_mgr = pShaderMgr ? (*pShaderMgr) : FixedFunctionPipelineManager();

//	RenderAsSkybox( m_SkyboxMesh, g_CameraController.GetPosition() );

	GraphicsDevice().Disable(  RenderStateType::FACE_CULLING );

	shader_mgr.SetWorldTransform( matWorld );

	SetLight();

	// Need to set the light again when the graphics device is released and re-created
//	if( pShaderMgr )
//	{
//		shared_ptr<CShaderLightManager> pShaderLightMgr = pShaderMgr->GetShaderLightManager();
//		if( pShaderLightMgr )
//			pShaderLightMgr->CommitChanges();
//	}

	Vector3 positions[] =
	{
		Vector3(-1.2f, 1.2f,0.0f),
		Vector3( 1.2f, 1.2f,0.0f),
		Vector3(-1.2f,-1.2f,0.0f),
		Vector3( 1.2f,-1.2f,0.0f)
	};

	for( size_t i=0; i<m_Meshes.size(); i++ )
	{
		shared_ptr<CBasicMesh> pMesh = m_Meshes[i].GetMesh();
		if( !pMesh )
			continue;

		shader_mgr.SetWorldTransform( Matrix34( positions[i], Matrix33Identity() ) );

		pMesh->Render( shader_mgr );
	}


//	shared_ptr<CBasicMesh> pSphereMesh = m_SphereMesh.GetMesh();
//	if( pSphereMesh )
//		pSphereMesh->Render( shader_mgr );

	GraphicsDevice().Disable( RenderStateType::LIGHTING );

	// Render 2D rectangles
	Render2DRects();

	int h = 16;
//	int y = 
	m_pFont->SetFontSize( h/2, h );
	m_pFont->SetFontColor( SFloatRGBAColor::White() );
	for( int i=0; i<(int)m_AvailableDisplayModes.size(); i++ )
	{
		const CDisplayMode& mode = m_AvailableDisplayModes[i];
		string text = fmt_string( "%d: %dx%d (%u Hz)", i, mode.Width, mode.Height, mode.RefreshRate );
		SFloatRGBAColor text_color = mode.Current ? SFloatRGBAColor::Aqua() : SFloatRGBAColor::White();
		m_pFont->SetFontColor( text_color );
		m_pFont->DrawText( text, 20, 50 + i * h );
	}

	int y = 50 + (int)m_AvailableDisplayModes.size() * h;
	string digits = "> ";
	digits += (0 <= m_CurrentlyEnteredDigits) ? to_string(m_CurrentlyEnteredDigits) : "-";
	m_pFont->DrawText( digits, 20, y );
}


void CResolutionChangeTest::HandleInput( const SInputData& input )
{
	if( input.iType == ITYPE_KEY_PRESSED
	 && '0' <= input.iGICode
	 && input.iGICode <= '9' )
	{
		if( m_CurrentlyEnteredDigits == -1 )
			m_CurrentlyEnteredDigits = 0;
		else
			m_CurrentlyEnteredDigits *= 10;

		m_CurrentlyEnteredDigits += (input.iGICode - '0' );
	}

	switch( input.iGICode )
	{
	case GIC_ENTER:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( 0 <= m_CurrentlyEnteredDigits
			 && m_CurrentlyEnteredDigits < (int)m_AvailableDisplayModes.size() )
			{
				const CDisplayMode& display_mode = m_AvailableDisplayModes[m_CurrentlyEnteredDigits];

				GameWindowManager().ChangeScreenSize( display_mode.Width, display_mode.Height, false );

				g_Camera.SetAspectRatio( (float)display_mode.Width / (float)display_mode.Height );
			}
			m_CurrentlyEnteredDigits = -1;
		}
		break;
//	case GIC_UP:
//		if( input.iType == ITYPE_KEY_PRESSED )
//			???
//		break;
//	case GIC_DOWN:
//		if( input.iType == ITYPE_KEY_PRESSED )
//			???
//		break;
	default:
		break;
	}
}


void CResolutionChangeTest::SetLight()
{
	CHemisphericDirectionalLight hs_dir_light;
	hs_dir_light.Attribute.UpperDiffuseColor = SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f);
	hs_dir_light.Attribute.LowerDiffuseColor = SFloatRGBAColor(0.1f,0.1f,0.1f,1.0f);
	hs_dir_light.vDirection = Vec3GetNormalized( Vector3(1.0f,-3.0f,1.5f) );

	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( pShaderMgr )
	{
		shared_ptr<CShaderLightManager> pShaderLightMgr = pShaderMgr->GetShaderLightManager();
		if( pShaderLightMgr )
		{
			pShaderLightMgr->ClearLights();
			pShaderLightMgr->SetHemisphericDirectionalLight( hs_dir_light );
			pShaderLightMgr->CommitChanges();
		}
	}
}
