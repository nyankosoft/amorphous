#include "MeshSplitterDemo.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/ShaderManagerHub.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Graphics/Font/FontBase.hpp"
#include "amorphous/Graphics/MeshUtilities.hpp"
#include "amorphous/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "amorphous/Graphics/Mesh/ConvexMeshSplitter.hpp"
#include "amorphous/Graphics/PrimitiveShapeRenderer.hpp"
#include "amorphous/Input/InputHub.hpp"
#include "amorphous/Support/Vec3_StringAux.hpp"
#include "amorphous/Support/CameraController.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Macro.h"

using std::string;
using namespace boost;


MeshSplitterDemo::MeshSplitterDemo()
:
m_MeshPose( Matrix34Identity() )
{
	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	m_ControlSplitPlane = false;

	if( CameraController() )
		CameraController()->SetPosition( Vector3( 0, 1, -2 ) );
}


MeshSplitterDemo::~MeshSplitterDemo()
{
}


bool MeshSplitterDemo::InitShader()
{
	m_MeshTechnique.SetTechniqueName( "Default" );

	// initialize shader
//	bool shader_loaded = m_Shader.Load( "./shaders/MeshSplitterDemo.fx" );
	GenericShaderDesc gs_desc;
	gs_desc.Specular = SpecularSource::NONE;
	ShaderResourceDesc shader_desc;
	shader_desc.pShaderGenerator.reset( new GenericShaderGenerator( gs_desc ) );
	bool shader_loaded = m_Shader.Load( shader_desc );
	
	return shader_loaded;
}


bool MeshSplitterDemo::SetLight()
{
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( !pShaderMgr )
		return false;

	ShaderLightManager *pShaderLightMgr = m_Shader.GetShaderManager()->GetShaderLightManager().get();
	if( pShaderLightMgr )
	{
		pShaderLightMgr->ClearLights();

		HemisphericDirectionalLight light;
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


void MeshSplitterDemo::Reset()
{
	m_RootMeshNode.release_children();

//	m_RootMeshNode.object.Release();

	string mesh_pathname;
	LoadParamFromFile( "MeshSplitterDemo/params.txt", "mesh", mesh_pathname );

	const char *builtin_mesh_prefix = "built-in_mesh::";
	if( mesh_pathname.find( builtin_mesh_prefix ) == 0 )
	{
		string builtin_mesh_name = mesh_pathname.substr( strlen(builtin_mesh_prefix) );
		if( builtin_mesh_name == "box" )
		{
			BoxMeshGenerator box_generator;
			box_generator.Generate();
			m_RootMeshNode.object.pMesh.reset( new CustomMesh );
			m_RootMeshNode.object.pMesh->LoadFromArchive( box_generator.MeshArchive() );
		}
		else if( builtin_mesh_name == "sphere" )
		{
			SphereDesc sphere_desc;
			sphere_desc.num_sides = 24;
			sphere_desc.num_segments = 16;
			SphereMeshGenerator sphere_generator(sphere_desc);
			sphere_generator.Generate();
			m_RootMeshNode.object.pMesh.reset( new CustomMesh );
			m_RootMeshNode.object.pMesh->LoadFromArchive( sphere_generator.MeshArchive() );
		}
	}
	else
	{
		mesh_pathname = "MeshSplitterDemo/" + mesh_pathname;
		m_RootMeshNode.object.pMesh.reset( new CustomMesh );
		bool res = m_RootMeshNode.object.pMesh->LoadFromFile( mesh_pathname );
	}
}


int MeshSplitterDemo::Init()
{
	InitShader();

	m_pSplitPlaneController.reset( new amorphous::CameraController( 3 ) );
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


void MeshSplitterDemo::Update( float dt )
{
	if( m_pSplitPlaneController )
		m_pSplitPlaneController->UpdateCameraPose( dt );
}


void MeshSplitterDemo::RenderMeshes( binary_node<CSplitMeshNodeObjects>& node, ShaderManager& shader_mgr, const Matrix34& parent_transform )
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


void MeshSplitterDemo::RenderMeshes()
{
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING, true );
	GraphicsDevice().SetRenderState( RenderStateType::DEPTH_TEST, true );
	GraphicsDevice().SetRenderState( RenderStateType::WRITING_INTO_DEPTH_BUFFER, true );

	ShaderManager *pShaderManager = m_Shader.GetShaderManager();
//	if( !pShaderManager )
//		return;

	ShaderManager& shader_mgr = pShaderManager ? *pShaderManager : FixedFunctionPipelineManager();

	// render the scene

	shader_mgr.SetViewerPosition( GetCurrentCamera().GetPosition() );

	GetShaderManagerHub().PushViewAndProjectionMatrices( GetCurrentCamera() );

	shader_mgr.SetTechnique( m_MeshTechnique );

	shader_mgr.SetWorldTransform( Matrix44Identity() );

	RenderMeshes( m_RootMeshNode, shader_mgr, m_MeshPose );

	GetShaderManagerHub().PopViewAndProjectionMatrices_NoRestore();
}


void MeshSplitterDemo::RenderSplitPlane()
{
	Matrix34 pose = m_pSplitPlaneController->GetPose();
	pose.vPosition = Vector3(0,0,0);

//	GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );
	GraphicsDevice().Disable( RenderStateType::LIGHTING );

	PrimitiveShapeRenderer shape_renderer;
	shape_renderer.RenderBox( Vector3(10.0f, 0.1f, 10.0f), pose, SFloatRGBAColor(0.0f,0.5f,0.5f,0.0f) );
}


void MeshSplitterDemo::Render()
{
	PROFILE_FUNCTION();

	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );
	GraphicsDevice().Enable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );

	SetLight();

	RenderMeshes();

	RenderSplitPlane();
}


void MeshSplitterDemo::SplitMeshesAtLeafNodes( CMeshNode& node, const Plane& split_plane )
{
	if( node.child0 && node.child1 )
	{
		SplitMeshesAtLeafNodes( *(node.child0), split_plane );
		SplitMeshesAtLeafNodes( *(node.child1), split_plane );
		return;
	}

	ConvexMeshSplitter splitter;
	splitter.SplitMesh( *(node.object.pMesh), m_MeshPose, split_plane );

	MeshSplitResults results;
	splitter.GetSplitResults( results );
	node.child0.reset( new CMeshNode );
	node.child0->object.pMesh = results.m_pFrontMesh;
	node.child0->object.shift =  split_plane.normal * 0.2f;
	node.child1.reset( new CMeshNode );
	node.child1->object.pMesh = results.m_pBackMesh;
	node.child1->object.shift = -split_plane.normal * 0.2f;

	node.object.pMesh->DumpToTextFile( ".debug/split_mesh_src.txt" );
	node.child0->object.pMesh->DumpToTextFile( ".debug/split_mesh_child0.txt" );
	node.child1->object.pMesh->DumpToTextFile( ".debug/split_mesh_child1.txt" );
}


void MeshSplitterDemo::SplitMesh()
{
	if( m_RootMeshNode.child0 && m_RootMeshNode.child1 )
		return; // already split

	Matrix34 pose = m_pSplitPlaneController->GetPose();

	Plane split_plane;
	split_plane.normal = pose.matOrient.GetColumn(1);
	split_plane.dist   = Vec3Dot( pose.vPosition, split_plane.normal );

	SplitMeshesAtLeafNodes( m_RootMeshNode, split_plane );
}


void MeshSplitterDemo::UpdateSplitPlaneControllerState()
{
//	if( GetInputHub().GetInputHandler(3) )
//		GetInputHub().GetInputHandler(3)->SetActive( m_ControlSplitPlane );

//	if( GetInputHub().GetInputHandler(ms_CameraControllerInputHandlerIndex) )
//		GetInputHub().GetInputHandler(ms_CameraControllerInputHandlerIndex)->SetActive( !m_ControlSplitPlane );

	if( m_pSplitPlaneController )
		m_pSplitPlaneController->SetActive( m_ControlSplitPlane );

	if( CameraController() )
		CameraController()->SetActive( !m_ControlSplitPlane );
}


void MeshSplitterDemo::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
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
			m_UseCameraControl = !m_UseCameraControl;
			UpdateSplitPlaneControllerState();
		}
		break;

	case 'I':
		if( input.iType == ITYPE_KEY_PRESSED )
			m_MeshPose = Matrix34Identity();
		break;

	case GIC_RIGHT:
		if( input.iType == ITYPE_KEY_PRESSED )
			m_MeshPose.vPosition.x += 0.1f;
		break;

	case GIC_LEFT:
		if( input.iType == ITYPE_KEY_PRESSED )
			m_MeshPose.vPosition.x -= 0.1f;
		break;

	case GIC_UP:
		if( input.iType == ITYPE_KEY_PRESSED )
			m_MeshPose.vPosition.y += 0.1f;
		break;

	case GIC_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
			m_MeshPose.vPosition.y -= 0.1f;
		break;

	default:
		CGraphicsTestBase::HandleInput( input );
		break;
	}
}
