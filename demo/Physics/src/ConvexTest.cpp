#include "ConvexTest.hpp"
//#include "amorphous/Graphics/HemisphericLight.hpp"
//#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/Rect.hpp"
#include "amorphous/Graphics/GraphicsResourceManager.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/Shader/ShaderManagerHub.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Graphics/Font/BuiltinFonts.hpp"
#include "amorphous/Graphics/MeshUtilities.hpp"
#include "amorphous/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/MTRand.hpp"
#include "amorphous/Utilities/Physics/PhysicsMeshUtility.hpp"
#include "amorphous/Physics/SceneDesc.hpp"
#include "amorphous/Physics/Scene.hpp"
#include "amorphous/Physics/PhysicsEngine.hpp"
#include "amorphous/Physics/Actor.hpp"
#include "amorphous/Physics/MaterialDesc.hpp"
#include "amorphous/Physics/Material.hpp"
#include "amorphous/Physics/SceneUtility.hpp"

using std::string;
using std::pair;
using std::vector;
using namespace boost;
using namespace physics;


CConvexTest::CConvexTest()
:
m_pPhysicsScene(NULL)
{
	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	if( GetCameraController() )
	{
		GetCameraController()->SetPosition( Vector3( 0, 2, -20 ) );
//		GetCameraController()->SetPosition( Vector3( 0, 520, 120 ) );
	}
}


CConvexTest::~CConvexTest()
{
}


bool CConvexTest::InitShader()
{
	m_MeshTechnique.SetTechniqueName( "Default" );

	// initialize shader
//	bool shader_loaded = m_Shader.Load( "./shaders/MeshSplitterTest.fx" );
	GenericShaderDesc gs_desc;
	gs_desc.Specular = SpecularSource::NONE;
	ShaderResourceDesc shader_desc;
	shader_desc.pShaderGenerator.reset( new GenericShaderGenerator( gs_desc ) );
	bool shader_loaded = m_Shader.Load( shader_desc );
	
	return shader_loaded;
}


bool CConvexTest::SetLight()
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


void CConvexTest::Reset()
{
}


int CConvexTest::InitPhysics( const vector< pair<string,int> >& mesh_and_quantity_pairs )
{
	bool physics_engine_initialized = PhysicsEngine().Init( "AgeiaPhysX" );

	CSceneDesc scene_desc;
	m_pPhysicsScene = PhysicsEngine().CreateScene( scene_desc );
	if( !m_pPhysicsScene )
		return -1;

	CMaterialDesc mat_desc;
	mat_desc.StaticFriction  = 1.5f;
	mat_desc.DynamicFriction = 1.2f;
	mat_desc.Restitution     = 0.5f;
	physics::CMaterial *pMat = m_pPhysicsScene->CreateMaterial( mat_desc );
	int mat_id = pMat->GetMaterialID();

	CSceneUtility scene_utility( m_pPhysicsScene );
	CActor *pPlaneActor = scene_utility.CreateStaticPlaneActor( Plane( Vector3(0,1,0), 0 ), mat_id );

	float y = 0;
	physics::CPhysicsMeshUtility mesh_utility( m_pPhysicsScene );
	for( size_t i=0; i<mesh_and_quantity_pairs.size(); i++ )
	{
		string pathname = mesh_and_quantity_pairs[i].first;
		for( int j=0; j<mesh_and_quantity_pairs[i].second; j++ )
		{
			m_Actors.push_back( CActorHolder() );
			bool mesh_loaded = m_Actors.back().mesh.LoadFromFile( pathname );
			if( !mesh_loaded )
				continue;

			AABB3 mesh_aabb = m_Actors.back().mesh.GetAABB();

			float x = RangedRand( -mesh_aabb.GetExtents().x, mesh_aabb.GetExtents().x );
			float z = RangedRand( -mesh_aabb.GetExtents().z, mesh_aabb.GetExtents().z );

			m_Actors.back().pActor = mesh_utility.CreateConvexActorFromMesh(
				m_Actors.back().mesh,
				Matrix34( Vector3(x,2+y,z), Matrix33Identity() )
				);

			y += mesh_aabb.GetExtents().y * 2.05f;
		}
	}

	return 0;
}


int CConvexTest::Init()
{
	m_pFont = CreateDefaultBuiltinFont();
	m_pFont->SetFontSize( 6, 12 );

	vector< pair<string,int> > mesh_and_quantity_pairs;
	CTextFileScanner scanner( "ConvexDemo/params.txt" );
	if( scanner.IsReady() )
	{
		string mesh;
		int quantity = 1;
		for( ; !scanner.End(); scanner.NextLine() )
		{
			if( scanner.TryScanLine( "mesh", mesh, quantity ) )
				mesh_and_quantity_pairs.push_back( pair<string,int>( "ConvexDemo/" + mesh, quantity ) );
		}
	}

	InitPhysics( mesh_and_quantity_pairs );

	InitShader();

	// Load a mesh
	Reset();

	return 0;
}


void CConvexTest::Update( float dt )
{
	if( m_pPhysicsScene )
	{
		m_pPhysicsScene->Simulate( dt );

		while( !m_pPhysicsScene->FetchResults( physics::SimulationStatus::RigidBodyFinished ) )
		{}
	}
}


void CConvexTest::RenderMeshes()
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

//	RenderMeshes( m_RootMeshNode, shader_mgr, Matrix34Identity() );
	for( size_t i=0; i<m_Actors.size(); i++ )
	{
		if( !m_Actors[i].pActor )
			continue;

		Matrix34 world_pose = m_Actors[i].pActor->GetWorldPose();

		shader_mgr.SetWorldTransform( world_pose );

		m_Actors[i].mesh.Render( shader_mgr );
	}

	GetShaderManagerHub().PopViewAndProjectionMatrices_NoRestore();
}


void CConvexTest::Render()
{
	PROFILE_FUNCTION();

	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );
	GraphicsDevice().Enable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );

	SetLight();

	RenderMeshes();

	m_TextBuffer.resize( 0 );
	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

	Vector2 vTopLeft(     GetWindowWidth() / 4,  16 );
	Vector2 vBottomRight( GetWindowWidth() - 16, GetWindowHeight() * 3 / 2 );
	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
//	rect.Draw();

	if( m_pFont )
	{
		m_pFont->DrawText( m_TextBuffer, vTopLeft );
	}
}


void CConvexTest::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;

	case GIC_SPACE:
//		if( input.iType == ITYPE_KEY_PRESSED )
//			OnSpacePressed();
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

	default:
		break;
	}
}
