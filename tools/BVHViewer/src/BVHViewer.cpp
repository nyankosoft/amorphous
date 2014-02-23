#include "BVHViewer.hpp"
#include "amorphous/3DMath/MatrixConversions.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Graphics/TextureGenerators/SingleColorTextureGenerator.hpp"
#include "amorphous/Graphics/Font/TextureFont.hpp"
#include "amorphous/Graphics/Font/BuiltinFonts.hpp"
#include "amorphous/Graphics/GraphicsDevice.hpp"
#include "amorphous/Graphics/Direct3D/Direct3D9.hpp"
#include "amorphous/Graphics/SkyboxMisc.hpp"
#include "amorphous/Graphics/HemisphericLight.hpp"
#include "amorphous/Graphics/FogParams.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/GraphicsResourceManager.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/CameraController.hpp"
#include "amorphous/Support/FileOpenDialog_Win32.hpp"
#include "amorphous/Input.hpp"
#include <boost/filesystem.hpp>

using std::string;
using std::vector;
using namespace boost;
using namespace boost::filesystem;


ApplicationBase *amorphous::CreateApplicationInstance() { return new CBVHViewer; }



CBVHViewer::CBVHViewer()
:
m_CurrentFileIndex(-1),
m_CurrentMouseX(0),
m_CurrentMouseY(0),
m_LastRMouseClickX(0),
m_LastRMouseClickY(0),
m_fHeading(0),
m_fPitch(0),
m_MeshWorldPose( Matrix34Identity() ),
m_fInitCamShift( 15.0f ),
m_Lighting(true),
m_CurrentShaderIndex(0),
m_Playing(false),
m_fPlayTime(0)
{
	m_UseCameraController = true;

	Camera().SetPose( Matrix34( Vector3( 0.0f, 0.0f, -m_fInitCamShift ), Matrix33Identity() ) );

	SetBackgroundColor( SFloatRGBAColor( 0.15f, 0.15f, 0.15f, 1.00f ) );
}


CBVHViewer::~CBVHViewer()
{
}


void CBVHViewer::RefreshFileList( const std::string& directory_path )
{
	m_Filepaths.resize( 0 );

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
		else if ( itr->path().extension() == ".bvh" )
		{
			m_Filepaths.push_back( itr->path().string() );
		}
	}
}


void CBVHViewer::SetLights( ShaderManager& shader_mgr )
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


void CBVHViewer::LoadBlankTextures( BasicMesh& mesh )
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


void CBVHViewer::LoadShaders()
{
	vector<GenericShaderDesc> shader_descs;
	shader_descs.resize( 2 );

	GenericShaderDesc desc;

	shader_descs[0].LightingType = ShaderLightingType::PER_PIXEL;
	shader_descs[0].Specular = SpecularSource::NONE;

	shader_descs[1].LightingType = ShaderLightingType::PER_PIXEL;
	shader_descs[1].Specular = SpecularSource::DECAL_TEX_ALPHA;

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

	// added shader manager that uses fixed function pipeline as a fallback
	ShaderResourceDesc default_desc;
	default_desc.ShaderType = ShaderType::NON_PROGRAMMABLE;
	m_Shaders.push_back( ShaderHandle() );
	m_Shaders.back().Load( default_desc );
	m_Techniques.push_back( ShaderTechniqueHandle() );
	m_Techniques.back().SetTechniqueName( "Default" ); // actually not used
}


void CBVHViewer::RenderBVHSkeleton()
{
	float play_time = 0.001f;
	m_BVHPlayer.SetWorldTransformation( play_time );

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

	ShaderManager *pShaderManager = m_Shaders[ m_CurrentShaderIndex ].GetShaderManager();
	ShaderManager& shader_mgr = pShaderManager ? *pShaderManager : FixedFunctionPipelineManager();

	shader_mgr.SetParam( "g_vEyePos", GetCameraController()->GetPosition() );

	SetLights( shader_mgr );

	shader_mgr.SetTechnique( m_Techniques[m_CurrentShaderIndex] );

	GraphicsDevice().Disable( RenderStateType::LIGHTING );

	// Calculate the scaling matrix.
	// Scale the mesh to fit its bounding sphere inside the view frustum.
//	float mesh_radius = Vec3Length( pMesh->GetAABB().GetExtents() );
//	float fov = (float)PI / 4.0f; // For Direct3D, see D3DXMatrixPerspectiveFovLH() call in CDirect3D9::InitD3D()
//	float scale = m_fInitCamShift * tan( fov / 2.0f ) / mesh_radius;

//	Matrix34 shift( Matrix34Identity() );
//	shift.vPosition = -pMesh->GetAABB().GetCenterPosition();

//	Matrix44 world
//		= ToMatrix44( m_MeshWorldPose )
//		* Matrix44Scaling( scale, scale, scale )
//		* ToMatrix44( shift );
	Matrix44 world( Matrix44Identity() );

	// reset the world transform matrix
	FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );
	shader_mgr.SetWorldTransform( world );

	m_BVHPlayer.SetWorldTransformation( m_fPlayTime );
	m_BVHPlayer.Render();
}


void CBVHViewer::UpdateShaderParams()
{
}


void CBVHViewer::RenderScene()
{
	RenderBVHSkeleton();
}


void CBVHViewer::Update( float dt )
{
	// update world pose for the mesh
	int entered = 1;

//	float heading = (m_CurrentMouseX - m_LastRMouseClickX) * 0.01f;
//	float pitch   = (m_CurrentMouseY - m_LastRMouseClickY) * 0.01f;

	m_MeshWorldPose.vPosition = Vector3(0,0,0);
	m_MeshWorldPose.matOrient
		= Matrix33RotationX(m_fPitch)
		* Matrix33RotationY(m_fHeading);

	if( m_Playing )
	{
		m_fPlayTime += dt;
		clamp( m_fPlayTime, 0.0f, m_BVHPlayer.GetTotalPlayTime() );
	}
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
void CBVHViewer::Render()
{
	UpdateShaderParams();

	RenderBVHSkeleton();

	// render the text info
//	string text = fmt_string( "gray mid value: %f", m_HDRLightingParams.key_value );
//	m_pFont->DrawText( text.c_str(), Vector2(20,100), 0xFFFFFFFF );

	if( m_pFont )
	{
		string text( "File: " );
		if( 0 <= m_CurrentFileIndex && m_CurrentFileIndex < (int)m_Filepaths.size() )
			text += m_Filepaths[m_CurrentFileIndex];

		m_pFont->DrawText( text.c_str(), Vector2(20,120), 0xFFFFFFFF );
	}
}


bool CBVHViewer::LoadModel( const std::string& filepath )
{
	LOG_PRINT( " Opening a file: " + filepath );

	bool loaded = m_BVHPlayer.LoadBVHFile( filepath );

	m_BVHPlayer.Scale( 0.01f );

	const path current_filepath = filepath;

	m_CurrentFileIndex = -1;
	for( int i=0; i<(int)m_Filepaths.size(); i++ )
	{
		if( current_filepath == m_Filepaths[i] )
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
	float fEnd     = 150.0f;//0.95f;
	float fDensity = 0.66f; // For exponential modes

//	SFloatRGBAColor fog_color( 0.863f, 0.871f, 0.805f, 1.000f );
	SFloatRGBAColor fog_color( 0.15f, 0.15f, 0.15f, 1.00f );

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


int CBVHViewer::Init()
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

	shared_ptr<TextureFont> pFont( new TextureFont );
	bool res = pFont->InitFont( GetBuiltinFontData( "BitstreamVeraSansMono-Bold-256" ) );
	m_pFont = pFont;
	m_pFont->SetFontSize( 8, 16 );
//	m_pFont = shared_ptr<CFont>( new CFont( "Arial", 16, 32 ) );

	return 0;
}


void CBVHViewer::HandleInput( const InputData& input )
{
	shared_ptr<InputDeviceGroup> pDeviceGroup = GetInputDeviceHub().GetInputDeviceGroup(0);

	switch( input.iGICode )
	{
	case GIC_SPACE:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_Playing = !m_Playing;
		}
		break;

	case GIC_HOME:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_fPlayTime = 0;
		}
		break;

	case GIC_END:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_fPlayTime = m_BVHPlayer.GetTotalPlayTime();
		}
		break;

	case GIC_F6:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;

	case GIC_F9:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			GraphicsResourceManager().Refresh();
		}
		break;

	case GIC_RIGHT:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_CurrentFileIndex == -1 || m_Filepaths.empty() )
				break;

			m_CurrentFileIndex = (m_CurrentFileIndex + 1) % (int)m_Filepaths.size();

			LoadModel( m_Filepaths[m_CurrentFileIndex] );
			m_fPlayTime = 0.0f;
			m_Playing = false;
		}
		break;

	case GIC_LEFT:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_CurrentFileIndex == -1 || m_Filepaths.empty() )
				break;

			m_CurrentFileIndex = (m_CurrentFileIndex + (int)m_Filepaths.size() - 1) % (int)m_Filepaths.size();

			LoadModel( m_Filepaths[m_CurrentFileIndex] );
			m_fPlayTime = 0.0f;
			m_Playing = false;
		}
		break;

	case 'L':
		if( input.iType == ITYPE_KEY_PRESSED )
			m_Lighting = !m_Lighting;
		break;

	case 'M':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;

	case 'S':
		if( input.iType == ITYPE_KEY_PRESSED )
			m_CurrentShaderIndex = (m_CurrentShaderIndex + 1) % (int)m_Shaders.size();
		break;

	case GIC_MOUSE_BUTTON_R:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_CurrentMouseX = input.GetParamH16();
			m_CurrentMouseY = input.GetParamL16();
		}
		break;

	case GIC_MOUSE_AXIS_X:
	case GIC_MOUSE_AXIS_Y:
		if( pDeviceGroup->GetInputState( GIC_MOUSE_BUTTON_R ) == CInputState::PRESSED )
		{
			int new_x = input.GetParamH16();
			int new_y = input.GetParamL16();
			m_fHeading -= ( new_x - m_CurrentMouseX ) * 0.01f;;
			m_fPitch   -= ( new_y - m_CurrentMouseY ) * 0.01f;;
			m_CurrentMouseX = new_x;
			m_CurrentMouseY = new_y;
		}
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
