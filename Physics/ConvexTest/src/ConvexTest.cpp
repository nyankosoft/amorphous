#include "ConvexTest.hpp"
//#include "gds/Graphics/HemisphericLight.hpp"
//#include "gds/Graphics/Mesh/BasicMesh.hpp"
#include "gds/Graphics/2DPrimitive/2DRect.hpp"
#include "gds/Graphics/Rect.hpp"
#include "gds/Graphics/GraphicsResourceManager.hpp"
#include "gds/Graphics/Camera.hpp"
#include "gds/Graphics/Shader/ShaderManagerHub.hpp"
#include "gds/Graphics/Shader/ShaderManager.hpp"
#include "gds/Graphics/Shader/GenericShaderGenerator.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "gds/Graphics/Shader/ShaderLightManager.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Graphics/MeshUtilities.hpp"
#include "gds/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "gds/Support/Profile.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/Macro.h"
#include "gds/Support/MTRand.hpp"
#include "gds/Utilities/Physics/PhysicsMeshUtility.hpp"
#include "gds/Physics/SceneDesc.hpp"
#include "gds/Physics/Scene.hpp"
#include "gds/Physics/PhysicsEngine.hpp"
#include "gds/Physics/Actor.hpp"
#include "gds/Physics/MaterialDesc.hpp"
#include "gds/Physics/Material.hpp"
#include "gds/Physics/SceneUtility.hpp"

using std::string;
using std::pair;
using std::vector;
using namespace boost;
using namespace physics;



extern CGraphicsTestBase *CreateTestInstance()
{
	return new CConvexTest();
}


extern const std::string GetAppTitle()
{
	return string("ConvexTest");
}


CConvexTest::CConvexTest()
:
m_pPhysicsScene(NULL)
{
	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	g_Camera.SetPosition( Vector3( 0, 1, -120 ) );
//	g_Camera.SetPosition( Vector3( 0, 520, 120 ) );
}


CConvexTest::~CConvexTest()
{
}


bool CConvexTest::InitShader()
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


bool CConvexTest::SetLight()
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
	CTextFileScanner scanner( "params.txt" );
	if( scanner.IsReady() )
	{
		string mesh;
		int quantity = 1;
		for( ; !scanner.End(); scanner.NextLine() )
		{
			if( scanner.TryScanLine( "mesh", mesh, quantity ) )
				mesh_and_quantity_pairs.push_back( pair<string,int>( mesh, quantity ) );
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

	CShaderManager *pShaderManager = m_Shader.GetShaderManager();
//	if( !pShaderManager )
//		return;

	CShaderManager& shader_mgr = pShaderManager ? *pShaderManager : FixedFunctionPipelineManager();

	// render the scene

	shader_mgr.SetViewerPosition( g_Camera.GetPosition() );

	ShaderManagerHub.PushViewAndProjectionMatrices( g_Camera );

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

	ShaderManagerHub.PopViewAndProjectionMatrices_NoRestore();
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


void CConvexTest::HandleInput( const SInputData& input )
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
