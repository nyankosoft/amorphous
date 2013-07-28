#include "MultiShapeActorsTest.hpp"
#include <boost/filesystem.hpp>
#include "amorphous/Graphics.hpp"
#include "amorphous/Graphics/Font/BuiltinFonts.hpp"
#include "amorphous/Graphics/VertexFormat.hpp"
#include "amorphous/Graphics/ShapesExtractor.hpp"
#include "amorphous/Graphics/PrimitiveShapeRenderer.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Support/Timer.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/MTRand.hpp"
#include "amorphous/Support/Vec3_StringAux.hpp"
#include "amorphous/Utilities/Physics/PhysicsShapeMakerVisitor.hpp"
#include "amorphous/GUI.hpp"
#include "amorphous/Physics.hpp"
#include "amorphous/Physics/SceneUtility.hpp"
#include "amorphous/Physics/RaycastHit.hpp"

using std::string;
using std::vector;
using boost::shared_ptr;
using namespace boost::filesystem;
using namespace physics;


const float CMultiShapeActorsTest::ms_fActorStartInitHeight = 10.0f;

CMultiShapeActorsTest::CMultiShapeActorsTest()
:
m_pPhysScene(NULL),
m_PhysTimestep(0.02f),
m_PhysOverlapTime(0),
m_StartPhysicsSimulation(false),
m_DefaultMaterialID(0),
m_fActorInitHeight( ms_fActorStartInitHeight )
{
	m_MeshTechnique.SetTechniqueName( "Default" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	g_Camera.SetPosition( Vector3( 0, 2, -120 ) );
//	g_Camera.SetPosition( Vector3( 0, 520, 120 ) );

	m_vWindForce.current = Vector3(0,0,0);
	m_vWindForce.target  = Vector3(0,0,0);
	m_vWindForce.vel     = Vector3(0,0,0);
	m_vWindForce.smooth_time = 1.0f;
}


CMultiShapeActorsTest::~CMultiShapeActorsTest()
{
	if( m_pPhysScene )
	{
/*		for( size_t i=0; i<m_RigidBodies.size(); i++ )
			m_RigidBodies[i].Release();
//		m_RigidBody.Release();
*/
		PhysicsEngine().ReleaseScene( m_pPhysScene );
	}
}


void CMultiShapeActorsTest::CreateSampleUI()
{
}


void CMultiShapeActorsTest::SetLights()
{
	ShaderManager *pShaderManager = m_Shader.GetShaderManager();
	ShaderManager& shader_mgr = pShaderManager ? (*pShaderManager) : FixedFunctionPipelineManager();

	ShaderLightManager *pShaderLightMgr = shader_mgr.GetShaderLightManager().get();
	if( !pShaderLightMgr )
		return;

	pShaderLightMgr->ClearLights();

//	DirectionalLight dir_light;
//	dir_light.DiffuseColor = SFloatRGBColor(1,1,1);
//	dir_light.fIntensity = 1.0f;
//	dir_light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -2.5f, -0.9f ) );
//	pShaderLightMgr->SetDirectionalLight( dir_light );

	bool set_pnt_light = false;
	if( set_pnt_light )
	{
		PointLight pnt_light;
		pnt_light.DiffuseColor = SFloatRGBColor(1,1,1);
		pnt_light.fIntensity = 1.0f;
		pnt_light.vPosition = Vector3( 0.3f, 3.5f, -1.9f );
		pnt_light.fAttenuation[0] = 1.0f;
		pnt_light.fAttenuation[1] = 1.0f;
		pnt_light.fAttenuation[2] = 0.1f;
		pShaderLightMgr->SetPointLight( pnt_light );
	}

	HemisphericDirectionalLight hdir_light;
	hdir_light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	hdir_light.Attribute.LowerDiffuseColor.SetRGBA( 0.3f, 0.3f, 0.3f, 1.0f );
	hdir_light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -2.5f, -0.9f ) );
	pShaderLightMgr->SetHemisphericDirectionalLight( hdir_light );

	pShaderLightMgr->CommitChanges();
}


bool CMultiShapeActorsTest::InitShader()
{
	// initialize shader
	GenericShaderDesc gsd;
	ShaderResourceDesc sd;
	gsd.Specular = SpecularSource::NONE;
	sd.pShaderGenerator.reset( new GenericShaderGenerator(gsd) );
	bool shader_loaded = m_Shader.Load( sd );
	if( !shader_loaded )
		return false;


//	string shader_filepath = LoadParamFromFile<string>( "config", "Shader" );

	SetLights();

//	Matrix44 matProj
//		= Matrix44PerspectiveFoV_LH( (float)PI / 4, 640.0f / 480.0f, 0.1f, 500.0f );
//
//	if( m_Shader.GetShaderManager() )
//		m_Shader.GetShaderManager()->SetProjectionTransform( matProj );

	return true;
}


void CMultiShapeActorsTest::CreateActors( const std::string& model, int quantity )
{
	if( !m_pPhysScene )
		return;

	CActorDesc actor_desc;

	CPhysicsShapeMakerVisitor visitor( actor_desc.vecpShapeDesc );
	visitor;
	string shape_descs_file( model );
	lfs::change_ext( shape_descs_file, "sd" );
	if( exists( shape_descs_file ) )
	{
		ShapeContainerSet shape_containers;
		shape_containers.LoadFromFile( shape_descs_file );

		for( int i=0; i<(int)shape_containers.m_pShapes.size(); i++ )
		{
			shape_containers.m_pShapes[i]->Accept( visitor );
		}
	}

	// 'visitor' contains shape descs

	actor_desc.BodyDesc.fMass = 1.0f;

	for( int i=0; i<quantity; i++ )
	{
		actor_desc.WorldPose
			= Matrix34(
//			Vector3( RangedRand( -3.0f, 3.0f ), RangedRand( 10.0f, 16.0f ), RangedRand( -3.0f, 3.0f ) ),
			Vector3( RangedRand( -1.0f, 1.0f ), m_fActorInitHeight, RangedRand( -1.0f, 1.0f ) ),
			Matrix33RotationY( RangedRand( 0.0f, 360.0f ) )
			);

		m_fActorInitHeight += 1.2f;

		actor_desc.vecpShapeDesc = visitor.m_pShapeDescs;

		for( size_t j=0; j<actor_desc.vecpShapeDesc.size(); j++ )
			actor_desc.vecpShapeDesc[j]->MaterialIndex = m_DefaultMaterialID;

		CActor *pActor = m_pPhysScene->CreateActor( actor_desc );
		if( pActor )
		{
			pActor->UpdateMassFromShapes( 0, 1.0f );
			m_pActors.push_back( pActor );
		}
	}
}


void CMultiShapeActorsTest::InitPhysicsEngine()
{
	physics::PhysicsEngine().Init();

	CSceneDesc scene_desc;
	m_pPhysScene = PhysicsEngine().CreateScene( scene_desc );
	if( !m_pPhysScene )
		return;

	CMaterialDesc mat_desc;
	mat_desc.StaticFriction  = 1.5f;
	mat_desc.DynamicFriction = 1.2f;
	mat_desc.Restitution     = 0.0f;
	physics::CMaterial *pMat = m_pPhysScene->CreateMaterial( mat_desc );
	m_DefaultMaterialID = pMat->GetMaterialID();

	m_pPhysScene->SetGravity( Vector3(  0.0f, -9.8f, 0.0f ) );

	CTextFileScanner scanner( "MultiShapeActorsDemo/params.txt" );
	if( !scanner.IsReady() )
		return;

	for( ; !scanner.End(); scanner.NextLine() )
	{
		string model;
		int quantity = 0;
		if( scanner.TryScanLine( "model", model, quantity ) )
		{
			model = "MultiShapeActorsDemo/" + model;

			size_t num_prev_actors = m_pActors.size();
			uint mesh_id = (uint)m_Meshes.size();
			m_Meshes.push_back( MeshHandle() );
			bool mesh_loaded = m_Meshes.back().Load( model );
			if( !mesh_loaded )
				continue;

			CreateActors( model, quantity );

			size_t num_added_actors = m_pActors.size() - num_prev_actors;
			m_ActorMeshIDs.insert( m_ActorMeshIDs.end(), num_added_actors, mesh_id );
		}
	}


	CSceneUtility util(m_pPhysScene);
	CActor *pBoxActor = util.CreateStaticBoxActor( Vector3(100,1,100), Matrix34(Vector3(0,-1,0),Matrix33Identity()) );
//	pBoxActor->GetShape(0)->SetMaterial(  );
//	pBoxActor->SetWorldPose( Matrix34Identity() );

	// create cloth

	// horizontal
//	Vector3 box_edges = Vector3(0.2f,0.2f,0.2f);
//	Matrix34 box_pose = Matrix34( Vector3(0.0f,2.0f,0.0f), Matrix33Identity() );

	// vertical
	Vector3 box_edges = Vector3(1.2f,0.2f,0.2f);
	Matrix34 box_pose = Matrix34( Vector3(0.0f,2.0f,0.0f), Matrix33Identity() );
	Matrix34 cloth_pose
		= Matrix34( Vector3(0,2.0f,0), Matrix33Identity() )
		* Matrix34( Vector3(0,0,0), Matrix33RotationX(deg_to_rad(45.0f)) )
		* Matrix34( Vector3(0,-0.5f,0), Matrix33Identity() );
}


int CMultiShapeActorsTest::Init()
{
	m_pFont = CreateDefaultBuiltinFont();

	m_Meshes.push_back( MeshHandle() );
	shared_ptr<BoxMeshGenerator> pBoxMeshGenerator( new BoxMeshGenerator() );
	pBoxMeshGenerator->SetEdgeLengths( Vector3(1,1,1) );
	pBoxMeshGenerator->SetDiffuseColor( SFloatRGBAColor::White() );
	MeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator = pBoxMeshGenerator;
//	m_Meshes.back().m_MeshDesc.OptionFlags |= GraphicsResourceOption::DONT_SHARE;
	m_Meshes.back().Load( mesh_desc );

/*
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NoShader" );
*/


	// init shader
	InitShader();

	InitPhysicsEngine();

	return 0;
}

void CMultiShapeActorsTest::UpdatePhysics( float dt )
{
	if( !m_StartPhysicsSimulation )
		return;

	if( !m_pPhysScene )
		return;

	Scalar total_time = dt + m_PhysOverlapTime;

	if (total_time > 0.1f)
		total_time = 0.1f;

	// split the timestep into fixed size chunks

	int num_loops = (int) (total_time / m_PhysTimestep);
	Scalar timestep = m_PhysTimestep;

	if ( false /*m_allow_smaller_timesteps*/ )
	{
		if (num_loops == 0)
			num_loops = 1;
		timestep = total_time / num_loops;
	}

	m_PhysOverlapTime = total_time - num_loops * timestep;
	for(int i=0; i<num_loops ; ++i)
	{
//		PROFILE_SCOPE( "Physics Simulation" );

		// handle the motions and collisions of rigid body entities
		m_pPhysScene->Simulate( timestep );

		while( !m_pPhysScene->FetchResults( physics::SimulationStatus::RigidBodyFinished ) )
		{}
	}
}


void CMultiShapeActorsTest::Update( float dt )
{
	if( m_pSampleUI )
		m_pSampleUI->Update( dt );

	float pitch = 0, yaw = 0;
	GaussianRand(pitch,yaw);
	m_vWindForce.target
		= Matrix33RotationY(yaw)
		* Matrix33RotationX(pitch)
		* Vector3(0,0,1);

	Vec3Normalize( m_vWindForce.target, m_vWindForce.target );
	m_vWindForce.target *= 5.0f;

	m_vWindForce.Update( dt );

	if( IsKeyPressed( (KeyCode::Code)'Y' ) )
		MoveClothHolderActor( Vector3( 0, 0, 1) * dt );
	if( IsKeyPressed( (KeyCode::Code)'H' ) )
		MoveClothHolderActor( Vector3( 0, 0,-1) * dt );
	if( IsKeyPressed( (KeyCode::Code)'J' ) )
		MoveClothHolderActor( Vector3( 1, 0, 0) * dt );
	if( IsKeyPressed( (KeyCode::Code)'G' ) )
		MoveClothHolderActor( Vector3(-1, 0, 0) * dt );
	if( IsKeyPressed( (KeyCode::Code)'U' ) )
		MoveClothHolderActor( Vector3( 0, 1, 0) * dt );
	if( IsKeyPressed( (KeyCode::Code)'T' ) )
		MoveClothHolderActor( Vector3( 0,-1, 0) * dt );

	UpdatePhysics( dt );
}


void CMultiShapeActorsTest::RenderMeshes()
{
	ShaderManager *pShaderManager = m_Shader.GetShaderManager();
	ShaderManager& shader_mgr = pShaderManager ? (*pShaderManager) : FixedFunctionPipelineManager();

	// render the scene
/*
	pShaderManager->SetViewerPosition( g_Camera.GetPosition() );
*/
	GetShaderManagerHub().PushViewAndProjectionMatrices( GetCurrentCamera() );

	shader_mgr.SetWorldTransform( ToMatrix44( Matrix34(Vector3(0.0f,0.05f,0.0f),Matrix33Identity()) ) * Matrix44Scaling( 100.0f, 100.0f, 0.1f ) );

	const int num_actors = (int)m_pActors.size();
	for( int i=0; i<num_actors; i++ )
	{
		if( !m_pActors[i] )
			continue;

		shader_mgr.SetWorldTransform( m_pActors[i]->GetWorldPose() );

		if( (int)m_ActorMeshIDs.size() <= i )
			continue;

		if( (uint)m_Meshes.size() <= m_ActorMeshIDs[i] )
			continue;

		boost::shared_ptr<BasicMesh> pMesh = m_Meshes[ m_ActorMeshIDs[i] ].GetMesh();
		if( !pMesh )
			continue;

//		pMesh->Render( shader_mgr );

		shader_mgr.SetWorldTransform( Matrix44Identity() );
		const int num_shapes = m_pActors[i]->GetNumShapes();
		const Matrix34 actor_world_pose = m_pActors[i]->GetWorldPose();
		PrimitiveShapeRenderer renderer;
		renderer.SetShader( m_Shader );
		for( int j=0; j<num_shapes; j++ )
		{
			if( m_pActors[i]->GetShape(j)->GetType() == PhysShape::Box )
			{
				CBoxShape *pBox = dynamic_cast<CBoxShape *>( m_pActors[i]->GetShape(j) );
				if( !pBox )
					continue;

				renderer.RenderBox( pBox->GetDimensions() * 2.0f, actor_world_pose * m_pActors[i]->GetShape(j)->GetLocalPose() );
			}
		}

	}

	GetShaderManagerHub().PopViewAndProjectionMatrices_NoRestore();
}


void CMultiShapeActorsTest::Render()
{
	PROFILE_FUNCTION();

	GraphicsDevice().SetRenderState( RenderStateType::DEPTH_TEST, true );

	FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );

	SetLights();

	Result::Name res = Result::SUCCESS;
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( pShaderMgr )
		res = pShaderMgr->SetTechnique( m_MeshTechnique );

	PrimitiveShapeRenderer renderer;
	renderer.SetShader( m_Shader );
	renderer.RenderAxisAlignedPlane();

	RenderMeshes();

	if( m_pSampleUI )
		m_pSampleUI->Render();

	Vector3 vCamPos = GetCurrentCamera().GetPosition();
	m_pFont->DrawText( to_string( vCamPos ), Vector2( 20, 300 ) );
}



void CMultiShapeActorsTest::MoveClothHolderActor( const Vector3& dist )
{
/*	Actor *pActor = m_RigidBodies[0].m_pActor;
	if( !pActor )
		return;s

	Matrix34 pose = pActor->GetWorldPose();
	pose.vPosition += dist;
	pActor->SetWorldPose( pose );*/
}


void CMultiShapeActorsTest::HandleInput( const InputData& input )
{
/*	if( m_pUIInputHandler )
	{
//		CInputHandler::ProcessInput() does not take const SInputData&
		SInputData input_copy = input;
		m_pUIInputHandler->ProcessInput( input_copy );

		if( m_pUIInputHandler->PrevInputProcessed() )
			return;
	}*/

	static int m_CurrentMouseX = -1;
	static int m_CurrentMouseY = -1;
	static bool m_MouseLButtonPressed = false;
	static bool m_MouseRButtonPressed = false;
	static bool m_ShiftKeyPressed = false;

	int x=0,y=0;//,dx=0,dy=0;

	switch( input.iGICode )
	{
	case 'C':
	case GIC_LSHIFT:
		m_ShiftKeyPressed = ( input.iType == ITYPE_KEY_PRESSED ) ? true : false;
		m_CurrentMouseX = -1;
		m_CurrentMouseY = -1;
		break;
	case GIC_MOUSE_BUTTON_L:
		m_MouseLButtonPressed = ( input.iType == ITYPE_KEY_PRESSED ) ? true : false;
		break;
	case GIC_MOUSE_BUTTON_R:
		m_MouseRButtonPressed = ( input.iType == ITYPE_KEY_PRESSED ) ? true : false;
		break;
	case GIC_MOUSE_AXIS_X:
		if( !m_StartPhysicsSimulation || !m_ShiftKeyPressed )
		{
			m_CurrentMouseX = x;
			m_CurrentMouseY = y;
			break;
		}

		x = (int)input.GetParamH16();
		if( m_CurrentMouseX < 0 )
			m_CurrentMouseX = x; // init
		else
		{
			MoveClothHolderActor( Vector3( (float)(x - m_CurrentMouseX) * 0.005f, 0.0f, 0.0f ) );
			m_CurrentMouseX = x;
		}
		break;
	case GIC_MOUSE_AXIS_Y:
		if( !m_StartPhysicsSimulation || !m_ShiftKeyPressed )
		{
			m_CurrentMouseX = x;
			m_CurrentMouseY = y;
			break;
		}

		y = (int)input.GetParamL16();
		if( m_CurrentMouseY < 0 )
			m_CurrentMouseY = y; // init
		else
		{
			MoveClothHolderActor( Vector3( 0.0f, (float)(y - m_CurrentMouseY) * 0.005f, 0.0f ) );
			m_CurrentMouseY = y;
		}
		break;
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
//			SaveTexturesAsImageFiles();
		}
		break;
	case GIC_SPACE:
		if( input.iType == ITYPE_KEY_PRESSED )
			m_StartPhysicsSimulation = true;
		break;
	case GIC_ENTER:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
//			m_pSampleUI->GetDialog(UIID_DLG_RESOLUTION)->Open();
		}
		break;
	case 'V':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			physics::CRay ray;
			ray.Direction = g_Camera.GetPose().matOrient.GetColumn(2);
			ray.Origin    = g_Camera.GetPose().vPosition;
			if( !m_pPhysScene )
				return;
			
			CRaycastHit hit;
			CShape *pShape = m_pPhysScene->RaycastClosestShape( ray, hit, 0, 100.0f );
			if( !pShape )
				return;

			pShape->GetActor().AddWorldForce( ray.Direction, ForceMode::Impulse );
		}
		break;
	default:
		break;
	}
}
