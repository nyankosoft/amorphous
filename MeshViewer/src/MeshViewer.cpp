#include "MeshViewer.hpp"
#include "gds/3DMath/MatrixConversions.hpp"
#include "gds/Graphics/Mesh/SkeletalMesh.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "gds/Graphics/Shader/ShaderManager.hpp"
#include "gds/Graphics/Shader/ShaderLightManager.hpp"
#include "gds/Graphics/MeshGenerators.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Graphics/GraphicsDevice.hpp"
#include "gds/Graphics/Direct3D/Direct3D9.hpp"
#include "gds/Graphics/SkyboxMisc.hpp"
#include "gds/Graphics/HemisphericLight.hpp"
#include "gds/Graphics/FogParams.hpp"
#include "gds/Graphics/Shader/GenericShaderGenerator.hpp"
#include "gds/Graphics/Shader/MiscEmbeddedShaderGenerator.hpp"
#include "gds/Graphics/2DPrimitive/2DRect.hpp"
#include "gds/Graphics/GraphicsResourceManager.hpp"
#include "gds/Input.hpp"
#include "gds/App/GameWindowManager.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/CameraController_Win32.hpp"
#include "gds/Support/FileOpenDialog_Win32.hpp"
#include <boost/filesystem.hpp>

using std::string;
using std::vector;
using namespace boost;
using namespace boost::filesystem;


CApplicationBase *CreateApplicationInstance() { return new CMeshViewer; }



CMeshViewer::CMeshViewer()
:
m_CurrentFileIndex(-1),
m_CurrentMouseX(0),
m_CurrentMouseY(0),
m_LastRMouseClickX(0),
m_LastRMouseClickY(0),
m_fHeading(0),
m_fPitch(0),
m_MeshWorldPose( Matrix34Identity() ),
m_fInitCamShift( 20.0f ),
m_Lighting(true),
m_CurrentShaderIndex(0),
m_ScalingFactor(1),
m_UseSingleDiffuseColorShader(false),
m_NormalMapTextureIndex(-1),
m_CurrentSDCShaderIndex(0),
m_RenderSubsetsInformation(false),
m_RenderZSorted(false)
{
	m_UseCameraController = false;

	Camera().SetPose( Matrix34( Vector3( 0.0f, 0.0f, -m_fInitCamShift ), Matrix33Identity() ) );
}


CMeshViewer::~CMeshViewer()
{
}


void CMeshViewer::RefreshFileList( const std::string& directory_path )
{
	m_vecMeshFilepath.resize( 0 );

	directory_iterator end_itr; // default construction yields past-the-end
	for ( directory_iterator itr( directory_path );
		itr != end_itr;
		++itr )
	{
		if ( is_directory(itr->status()) )
		{
			continue;
		}
//		else if ( IsMeshFile( itr ) )
		else if ( itr->path().extension() == ".msh" )
		{
			m_vecMeshFilepath.push_back( itr->path().string() );
		}
	}
}


void CMeshViewer::SetLights( CShaderManager& shader_mgr )
{
	// change lighting render state
	// - Needed when fixed function pipeline is used
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING, m_Lighting );

	shared_ptr<CShaderLightManager> pLightMgr
		= shader_mgr.GetShaderLightManager();

	if( !pLightMgr )
		return;

	pLightMgr->ClearLights();

	if( !m_Lighting )
	{
		pLightMgr->CommitChanges();
		return;
	}

//	CDirectionalLight dir_light;
	CHemisphericDirectionalLight dir_light;
	dir_light.vDirection = Vec3GetNormalized( Vector3( 1,-3,2 ) );
	dir_light.fIntensity = 1.0f;
//	dir_light.DiffuseColor = SFloatRGBColor::White();
	dir_light.Attribute.UpperDiffuseColor = SFloatRGBAColor::White();
	dir_light.Attribute.LowerDiffuseColor = SFloatRGBAColor( 0.1f, 0.1f, 0.1f, 1.0f );


//	pLightMgr->SetDirectionalLight( dir_light );
	pLightMgr->SetHemisphericDirectionalLight( dir_light );

	pLightMgr->CommitChanges();
}


void CMeshViewer::LoadBlankTextures( CBasicMesh& mesh )
{
	int num_subsets = mesh.GetNumMaterials();
	for( int i=0; i<num_subsets; i++ )
	{
		CMeshMaterial& mat = mesh.Material(i);
		if( mat.Texture.empty() )
		{
			mat.Texture.resize( 1 );
			mat.TextureDesc.resize( 1 );
			mat.TextureDesc[0].Width  = 1;
			mat.TextureDesc[0].Height = 1;
			mat.TextureDesc[0].Format = TextureFormat::A8R8G8B8;
			mat.TextureDesc[0].pLoader.reset( new CSingleColorTextureFilling(SFloatRGBAColor::White()) );
			mat.Texture[0].Load( mat.TextureDesc[0] );
		}
	}
}


void CMeshViewer::RenderSubsetsInformation( CBasicMesh& mesh )
{
	const int screen_width  = CGraphicsComponent::GetScreenWidth();
	const int screen_height = CGraphicsComponent::GetScreenWidth();
	SRect screen_rect = RectLTWH( 0, 0, screen_width, screen_height );
	screen_rect.Inflate( -16, -16 );

	C2DRect bg_rect( screen_rect );
//	bg_rect.SetPositionLTWH( 20, 20, 640, 400 );
	bg_rect.SetColor( SFloatRGBAColor( 0.0f, 0.1f, 0.1f, 0.6f ) );
	bg_rect.Draw();
	
	if( m_pFont )
		m_pFont->SetFontSize( 6, 12 );

	const int num_subsets = mesh.GetNumMaterials();
	for( int i=0; i<num_subsets; i++ )
	{
		if( m_pFont )
			m_pFont->DrawText( fmt_string( "[%02d]", i ), 24, 24 );

//		const CMeshSubset& subset = mesh.GetSubset(i);
//		subset.GetNumVertices();
//		subset.GetNumTriangles();

		const CMeshMaterial& mat = mesh.GetMaterial(i);
		const int num_textures = (int)mat.Texture.size();
		for( int j=0; j<num_textures; j++ )
		{
			int rect_edge_len = 64;
			C2DRect rect;
			int left = 64;
			int top  = 24 + (rect_edge_len + 8) * j;
			rect.SetPositionLTRB(
				left,  // left
				top, // top
				left  + rect_edge_len - 1, // right
				top   + rect_edge_len - 1  // bottom
				);

			rect.Draw( mat.Texture[j] );

			const CTextureResourceDesc& desc = mat.TextureDesc[j];
			if( !m_pFont )
				continue;

			string text = fmt_string( "%dx%d / %s", desc.Width, desc.Height, desc.ResourcePath.c_str() );
			m_pFont->DrawText( text, left + rect_edge_len + 8, top );
		}

	}

}


void CMeshViewer::LoadShaders()
{
	// Clear all the shaders
	m_Shaders.resize( 0 );

	vector<CGenericShaderDesc> shader_descs;
	shader_descs.resize( 2 );

	CGenericShaderDesc desc;

	shader_descs[0].ShaderLightingType = CShaderLightingType::PER_PIXEL;
	shader_descs[0].Specular = CSpecularSource::NONE;

	shader_descs[0].NormalMapTextureIndex = m_NormalMapTextureIndex;

	shader_descs[1].ShaderLightingType = CShaderLightingType::PER_PIXEL;
	shader_descs[1].Specular = CSpecularSource::DECAL_TEX_ALPHA;

	shader_descs[1].NormalMapTextureIndex = m_NormalMapTextureIndex;

	CShaderResourceDesc shader_desc;

//	m_Shaders.resize( shader_descs.size() );
//	m_Techniques.resize( shader_descs.size() );
	for( size_t i=0; i<shader_descs.size(); i++ )
	{
		shader_desc.pShaderGenerator.reset( new CGenericShaderGenerator(shader_descs[i]) );
		CShaderHandle shader;
		bool loaded = shader.Load( shader_desc );

		if( loaded )
		{
			m_Shaders.push_back( shader );
			m_Techniques.push_back( CShaderTechniqueHandle() );
			m_Techniques.back().SetTechniqueName( "Default" );
		}
	}

	// miscellaneous shaders
	CEmbeddedMiscShader::ID shader_ids[] =
	{
		CEmbeddedMiscShader::SINGLE_DIFFUSE_COLOR,
		CEmbeddedMiscShader::SHADED_SINGLE_DIFFUSE_COLOR,
		CEmbeddedMiscShader::VERTEX_WEIGHT_MAP_DISPLAY
	};

	for( size_t i=0; i<numof(shader_ids); i++ )
	{
		shader_desc.pShaderGenerator.reset( new CMiscEmbeddedShaderGenerator(shader_ids[i]) );
		CShaderHandle shader;
		bool loaded = shader.Load( shader_desc );

		if( loaded )
		{
			m_SingleDiffuseColorShaders.push_back( shader );
			m_SingleDiffuseColorShaderTechniques.push_back( CShaderTechniqueHandle() );
			m_SingleDiffuseColorShaderTechniques.back().SetTechniqueName( "Default" );
		}
	}

	// added shader manager that uses fixed function pipeline as a fallback
	CShaderResourceDesc default_desc;
	default_desc.ShaderType = CShaderType::NON_PROGRAMMABLE;
	m_Shaders.push_back( CShaderHandle() );
	m_Shaders.back().Load( default_desc );
	m_Techniques.push_back( CShaderTechniqueHandle() );
	m_Techniques.back().SetTechniqueName( "Default" ); // actually not used

	CShaderResourceDesc vegetation_shader_desc;
	vegetation_shader_desc.ResourcePath = "shaders/vegetation.fx";
	m_Shaders.push_back( CShaderHandle() );
	m_Shaders.back().Load( vegetation_shader_desc );
	m_Techniques.push_back( CShaderTechniqueHandle() );
	m_Techniques.back().SetTechniqueName( "Default" ); // actually not used
}


void CMeshViewer::RenderMeshes()
{
	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	for( int i=0; i<4; i++ )
	{
		hr = pd3dDevice->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		hr = pd3dDevice->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		hr = pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
		hr = pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );
	}

	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );

	// alpha-blending settings 
//	GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );
	GraphicsDevice().Disable( RenderStateType::ALPHA_BLEND );
//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,  (DWORD)0x00000001 );
	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );	// draw a pixel if its alpha value is greater than or equal to '0x00000001'
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );

//	CShaderManager *pShaderManager = m_Shaders[ m_CurrentShaderIndex ].GetShaderManager();
	CShaderManager *pShaderManager
		= m_UseSingleDiffuseColorShader ?
		m_SingleDiffuseColorShaders[ m_CurrentSDCShaderIndex ].GetShaderManager() :
		m_Shaders[ m_CurrentShaderIndex ].GetShaderManager();

	CShaderManager& shader_mgr = pShaderManager ? *pShaderManager : FixedFunctionPipelineManager();

	shader_mgr.SetParam( "g_vEyePos", GetCameraController()->GetPosition() );

	if( !m_UseSingleDiffuseColorShader )
		SetLights( shader_mgr );

	if( m_UseSingleDiffuseColorShader )
		shader_mgr.SetTechnique( m_SingleDiffuseColorShaderTechniques[m_CurrentSDCShaderIndex] );
	else
		shader_mgr.SetTechnique( m_Techniques[m_CurrentShaderIndex] );

	shared_ptr<CBasicMesh> pMesh = m_Mesh.GetMesh();
	if(	!pMesh )
		return;

	// Calculate the scaling matrix.
	// Scale the mesh to fit its bounding sphere inside the view frustum.
	float mesh_radius = Vec3Length( pMesh->GetAABB().GetExtents() );
	float fov = (float)PI / 4.0f; // For Direct3D, see D3DXMatrixPerspectiveFovLH() call in CDirect3D9::InitD3D()
	float magnification = (float)m_ScalingFactor * 1.0f;
	float scale = m_fInitCamShift * tan( fov / 2.0f ) / mesh_radius * magnification;

	Matrix34 shift( Matrix34Identity() );
	shift.vPosition = -pMesh->GetAABB().GetCenterPosition();

	Matrix44 world
		= ToMatrix44( m_MeshWorldPose )
		* Matrix44Scaling( scale, scale, scale )
		* ToMatrix44( shift );

	// reset the world transform matrix
	FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );
	shader_mgr.SetWorldTransform( world );

//	if( pMesh )
//		pMesh->Render( shader_mgr );

	if( m_UseSingleDiffuseColorShader )
	{
		SFloatRGBAColor colors[] =
		{
			SFloatRGBAColor( 1.0f, 0.6f, 0.6f, 1.0f ),
			SFloatRGBAColor( 0.6f, 1.0f, 0.6f, 1.0f ),
			SFloatRGBAColor( 0.6f, 0.6f, 1.0f, 1.0f ),
			SFloatRGBAColor( 0.6f, 1.0f, 1.0f, 1.0f ),
			SFloatRGBAColor( 1.0f, 0.6f, 1.0f, 1.0f ),
			SFloatRGBAColor( 1.0f, 1.0f, 0.6f, 1.0f ),
		};
		for( int i=0; i<pMesh->GetNumMaterials(); i++ )
		{
			int color_index = i % numof(colors);
			shader_mgr.SetParam( "DiffuseColor", colors[color_index] );
			pMesh->RenderSubset( shader_mgr, i );
		}
	}
	else
	{
		CShaderManager& mesh_shader
			= ( m_CurrentShaderIndex == (int)m_Shaders.size() - 1 ) ? FixedFunctionPipelineManager() : shader_mgr;
		
		if( m_RenderZSorted )
		{
			GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );
			pMesh->RenderZSorted( mesh_shader );
		}
		else
			pMesh->Render( mesh_shader );
	}
}


void CMeshViewer::UpdateShaderParams()
{
}


void CMeshViewer::RenderScene()
{
	RenderMeshes();
}


void CMeshViewer::Update( float dt )
{
	// update world pose for the mesh
	int entered = 1;

//	float heading = (m_CurrentMouseX - m_LastRMouseClickX) * 0.01f;
//	float pitch   = (m_CurrentMouseY - m_LastRMouseClickY) * 0.01f;

//	m_MeshWorldPose.vPosition = Vector3(0,0,0);
	m_MeshWorldPose.matOrient
		= Matrix33RotationX(m_fPitch)
		* Matrix33RotationY(m_fHeading);
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
void CMeshViewer::Render()
{
	UpdateShaderParams();

	RenderMeshes();

	int screen_w = GameWindowManager().GetScreenWidth();
	int screen_h = GameWindowManager().GetScreenHeight();
	C2DRect rect( 0, screen_h - 25, screen_w - 1, screen_h - 1 );
	rect.SetColor( SFloatRGBAColor(0,0,0,0.6f) );
	rect.Draw();

	shared_ptr<CBasicMesh> pMesh = m_Mesh.GetMesh();
	if( pMesh )
	{
		int num_subsets   = pMesh->GetNumMaterials();
		unsigned int num_vertices  = pMesh->GetNumVertices();
		unsigned int num_triangles = pMesh->GetNumTriangles();
		const Vector3 whd = pMesh->GetAABB().vMax - pMesh->GetAABB().vMin; // width, height and depth
		m_pFont->SetFontSize( 8, 16 );
		string text = fmt_string( "%u vertices | %u triangles | %d subsets | %.03f x %.03f x %.03f[m]",
			num_vertices, num_triangles, num_subsets,
			whd.x, whd.y, whd.z );

		// Display the number of bones if it's a skeletal mesh
		shared_ptr<CSkeletalMesh> pSkeletalMesh
			= boost::dynamic_pointer_cast<CSkeletalMesh,CBasicMesh>(pMesh);
		if( pSkeletalMesh )
			text += fmt_string( " | %d bones", pSkeletalMesh->GetNumBones() );

		m_pFont->DrawText( text, Vector2( 20, screen_h - 20 ) );
	}

	// render the text info
//	string text = fmt_string( "gray mid value: %f", m_HDRLightingParams.key_value );
//	m_pFont->DrawText( text.c_str(), Vector2(20,100), 0xFFFFFFFF );

	if( m_RenderSubsetsInformation
	 && pMesh )
	{
		RenderSubsetsInformation( *pMesh );
	}
}


bool CMeshViewer::LoadModel( const std::string& mesh_filepath )
{
	LOG_PRINT( " Opening a mesh file: " + mesh_filepath );

//	bool loaded = m_Mesh.Load( mesh_filepath );
	CMeshResourceDesc desc;
	desc.ResourcePath = mesh_filepath;
	desc.LoadOptionFlags |= MeshLoadOption::CUSTOM_MESH;
//	desc.MeshType = CMeshType::SKELETAL;
	bool loaded = m_Mesh.Load( desc );

	if( loaded && m_Mesh.GetMesh() )
	{
		LoadBlankTextures( *m_Mesh.GetMesh() );
	}

	const path current_mesh_filepath = mesh_filepath;

	m_CurrentFileIndex = -1;
	for( int i=0; i<(int)m_vecMeshFilepath.size(); i++ )
	{
		if( current_mesh_filepath == m_vecMeshFilepath[i] )
		{
			m_CurrentFileIndex = i;
			break;
		}
	}

	return true;
}


void SetDefaultLinearFog()
{
	float fStart   = 0.50f; // For linear mode
	float fEnd     = 480.0f;//0.95f;
	float fDensity = 0.66f; // For exponential modes

	SFloatRGBAColor fog_color( 0.863f, 0.871f, 0.805f, 1.000f );

	CFogParams params;
	params.Start = fStart;
	params.End   = fEnd;
	params.Mode  = FogMode::LINEAR;
	params.Color = fog_color;

//	params.Density = fDensity;
//	params.Mode = FogMode::EXP;

	GraphicsDevice().SetFogParams( params );
	GraphicsDevice().Enable( RenderStateType::FOG );
}


int CMeshViewer::Init()
{
	GetCameraController()->SetPosition( Vector3(0,0,-10) );

	LoadShaders();

	string input_filepath;// = g_CmdLine;

	if( 0 < ms_CommandLineArguments.size() )
		input_filepath = ms_CommandLineArguments[0];

	string mesh_filepath;
	if( 0 < input_filepath.length() )
		mesh_filepath = input_filepath;
	else
		GetFilename( mesh_filepath );

	if( mesh_filepath.length() == 0 )
		return -1;
	else
	{
		const path parent_directory = path(mesh_filepath).parent_path();
		RefreshFileList( parent_directory.string() );

		// load models
		bool loaded = LoadModel( mesh_filepath );
	}
/*
	CShaderManager *pShaderManager = m_Shader.GetShaderManager();

	// set the world matrix to the identity
	pShaderManager->SetWorldTransform( Matrix44Identity() );

	// set the projection matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 640.0f / 480.0f, 0.5f, 320.0f );
    DIRECT3D9.GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );
	pShaderManager->SetProjectionTransform( matProj );
*/
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

	SetDefaultLinearFog();

	m_pFont = CreateDefaultBuiltinFont();
	m_pFont->SetFontSize( 16, 32 );

	return 0;
}


void CMeshViewer::HandleInput( const SInputData& input )
{
	shared_ptr<CInputDeviceGroup> pDeviceGroup = InputDeviceHub().GetInputDeviceGroup(0);
	int new_x = 0, new_y = 0;

	switch( input.iGICode )
	{
	case GIC_F6:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
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

	case GIC_RIGHT:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_CurrentFileIndex == -1 || m_vecMeshFilepath.empty() )
				break;

			m_CurrentFileIndex = (m_CurrentFileIndex + 1) % (int)m_vecMeshFilepath.size();

			LoadModel( m_vecMeshFilepath[m_CurrentFileIndex] );
		}
		break;

	case GIC_LEFT:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_CurrentFileIndex == -1 || m_vecMeshFilepath.empty() )
				break;

			m_CurrentFileIndex = (m_CurrentFileIndex + (int)m_vecMeshFilepath.size() - 1) % (int)m_vecMeshFilepath.size();

			LoadModel( m_vecMeshFilepath[m_CurrentFileIndex] );
		}
		break;

	case 'L':
		if( input.iType == ITYPE_KEY_PRESSED )
			m_Lighting = !m_Lighting;
		break;

	case 'D':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( !m_UseSingleDiffuseColorShader )
				m_UseSingleDiffuseColorShader = true;
			else
				m_CurrentSDCShaderIndex = (m_CurrentSDCShaderIndex + 1) % (int)m_SingleDiffuseColorShaders.size();
		}
		break;

	case 'N':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_NormalMapTextureIndex += 1;
			if( 4 <= m_NormalMapTextureIndex )
				m_NormalMapTextureIndex = -1;

			// Reload the shaders with the new settings.
			LoadShaders();
		}
		break;

	case 'S':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_UseSingleDiffuseColorShader )
				m_UseSingleDiffuseColorShader = false;
			else
				m_CurrentShaderIndex = (m_CurrentShaderIndex + 1) % (int)m_Shaders.size();
		}
		break;

	case 'T':
		if( input.iType == ITYPE_KEY_PRESSED )
			m_RenderSubsetsInformation = !m_RenderSubsetsInformation;
		break;

	case 'A':
		if( input.iType == ITYPE_KEY_PRESSED )
			m_RenderZSorted = !m_RenderZSorted;
		break;

	case 'B':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			static int color_index = 0;
			SFloatRGBAColor bg_colors[] =
			{
				SFloatRGBAColor::Blue(),
				SFloatRGBAColor(0,0,0,1),
				SFloatRGBAColor(0.2f,0.2f,0.2f,1.0f)
			};

			color_index = (color_index+1) % numof(bg_colors);

			SetBackgroundColor( bg_colors[color_index] );
		}
		break;

	case GIC_MOUSE_BUTTON_L:
	case GIC_MOUSE_BUTTON_R:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_CurrentMouseX = input.GetParamH16();
			m_CurrentMouseY = input.GetParamL16();
		}
		break;

	case GIC_MOUSE_AXIS_X:
	case GIC_MOUSE_AXIS_Y:
		new_x = input.GetParamH16();
		new_y = input.GetParamL16();
		if( pDeviceGroup->GetInputState( GIC_MOUSE_BUTTON_R ) == CInputState::PRESSED )
		{
			m_fHeading -= ( new_x - m_CurrentMouseX ) * 0.01f;
			m_fPitch   -= ( new_y - m_CurrentMouseY ) * 0.01f;
		}
		if( pDeviceGroup->GetInputState( GIC_MOUSE_BUTTON_L ) == CInputState::PRESSED )
		{
			m_MeshWorldPose.vPosition.x += ( new_x - m_CurrentMouseX ) * 0.01f;
			m_MeshWorldPose.vPosition.y -= ( new_y - m_CurrentMouseY ) * 0.01f;
		}
		m_CurrentMouseX = new_x;
		m_CurrentMouseY = new_y;
		break;

	case GIC_MOUSE_WHEEL_UP:
		m_ScalingFactor += 1;
		clamp( m_ScalingFactor, 1, 64 );
		break;

	case GIC_MOUSE_WHEEL_DOWN:
		m_ScalingFactor -= 1;
		clamp( m_ScalingFactor, 1, 64 );
		break;

	case GIC_PAGE_UP:
	case GIC_PAGE_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;

	default:
		break;
	}
}
