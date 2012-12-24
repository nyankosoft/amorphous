#include "MeshSplitterTest.hpp"
#include "gds/3DMath/Matrix34.hpp"
#include "gds/Graphics.hpp"
//#include "gds/Graphics/Camera.hpp"
//#include "gds/Graphics/HemisphericLight.hpp"
//#include "gds/Graphics/2DPrimitive/2DRect.hpp"
//#include "gds/Graphics/Mesh/BasicMesh.hpp"
#include "gds/Graphics/Shader/ShaderManager.hpp"
#include "gds/Graphics/Shader/GenericShaderGenerator.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "gds/Graphics/Shader/ShaderLightManager.hpp"
//#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Graphics/MeshUtilities.hpp"
#include "gds/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "gds/Graphics/Mesh/ConvexMeshSplitter.hpp"
#include "gds/Graphics/PrimitiveShapeRenderer.hpp"
#include "gds/Input/InputHub.hpp"
#include "gds/Support/CameraController.hpp"
#include "gds/Support/Profile.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/Macro.h"

using std::string;
using namespace boost;


CMeshSplitterTest::CMeshSplitterTest()
{
	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	m_ControlSplitPlane = false;

	g_Camera.SetPosition( Vector3( 0, 1, -120 ) );
//	g_Camera.SetPosition( Vector3( 0, 520, 120 ) );
}


CMeshSplitterTest::~CMeshSplitterTest()
{
}


bool CMeshSplitterTest::InitShader()
{
	m_MeshTechnique.SetTechniqueName( "Default" );

	// initialize shader
//	bool shader_loaded = m_Shader.Load( "./shaders/MeshSplitterTest.fx" );
	CGenericShaderDesc gs_desc;
	gs_desc.Specular = CSpecularSource::NONE;
	CShaderResourceDesc shader_desc;
	shader_desc.pShaderGenerator.reset( new CGenericShaderGenerator( gs_desc ) );
	bool shader_loaded = m_Shader.Load( shader_desc );
	
	return shader_loaded;
}


bool CMeshSplitterTest::SetLight()
{
	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( !pShaderMgr )
		return false;

	CShaderLightManager *pShaderLightMgr = m_Shader.GetShaderManager()->GetShaderLightManager().get();
	if( pShaderLightMgr )
	{
		pShaderLightMgr->ClearLights();

		CHemisphericDirectionalLight light;
		light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
		light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
		light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );

//		pShaderLightMgr->SetLight( 0, light );
//		pShaderLightMgr->SetDirectionalLightOffset( 0 );
//		pShaderLightMgr->SetNumDirectionalLights( 1 );
		pShaderLightMgr->SetHemisphericDirectionalLight( light );

		pShaderLightMgr->CommitChanges();

		return true;
	}
	else
		return false;
}


void CMeshSplitterTest::Reset()
{
	m_RootMeshNode.release_children();

//	m_RootMeshNode.object.Release();

	string mesh_pathname;
	LoadParamFromFile( "params.txt", "mesh", mesh_pathname );

	const char *builtin_mesh_prefix = "built-in_mesh::";
	if( mesh_pathname.find( builtin_mesh_prefix ) == 0 )
	{
		string builtin_mesh_name = mesh_pathname.substr( strlen(builtin_mesh_prefix) );
		if( builtin_mesh_name == "box" )
		{
			CBoxMeshGenerator box_generator;
			box_generator.Generate();
			m_RootMeshNode.object.pMesh.reset( new CCustomMesh );
			m_RootMeshNode.object.pMesh->LoadFromArchive( box_generator.MeshArchive() );
		}
	}
	else
	{
		m_RootMeshNode.object.pMesh.reset( new CCustomMesh );
		bool res = m_RootMeshNode.object.pMesh->LoadFromFile( mesh_pathname );
	}
}


int CMeshSplitterTest::Init()
{
	m_pFont = CreateDefaultBuiltinFont();
	m_pFont->SetFontSize( 6, 12 );

	InitShader();

	m_pSplitPlaneController.reset( new CCameraController( 3 ) );
	UpdateSplitPlaneControllerState();

	m_pSplitPlaneController->AssignKeyForCameraControl( GIC_INVALID, CameraControl::Forward );
	m_pSplitPlaneController->AssignKeyForCameraControl( GIC_INVALID, CameraControl::Backward );
	m_pSplitPlaneController->AssignKeyForCameraControl( GIC_INVALID, CameraControl::Left );
	m_pSplitPlaneController->AssignKeyForCameraControl( GIC_INVALID, CameraControl::Right );
	m_pSplitPlaneController->AssignKeyForCameraControl( GIC_INVALID, CameraControl::Up );
	m_pSplitPlaneController->AssignKeyForCameraControl( GIC_INVALID, CameraControl::Down );

	// Load a mesh
	Reset();

	return 0;
}


void CMeshSplitterTest::Update( float dt )
{
	if( m_pSplitPlaneController )
		m_pSplitPlaneController->UpdateCameraPose( dt );
}


void CMeshSplitterTest::RenderMeshes( binary_node<CSplitMeshNodeObjects>& node, CShaderManager& shader_mgr, const Matrix34& parent_transform )
{
	Matrix34 pose = parent_transform * Matrix34( node.object.shift, Matrix33Identity() );
	if( !node.child0 && !node.child1 )
	{
		shader_mgr.SetWorldTransform( pose );

		if( node.object.pMesh )
			node.object.pMesh->Render( shader_mgr );
	}

	if( node.child0 ) RenderMeshes( *(node.child0), shader_mgr, parent_transform );
	if( node.child1 ) RenderMeshes( *(node.child1), shader_mgr, parent_transform );
}


void CMeshSplitterTest::RenderMeshes()
{
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING, true );
	GraphicsDevice().SetRenderState( RenderStateType::DEPTH_TEST, true );
	GraphicsDevice().SetRenderState( RenderStateType::WRITING_INTO_DEPTH_BUFFER, true );

	CShaderManager *pShaderManager = m_Shader.GetShaderManager();
//	if( !pShaderManager )
//		return;

	CShaderManager& shader_mgr = pShaderManager ? *pShaderManager : FixedFunctionPipelineManager();

	// render the scene

	shader_mgr.SetViewerPosition( g_Camera.GetPosition() );

	ShaderManagerHub.PushViewAndProjectionMatrices( g_Camera );

	shader_mgr.SetTechnique( m_MeshTechnique );

	shader_mgr.SetWorldTransform( Matrix44Identity() );

	RenderMeshes( m_RootMeshNode, shader_mgr, Matrix34Identity() );

	ShaderManagerHub.PopViewAndProjectionMatrices_NoRestore();
}


void CMeshSplitterTest::RenderSplitPlane()
{
	Matrix34 pose = m_pSplitPlaneController->GetPose();
	pose.vPosition = Vector3(0,0,0);

//	GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );
	GraphicsDevice().Disable( RenderStateType::LIGHTING );

	CPrimitiveShapeRenderer shape_renderer;
	shape_renderer.RenderBox( Vector3(10.0f, 0.1f, 10.0f), pose, SFloatRGBAColor(0.0f,0.5f,0.5f,0.0f) );
}


void CMeshSplitterTest::Render()
{
	PROFILE_FUNCTION();

	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );
	GraphicsDevice().Enable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );

	SetLight();

	RenderMeshes();

	RenderSplitPlane();

	m_TextBuffer.resize( 0 );
	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

	Vector2 vTopLeft(     GetWindowWidth() / 4,  16 );
	Vector2 vBottomRight( GetWindowWidth() - 16, GetWindowHeight() * 3 / 2 );
	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
//	rect.Draw();

	if( m_pFont )
	{
		m_pFont->DrawText( m_TextBuffer, vTopLeft );

		string cam_pos_text = to_string( g_Camera.GetPosition() );
		m_pFont->DrawText( string("camera position: ") + cam_pos_text, Vector2( 20, 300 ) );
	}
}


void CMeshSplitterTest::SplitMeshesAtLeafNodes( CMeshNode& node, const Plane& split_plane )
{
	if( node.child0 && node.child1 )
	{
		SplitMeshesAtLeafNodes( *(node.child0), split_plane );
		SplitMeshesAtLeafNodes( *(node.child1), split_plane );
		return;
	}

	CConvexMeshSplitter splitter;
	splitter.SplitMesh( *(node.object.pMesh), split_plane );

	CMeshSplitResults results;
	splitter.GetSplitResults( results );
	node.child0.reset( new CMeshNode );
	node.child0->object.pMesh = results.m_pFrontMesh;
	node.child0->object.shift =  split_plane.normal * 0.2f;
	node.child1.reset( new CMeshNode );
	node.child1->object.pMesh = results.m_pBackMesh;
	node.child1->object.shift = -split_plane.normal * 0.2f;
}


void CMeshSplitterTest::SplitMesh()
{
	if( m_RootMeshNode.child0 && m_RootMeshNode.child1 )
		return; // already split

	Matrix34 pose = m_pSplitPlaneController->GetPose();

	Plane split_plane;
	split_plane.normal = pose.matOrient.GetColumn(1);
	split_plane.dist   = Vec3Dot( pose.vPosition, split_plane.normal );

	SplitMeshesAtLeafNodes( m_RootMeshNode, split_plane );
}


void CMeshSplitterTest::UpdateSplitPlaneControllerState()
{
	if( InputHub().GetInputHandler(3) )
		InputHub().GetInputHandler(3)->SetActive( m_ControlSplitPlane );

	if( InputHub().GetInputHandler(ms_CameraControllerInputHandlerIndex) )
		InputHub().GetInputHandler(ms_CameraControllerInputHandlerIndex)->SetActive( !m_ControlSplitPlane );
}


void CMeshSplitterTest::HandleInput( const SInputData& input )
{
	switch( input.iGICode )
	{
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;

	case GIC_SPACE:
		if( input.iType == ITYPE_KEY_PRESSED )
			SplitMesh();
		break;

	case GIC_ENTER:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;

	case 'R':
		if( input.iType == ITYPE_KEY_PRESSED )
			Reset();
		break;

	case 'T':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_ControlSplitPlane = !m_ControlSplitPlane;
			UpdateSplitPlaneControllerState();
		}
		break;

	default:
		break;
	}
}
