#include "MeshViewer.hpp"
#include "gds/3DMath/MatrixConversions.hpp"
#include "gds/Graphics/Mesh/BasicMesh.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "gds/Graphics/Shader/ShaderManager.hpp"
#include "gds/Graphics/MeshGenerators.hpp"
#include "gds/Graphics/Font/Font.hpp"
#include "gds/Graphics/GraphicsDevice.hpp"
#include "gds/Graphics/Direct3D9.hpp"
#include "gds/Graphics/SkyboxMisc.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/CameraController_Win32.hpp"
#include "gds/Support/FileOpenDialog_Win32.hpp"
#include <boost/filesystem.hpp>

using namespace std;
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
m_fInitCamShift( 20.0f )
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
			m_vecMeshFilepath.push_back( itr->string() );
		}
	}
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
/*
	CShaderManager *pShaderManager = m_Shader.GetShaderManager();
	LPD3DXEFFECT pEffect = pShaderManager->GetEffect();
	if( pEffect )
	{
		hr = pEffect->SetValue( "m_vEyePos", &(g_Camera.GetPosition()), sizeof(float) * 3 );

//		hr = pEffect->SetTechnique( "Default" );
//		hr = pEffect->SetTechnique( "NullShader" );
		hr = pEffect->SetTechnique( "QuickTest" );
	}
*/

//	if( FAILED(hr) )
//		return;

//	size_t i, num_meshes = m_vecMesh.size();
//	if( 0 < num_meshes )
//		RenderAsSkybox( m_vecMesh[0], g_CameraController.GetPosition() );

	shared_ptr<CBasicMesh> pMesh = m_Mesh.GetMesh();
	if(	!pMesh )
		return;

	// Calculate the scaling matrix.
	// Scale the mesh to fit its bounding sphere inside the view frustum.
	float mesh_radius = Vec3Length( pMesh->GetAABB().GetExtents() );
	float fov = (float)PI / 4.0f; // For Direct3D, see D3DXMatrixPerspectiveFovLH() call in CDirect3D9::InitD3D()
	float scale = m_fInitCamShift * tan( fov / 2.0f ) / mesh_radius;

	Matrix34 shift( Matrix34Identity() );
	shift.vPosition = -pMesh->GetAABB().GetCenterPosition();

	Matrix44 world
		= ToMatrix44( m_MeshWorldPose )
		* Matrix44Scaling( scale, scale, scale )
		* ToMatrix44( shift );

	// reset the world transform matrix
//	FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );
	FixedFunctionPipelineManager().SetWorldTransform( world );

	DWORD fog_colors[] =
	{
		0xFFFFFFFF, // place holder
		0xFFDDDFDE, // terrain mesh
		0xFF101010  // underground
	};
/*
	for( i=1; i<num_meshes; i++ )
	{
		shared_ptr<CBasicMesh> pMesh = m_vecMesh[i].GetMesh();

		hr = pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, fog_colors[i] );

		if( pMesh )
			pMesh->Render( ffp_mgr );
	}*/

	if( pMesh )
		pMesh->Render();
//		pMesh->Render( FixedFunctionPipelineManager() );
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

	m_MeshWorldPose.vPosition = Vector3(0,0,0);
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
//	DIRECT3D9.GetDevice()->BeginScene();
	UpdateShaderParams();
//	DIRECT3D9.GetDevice()->EndScene();

	RenderMeshes();

	// render the text info
//	string text = fmt_string( "gray mid value: %f", m_HDRLightingParams.key_value );
//	m_pFont->DrawText( text.c_str(), Vector2(20,100), 0xFFFFFFFF );
}

// mesh 0: skybox
// mesh 1 to n: meshes
bool CMeshViewer::LoadModel( const std::string& mesh_filepath )
{
	LOG_PRINT( " Opening a mesh file: " + mesh_filepath );

	bool loaded = m_Mesh.Load( mesh_filepath );

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


int CMeshViewer::Init()
{
	GetCameraController()->SetPosition( Vector3(0,0,-10) );

//	bool loaded = m_Shader.Load( "shaders/mesh.fx" );

//	if( !loaded )
//		return 1;

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
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matWorld );
	pShaderManager->SetWorldTransform( matWorld );

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
/*
	D3DSURFACE_DESC back_buffer_desc;
	IDirect3DSurface9 *pBackBuffer;
	DIRECT3D9.GetDevice()->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pBackBuffer->GetDesc( &back_buffer_desc );
*/
	SetDefaultLinearFog();

	m_pFont = shared_ptr<CFont>( new CFont( "Arial", 16, 32 ) );

	return 0;
}


void CMeshViewer::HandleInput( const SInputData& input )
{
	shared_ptr<CInputDeviceGroup> pDeviceGroup = InputDeviceHub().GetInputDeviceGroup(0);

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

	case 'M':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;

	case 'N':
		break;

	case 'V':
		break;

	case 'K':
/*		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}*/
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
