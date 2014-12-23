#include "MeshViewer.hpp"
#include "amorphous/3DMath/MatrixConversions.hpp"
#include "amorphous/Graphics/Mesh/SkeletalMesh.hpp"
#include "amorphous/Graphics/Mesh/MeshInfoRenderer.hpp"
#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Graphics/TextureGenerators/SingleColorTextureGenerator.hpp"
#include "amorphous/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "amorphous/Graphics/Font/BuiltinFonts.hpp"
#include "amorphous/Graphics/GraphicsDevice.hpp"
#include "amorphous/Graphics/SkyboxMisc.hpp"
#include "amorphous/Graphics/HemisphericLight.hpp"
#include "amorphous/Graphics/FogParams.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/Shader/MiscShaderGenerator.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/GraphicsResourceManager.hpp"
#include "amorphous/Input.hpp"
#include "amorphous/App/GameWindowManager.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/CameraController.hpp"
#include "amorphous/Support/FileOpenDialog_Win32.hpp"
#include "amorphous/Support/recursive_file_finder.hpp"
#include <boost/filesystem.hpp>

using std::string;
using std::vector;
using namespace boost;
using namespace boost::filesystem;


ApplicationBase *amorphous::CreateApplicationInstance() { return new MeshViewer; }


static const char *sg_help[] = {
	"[Keyboard]",
	"  F: Show/hide vertex normals",
	"  S: Cycle shaders",
	"  B: Cycle background colors",
	"  T: Display subset information",
	"  A: Z-sort polygons to render",
	"  L: Turn on/off lighting",
	"  N: Enable/disable bump mapping",
	"  F9: Reload graphics resources",
	"  Right: Load the next mesh file in the current directory",
	"  Left: Load the previous mesh file in the current directory",
	"",
	"[Mouse]",
	"  Drag with left button: Move (x/y)",
	"  Drag with right button: Rotate (heading/pitch)",
	"  Wheel: Zoom in/out",
	""
};


MeshViewer::MeshViewer()
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
m_ScalingFactor(10),
m_UseSingleDiffuseColorShader(false),
m_NormalMapTextureIndex(-1),
m_CurrentSDCShaderIndex(0),
m_RenderSubsetsInformation(false),
m_RenderZSorted(false),
m_NormalLengthFactor(0),
m_DisplayHelp(false)
{
	m_UseCameraController = false;

	Camera().SetPose( Matrix34( Vector3( 0.0f, 0.0f, -m_fInitCamShift ), Matrix33Identity() ) );
}


MeshViewer::~MeshViewer()
{
}


void MeshViewer::RefreshFileList( const std::string& directory_path )
{
	m_MeshFilepaths.resize( 0 );

	recursive_file_finder rff;
	rff.m_include_extensions.push_back( ".msh" );
	rff.process( directory_path );

	const size_t num_mesh_files = rff.m_found_file_pathnames.size();
	m_MeshFilepaths.resize( num_mesh_files );
	for( size_t i=0; i<num_mesh_files; i++ )
		m_MeshFilepaths[i] = rff.m_found_file_pathnames[i].string();

	LOG_PRINT( to_string((int)num_mesh_files) + " .msh files were found." );

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
			m_MeshFilepaths.push_back( itr->path().string() );
		}
	}
}


void MeshViewer::SetLights( ShaderManager& shader_mgr )
{
	// change lighting render state
	// - Needed when fixed function pipeline is used
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING, m_Lighting );

	shared_ptr<ShaderLightManager> pLightMgr
		= shader_mgr.GetShaderLightManager();

	if( !pLightMgr )
		return;

	pLightMgr->ClearLights();

	if( !m_Lighting )
	{
		pLightMgr->CommitChanges();
		return;
	}

//	DirectionalLight dir_light;
	HemisphericDirectionalLight dir_light;
	dir_light.vDirection = Vec3GetNormalized( Vector3( 1,-3,2 ) );
	dir_light.fIntensity = 1.0f;
//	dir_light.DiffuseColor = SFloatRGBColor::White();
	dir_light.Attribute.UpperDiffuseColor = SFloatRGBAColor::White();
	dir_light.Attribute.LowerDiffuseColor = SFloatRGBAColor( 0.1f, 0.1f, 0.1f, 1.0f );


//	pLightMgr->SetDirectionalLight( dir_light );
	pLightMgr->SetHemisphericDirectionalLight( dir_light );

	pLightMgr->CommitChanges();
}


void MeshViewer::LoadBlankTextures( BasicMesh& mesh )
{
	int num_subsets = mesh.GetNumMaterials();
	for( int i=0; i<num_subsets; i++ )
	{
		MeshMaterial& mat = mesh.Material(i);
		if( mat.Texture.empty() )
		{
			mat.Texture.resize( 1 );
			mat.TextureDesc.resize( 1 );
			mat.TextureDesc[0].Width  = 1;
			mat.TextureDesc[0].Height = 1;
			mat.TextureDesc[0].Format = TextureFormat::A8R8G8B8;
			mat.TextureDesc[0].pLoader.reset( new SingleColorTextureGenerator(SFloatRGBAColor::White()) );
			mat.Texture[0].Load( mat.TextureDesc[0] );
		}
	}
}


void MeshViewer::RenderSubsetsInformation( BasicMesh& mesh )
{
	const int screen_width  = GraphicsComponent::GetScreenWidth();
	const int screen_height = GraphicsComponent::GetScreenWidth();
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

		const MeshMaterial& mat = mesh.GetMaterial(i);
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

			const TextureResourceDesc& desc = mat.TextureDesc[j];
			if( !m_pFont )
				continue;

			string text = fmt_string( "%dx%d / %s", desc.Width, desc.Height, desc.ResourcePath.c_str() );
			m_pFont->DrawText( text, left + rect_edge_len + 8, top );
		}

	}

}


void MeshViewer::LoadShaders()
{
	// Clear all the shaders
	m_Shaders.resize( 0 );

	vector<GenericShaderDesc> shader_descs;
	shader_descs.resize( 2 );

	GenericShaderDesc desc;

	shader_descs[0].LightingType = ShaderLightingType::PER_PIXEL;
	shader_descs[0].Specular = SpecularSource::NONE;

	shader_descs[0].NormalMapTextureIndex = m_NormalMapTextureIndex;

	shader_descs[1].LightingType = ShaderLightingType::PER_PIXEL;
	shader_descs[1].Specular = SpecularSource::DECAL_TEX_ALPHA;

	shader_descs[1].NormalMapTextureIndex = m_NormalMapTextureIndex;

	ShaderResourceDesc shader_desc;

//	m_Shaders.resize( shader_descs.size() );
//	m_Techniques.resize( shader_descs.size() );
	for( size_t i=0; i<shader_descs.size(); i++ )
	{
		shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(shader_descs[i]) );
		ShaderHandle shader;
		bool loaded = shader.Load( shader_desc );

		if( loaded )
		{
			m_Shaders.push_back( shader );
			m_Techniques.push_back( ShaderTechniqueHandle() );
			m_Techniques.back().SetTechniqueName( "Default" );
		}
	}

	// miscellaneous shaders
	MiscShader::ID shader_ids[] =
	{
		MiscShader::SINGLE_DIFFUSE_COLOR,
		MiscShader::SHADED_SINGLE_DIFFUSE_COLOR,
		MiscShader::VERTEX_WEIGHT_MAP_DISPLAY
	};

	for( size_t i=0; i<numof(shader_ids); i++ )
	{
		shader_desc.pShaderGenerator.reset( new MiscShaderGenerator(shader_ids[i]) );
		ShaderHandle shader;
		bool loaded = shader.Load( shader_desc );

		if( loaded )
		{
			m_SingleDiffuseColorShaders.push_back( shader );
			m_SingleDiffuseColorShaderTechniques.push_back( ShaderTechniqueHandle() );
			m_SingleDiffuseColorShaderTechniques.back().SetTechniqueName( "Default" );
		}
	}

	// added shader manager that uses fixed function pipeline as a fallback
	ShaderResourceDesc default_desc;
	default_desc.ShaderType = ShaderType::NON_PROGRAMMABLE;
	m_Shaders.push_back( ShaderHandle() );
	m_Shaders.back().Load( default_desc );
	m_Techniques.push_back( ShaderTechniqueHandle() );
	m_Techniques.back().SetTechniqueName( "Default" ); // actually not used

	ShaderResourceDesc vegetation_shader_desc;
	vegetation_shader_desc.ResourcePath = "shaders/vegetation.fx";
	m_Shaders.push_back( ShaderHandle() );
	m_Shaders.back().Load( vegetation_shader_desc );
	m_Techniques.push_back( ShaderTechniqueHandle() );
	m_Techniques.back().SetTechniqueName( "Default" ); // actually not used
}


void MeshViewer::RenderMeshes()
{
	Result::Name res = Result::SUCCESS;

	for( int i=0; i<4; i++ )
	{
		res = GraphicsDevice().SetSamplingParameter( i, SamplingParameter::MAG_FILTER, TextureFilter::LINEAR );
		res = GraphicsDevice().SetSamplingParameter( i, SamplingParameter::MIN_FILTER, TextureFilter::LINEAR );
		res = GraphicsDevice().SetSamplingParameter( i, SamplingParameter::TEXTURE_WRAP_AXIS_0, TextureAddressMode::MIRRORED_REPEAT );
		res = GraphicsDevice().SetSamplingParameter( i, SamplingParameter::TEXTURE_WRAP_AXIS_1, TextureAddressMode::MIRRORED_REPEAT );
	}

	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );

	// alpha-blending settings 
//	GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );
	GraphicsDevice().Disable( RenderStateType::ALPHA_BLEND );
//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
//	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,  (DWORD)0x00000001 );
//	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );	// draw a pixel if its alpha value is greater than or equal to '0x00000001'
	GraphicsDevice().Enable( RenderStateType::FACE_CULLING );
	GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );

//	ShaderManager *pShaderManager = m_Shaders[ m_CurrentShaderIndex ].GetShaderManager();
	ShaderManager *pShaderManager
		= m_UseSingleDiffuseColorShader ?
		m_SingleDiffuseColorShaders[ m_CurrentSDCShaderIndex ].GetShaderManager() :
		m_Shaders[ m_CurrentShaderIndex ].GetShaderManager();

	ShaderManager& shader_mgr = pShaderManager ? *pShaderManager : FixedFunctionPipelineManager();

	shader_mgr.SetParam( "g_vEyePos", GetCameraController()->GetPosition() );

	if( !m_UseSingleDiffuseColorShader )
		SetLights( shader_mgr );

	if( m_UseSingleDiffuseColorShader )
		shader_mgr.SetTechnique( m_SingleDiffuseColorShaderTechniques[m_CurrentSDCShaderIndex] );
	else
		shader_mgr.SetTechnique( m_Techniques[m_CurrentShaderIndex] );

	shared_ptr<BasicMesh> pMesh = m_Mesh.GetMesh();
	if(	!pMesh )
		return;

	// Calculate the scaling matrix.
	// Scale the mesh to fit its bounding sphere inside the view frustum.
	float mesh_radius = Vec3Length( pMesh->GetAABB().GetExtents() );
	float fov = (float)PI / 4.0f; // For Direct3D, see D3DXMatrixPerspectiveFovLH() call in CDirect3D9::InitD3D()
	float magnification = (float)m_ScalingFactor * 0.1f;
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
		ShaderManager& mesh_shader
			= ( m_CurrentShaderIndex == (int)m_Shaders.size() - 1 ) ? FixedFunctionPipelineManager() : shader_mgr;
		
		if( m_RenderZSorted )
		{
			GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );
			pMesh->RenderZSorted( mesh_shader );
		}
		else
			pMesh->Render( mesh_shader );
	}

	if( 0 < m_NormalLengthFactor )
	{
		MeshInfoRenderer renderer;
		float normal_length = pow( 2.0f, m_NormalLengthFactor ) * 0.001f;
		renderer.RenderNormals( *pMesh, world, normal_length );
	}
}


void MeshViewer::UpdateShaderParams()
{
}


void MeshViewer::RenderScene()
{
	RenderMeshes();
}


void MeshViewer::Update( float dt )
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
void MeshViewer::Render()
{
	UpdateShaderParams();

	RenderMeshes();

	int screen_w = GetGameWindowManager().GetScreenWidth();
	int screen_h = GetGameWindowManager().GetScreenHeight();
	C2DRect rect( 0, screen_h - 25, screen_w - 1, screen_h - 1 );
	rect.SetColor( SFloatRGBAColor(0,0,0,0.6f) );
	rect.Draw();

	shared_ptr<BasicMesh> pMesh = m_Mesh.GetMesh();
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
		shared_ptr<SkeletalMesh> pSkeletalMesh
			= boost::dynamic_pointer_cast<SkeletalMesh,BasicMesh>(pMesh);
		if( pSkeletalMesh )
			text += fmt_string( " | %d bones", pSkeletalMesh->GetNumBones() );

		m_pFont->DrawText( text, Vector2( 20, screen_h - 20 ) );
	}

	const char *help_instruction = "Press 'H' to display help.";
	m_pFont->DrawText( help_instruction, Vector2( screen_w - m_pFont->GetTextWidth(help_instruction) - 10, screen_h - 20 ) );

	// render the text info
//	string text = fmt_string( "gray mid value: %f", m_HDRLightingParams.key_value );
//	m_pFont->DrawText( text.c_str(), Vector2(20,100), 0xFFFFFFFF );

	if( m_RenderSubsetsInformation
	 && pMesh )
	{
		RenderSubsetsInformation( *pMesh );
	}

	if( m_DisplayHelp )
	{
		int num_help_rows = numof(sg_help);
		C2DRect help_bg_rect;
		help_bg_rect.SetPositionLTWH( 40, 40, 500, num_help_rows * m_pFont->GetFontHeight() );
		help_bg_rect.SetColor( SFloatRGBAColor(0.0f,0.0f,0.0f,0.5f) );
		help_bg_rect.Draw();

		for( int i=0; i<num_help_rows; i++ )
		{
			Vector2 pos;
			pos.x = 50;
			pos.y = 50 + i * m_pFont->GetFontHeight();
			m_pFont->DrawText( sg_help[i], pos );
		}
	}
}


bool MeshViewer::LoadModel( const std::string& mesh_filepath )
{
	LOG_PRINT( " Opening a mesh file: " + mesh_filepath );

//	bool loaded = m_Mesh.Load( mesh_filepath );
	MeshResourceDesc desc;
	desc.ResourcePath = mesh_filepath;
	desc.LoadOptionFlags |= MeshLoadOption::CUSTOM_MESH;
//	desc.MeshType = MeshType::SKELETAL;
	bool loaded = m_Mesh.Load( desc );

	if( loaded && m_Mesh.GetMesh() )
	{
		LoadBlankTextures( *m_Mesh.GetMesh() );
	}

	const path current_mesh_filepath = mesh_filepath;

	m_CurrentFileIndex = -1;
	for( int i=0; i<(int)m_MeshFilepaths.size(); i++ )
	{
		if( current_mesh_filepath == m_MeshFilepaths[i] )
		{
			m_CurrentFileIndex = i;
			break;
		}
	}

	GetGameWindowManager().SetWindowTitleText( desc.ResourcePath );

	return true;
}


void SetDefaultLinearFog()
{
	float fStart   = 0.50f; // For linear mode
	float fEnd     = 480.0f;//0.95f;
	float fDensity = 0.66f; // For exponential modes

	SFloatRGBAColor fog_color( 0.863f, 0.871f, 0.805f, 1.000f );

	FogParams params;
	params.Start = fStart;
	params.End   = fEnd;
	params.Mode  = FogMode::LINEAR;
	params.Color = fog_color;

//	params.Density = fDensity;
//	params.Mode = FogMode::EXP;

	GraphicsDevice().SetFogParams( params );
	GraphicsDevice().Enable( RenderStateType::FOG );
}


int MeshViewer::Init()
{
	GetCameraController()->SetPosition( Vector3(0,0,-10) );

	LoadShaders();

	string input_pathname;// = g_CmdLine;
	bool mesh_loaded = false;

	if( 0 < ms_CommandLineArguments.size() )
		input_pathname = ms_CommandLineArguments[0];

	if( input_pathname.length() == 0 )
		GetFilename( input_pathname );

	if( input_pathname.length() == 0 )
		return -1;

	if( is_directory(input_pathname) )
	{
		// A directory
		RefreshFileList( input_pathname );

		// Load the first file if any were found in RefreshFileList().
		if( 0 < m_MeshFilepaths.size() )
			mesh_loaded = LoadModel( m_MeshFilepaths.front() );
	}
	else
	{
		const string mesh_filepath = input_pathname;

		// A file pathname
		const path parent_directory = path(mesh_filepath).parent_path();
		RefreshFileList( parent_directory.string() );

		// load models
		mesh_loaded = LoadModel( mesh_filepath );
	}
	

	SetDefaultLinearFog();

	m_pFont = CreateDefaultBuiltinFont();
	m_pFont->SetFontSize( 16, 32 );

	return 0;
}


void MeshViewer::HandleInput( const InputData& input )
{
	shared_ptr<InputDeviceGroup> pDeviceGroup = GetInputDeviceHub().GetInputDeviceGroup(0);
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
			if( m_CurrentFileIndex == -1 || m_MeshFilepaths.empty() )
				break;

			m_CurrentFileIndex = (m_CurrentFileIndex + 1) % (int)m_MeshFilepaths.size();

			LoadModel( m_MeshFilepaths[m_CurrentFileIndex] );
		}
		break;

	case GIC_LEFT:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_CurrentFileIndex == -1 || m_MeshFilepaths.empty() )
				break;

			m_CurrentFileIndex = (m_CurrentFileIndex + (int)m_MeshFilepaths.size() - 1) % (int)m_MeshFilepaths.size();

			LoadModel( m_MeshFilepaths[m_CurrentFileIndex] );
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

	case 'F':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_NormalLengthFactor = (m_NormalLengthFactor + 1) % 8;
		}
		break;

	case 'H':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_DisplayHelp = !m_DisplayHelp;
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

	case 'R':
	case GIC_MOUSE_BUTTON_M:
		if( input.iType == ITYPE_KEY_PRESSED )
			m_ScalingFactor = 10; // Reset the scaling
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
		GraphicsApplicationBase::HandleInput( input );
		break;
	}
}
