#include "SkeletalCharacter.hpp"
#include "gds/Input/InputHub.hpp"
#include "gds/Input/InputDevice.hpp"
#include "gds/Graphics/Mesh/SkeletalMesh.hpp"
#include "gds/Graphics/Shader/GenericShaderGenerator.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "gds/Graphics/PrimitiveRenderer.hpp"
#include "gds/3DMath/3DGameMath.hpp"
#include "gds/MotionSynthesis/MotionDatabase.hpp"
#include "gds/MotionSynthesis/MotionPrimitiveBlender.hpp"
#include "gds/Support/DebugOutput.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Stage/BaseEntity_Draw.hpp"
#include "gds/Stage/MeshBonesUpdateCallback.hpp"
#include "gds/Physics/Actor.hpp"
#include "gds/Physics/Scene.hpp"
#include "gds/Physics/ContactStreamIterator.hpp"
#include "gds/Item/ItemDatabaseManager.hpp"
#include "gds/Item/Clothing.hpp"
#include "gds/GameCommon/ClothSystem.hpp"
#include <boost/filesystem.hpp>


namespace amorphous
{

using std::string;
using std::vector;
using std::map;
using std::pair;
using boost::shared_ptr;
using namespace boost::filesystem;
using namespace msynth;


static int sg_DisplayCharacterPosition = 0;

class CDebugItem_MotionFSM : public CDebugItem_ResourceManager
{
public:
	boost::shared_ptr<msynth::CMotionFSMManager> m_pMotionFSMManager;
public:

	CDebugItem_MotionFSM() {}

	void GetTextInfo()
	{
		m_pMotionFSMManager->GetDebugInfo( m_TextBuffer );
	}
};


/// For debugging
void DisplayCharacterPosition( const Vector3& world_position )
{
	vector<Vector3> points;
	points.resize( 5 );
	points[0] = world_position + Vector3( 0.5f, 0.1f, 0.5f);
	points[1] = world_position + Vector3( 0.5f, 0.1f,-0.5f);
	points[2] = world_position + Vector3(-0.5f, 0.1f,-0.5f);
	points[3] = world_position + Vector3(-0.5f, 0.1f, 0.5f);
	points[4] = points[0];
	FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );
	GetPrimitiveRenderer().DrawConnectedLines( points, SFloatRGBAColor::White() );
}


inline boost::shared_ptr<SkeletalMesh> SkeletalCharacter::GetCharacterSkeletalMesh()
{
	if( m_MeshContainerRootNode.GetNumMeshContainers() == 0 )
		return shared_ptr<SkeletalMesh>();

	shared_ptr<MeshObjectContainer> pContainer = m_MeshContainerRootNode.GetMeshContainer( 0 );
	if( !pContainer )
		return shared_ptr<SkeletalMesh>();

	shared_ptr<BasicMesh> pMesh = pContainer->m_MeshObjectHandle.GetMesh();
	if( !pMesh )
		return shared_ptr<SkeletalMesh>();

	shared_ptr<SkeletalMesh> pSkeletalMesh = boost::dynamic_pointer_cast<SkeletalMesh,BasicMesh>( pMesh );

	return pSkeletalMesh;
}

int SkeletalCharacter::ms_DefaultInputHandlerIndex = 2;

SkeletalCharacter::SkeletalCharacter()
:
m_fFwdSpeed(0.0f),
m_fTurnSpeed(0.0f),
m_fMaxTurnSpeed(0.0f),
m_fFloorHeight(0.0f),
m_FeetOnGround( true ),
m_CameraDependentMotionControl( true ),
m_vDesiredHorizontalDirection( Vector3(0,0,0) )
{
	// The mesh
	// Previously the character's skeletal mesh was loaded here

	m_pRenderMethod.reset( new MeshContainerRenderMethod );
//	m_pRenderMethod = shared_new<MeshContainerRenderMethod>();

//	m_pRenderMethod->MeshRenderMethod().resize( 1 );
//	m_pRenderMethod->MeshRenderMethod()[0].m_ShaderFilepath = "Shader/VertexBlend.fx";
/*	m_pRenderMethod->MeshRenderMethod()[0].m_ShaderFilepath = "Shader/Default.fx";
	m_pRenderMethod->MeshRenderMethod()[0].m_Technique.SetTechniqueName( "VertBlend_PVL_HSLs" );
//	m_pRenderMethod->MeshRenderMethod()[0].m_Technique.SetTechniqueName( "SingleHSDL_Specular_CTS" );*/
	m_pRenderMethod->LoadRenderMethodResources();

	// TODO: add shader resource desc as a member variable to SubsetRenderMethod
	ShaderResourceDesc shader_desc;
	GenericShaderDesc gen_shader_desc;
	gen_shader_desc.Specular = SpecularSource::DECAL_TEX_ALPHA;
	gen_shader_desc.VertexBlendType = CVertexBlendType::QUATERNION_AND_VECTOR3;
	shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(gen_shader_desc) );
	m_pRenderMethod->PrimaryMeshRenderMethod().m_Shader.Load( shader_desc );
	m_pRenderMethod->PrimaryMeshRenderMethod().m_Technique.SetTechniqueName( "Default" );

	// The input handler
	InitInputHandler( ms_DefaultInputHandlerIndex );

//	InitMotionFSMs();

//	InitClothSystem();

	LoadParamFromFile( ".debug/skeletal_character.txt", "camera_dependent_motion_control", m_CameraDependentMotionControl );
}


Result::Name SkeletalCharacter::InitClothSystem()
{
	if( !m_pSkeletonSrcMotion )
		return Result::UNKNOWN_ERROR;

	m_pClothSystem.reset( new CClothSystem );
	m_pClothSystem->InitPhysics();

	Result::Name res = Result::SUCCESS;
	res = m_pClothSystem->InitMotionSystem( m_pSkeletonSrcMotion->GetSkeleton() );
//	res = m_pClothSystem->AddCollisionSphere( "head",      Sphere(Vector3(0,0,0),0.5f) );
	res = m_pClothSystem->AddCollisionSphere( "spine1",    Sphere(Vector3(0,0,0),0.18f) );
	res = m_pClothSystem->AddCollisionSphere( "spine2",    Sphere(Vector3(0,0,0),0.1f) );
	res = m_pClothSystem->AddCollisionSphere( "spine3",    Sphere(Vector3(0,0,0),0.18f) );
//	res = m_pClothSystem->AddCollisionSphere( "spine4",    Sphere(Vector3(0,0,0),0.5f) );
	res = m_pClothSystem->AddCollisionSphere( "r-thigh",   Sphere(Vector3(0,0,0),0.12f) );
	res = m_pClothSystem->AddCollisionSphere( "l-thigh",   Sphere(Vector3(0,0,0),0.12f) );
	res = m_pClothSystem->AddCollisionSphere( "r-leg",     Sphere(Vector3(0,0,0),0.1f) );
	res = m_pClothSystem->AddCollisionSphere( "l-leg",     Sphere(Vector3(0,0,0),0.1f) );
	res = m_pClothSystem->AddCollisionSphere( "r-foot",    Sphere(Vector3(0,0,0),0.1f) );
	res = m_pClothSystem->AddCollisionSphere( "l-foot",    Sphere(Vector3(0,0,0),0.1f) );
	res = m_pClothSystem->AddCollisionSphere( "r-toe",     Sphere(Vector3(0,0,0),0.06f) );
	res = m_pClothSystem->AddCollisionSphere( "l-toe",     Sphere(Vector3(0,0,0),0.06f) );
	res = m_pClothSystem->AddCollisionSphere( "r-arm",     Sphere(Vector3(0,0,0),0.1f) );
	res = m_pClothSystem->AddCollisionSphere( "l-arm",     Sphere(Vector3(0,0,0),0.1f) );
	res = m_pClothSystem->AddCollisionSphere( "r-forearm", Sphere(Vector3(0,0,0),0.1f) );
	res = m_pClothSystem->AddCollisionSphere( "l-forearm", Sphere(Vector3(0,0,0),0.1f) );
	res = m_pClothSystem->AddCollisionSphere( "r-hand",    Sphere(Vector3(0,0,0),0.1f) );
	res = m_pClothSystem->AddCollisionSphere( "l-hand",    Sphere(Vector3(0,0,0),0.1f) );
/*
	shared_ptr<Clothing> pClothes = ItemDatabaseManager().GetItem<CClothing>( "vest", 1 );
	if( pClothes )
	{
//		m_pClothSystem->AddClothesMesh( pClothes->GetClothesMesh() );

		// How to attach fixed vertices to the mesh
		// Physics shapes are needed to fix the vertices
		// The physics shapes are transformed the same way as the skeleton of the character
	}*/

	// Debug - dump the skeleton for the motion data and the skeleton of the mesh
	if( exists( ".debug" ) )
	{
		m_pSkeletonSrcMotion->GetSkeleton()->DumpToTextFile( ".debug/msynth_skeleton.txt" );
		shared_ptr<SkeletalMesh> pSMesh = GetCharacterSkeletalMesh();
		if( pSMesh )
			pSMesh->DumpSkeletonToTextFile( ".debug/mesh_skeleton.txt" );
	}

	return Result::SUCCESS;
}


void SkeletalCharacter::InitInputHandler( int input_handler_index )
{
	if( m_pInputHandler )
		GetInputHub().RemoveInputHandler( m_pInputHandler.get() );

	m_pInputHandler.reset( new CInputDataDelegate<SkeletalCharacter>( this ) );

	if( GetInputHub().GetInputHandler(input_handler_index) )
		GetInputHub().GetInputHandler(input_handler_index)->AddChild( m_pInputHandler.get() );
	else
		GetInputHub().PushInputHandler( input_handler_index, m_pInputHandler.get() );
}


Result::Name SkeletalCharacter::InitMotionFSMs( const std::string& motion_fsm_file )
{
	m_pMotionFSMManager.reset( new CMotionFSMManager );

	// Load from the binary archive
	bool loaded_from_archive = m_pMotionFSMManager->LoadFromFile( motion_fsm_file );

	if( !loaded_from_archive )
	{
		LOG_PRINT_ERROR( " Failed to load the motion FSM file: " + motion_fsm_file );
		return Result::INVALID_ARGS;
	}

	if( loaded_from_archive )
		m_pMotionFSMManager->LoadMotions();

	m_pLowerLimbsMotionsFSM = m_pMotionFSMManager->GetMotionFSM( "lower_limbs" );

	if( !m_pLowerLimbsMotionsFSM )
		m_pLowerLimbsMotionsFSM.reset( new CMotionFSM ); // avoid NULL checking

	// Init input handler
	if( !m_pInputHandler )
		InitInputHandler( ms_DefaultInputHandlerIndex );

	m_pMotionFSMInputHandler.reset( new CMotionFSMInputHandler(m_pMotionFSMManager) );
	m_pInputHandler->AddChild( m_pMotionFSMInputHandler.get() );

	// save a motion primitive to get skeleton info in Render()
//	string mdb_filepath = "motions/lws-fwd.mdb";
//	CMotionDatabase mdb( mdb_filepath );
//	m_pSkeletonSrcMotion = mdb.GetMotionPrimitive("standing");
	m_pSkeletonSrcMotion = m_pMotionFSMManager->GetCompleteSkeletonSourceMotion();


	m_pLowerLimbsMotionsFSM->StartMotion("standing");

	CDebugItem_MotionFSM *pDbgItem = new CDebugItem_MotionFSM;
	pDbgItem->m_pMotionFSMManager = m_pMotionFSMManager;
	DebugOutput.AddDebugItem( "motion_graph_mgr", pDbgItem );

	return Result::SUCCESS;
}


void SkeletalCharacter::OnEntityCreated( CCopyEntity& entity )
{
//	shared_ptr<CCopyEntity> pEntity = m_Entity.Get();
//	if( !pEntity )
//		return;

//	CCopyEntity& entity = *pEntity;

	m_PrevWorldPose = entity.GetWorldPose();

	entity.m_MeshHandle = m_MeshContainerRootNode.GetMeshContainer(0)->m_MeshObjectHandle;
	entity.m_pMeshRenderMethod = m_pRenderMethod;

	entity.RaiseEntityFlags( BETYPE_SHADOW_CASTER );
	entity.RaiseEntityFlags( BETYPE_SHADOW_RECEIVER );

	Vector3 vImpactNormal = Vector3(0,1,0);
	m_fFloorHeight = GetFloorHeight( entity, vImpactNormal );
}

static void UpdatePhysicsActorPose( physics::CActor& actor, const Matrix34& world_pose )
{
	Vector3 vPhysActorOffset = Vector3( 0, 1, 0 );
	Matrix34 phys_actor_world_pose( world_pose );
	phys_actor_world_pose.vPosition += vPhysActorOffset;
	phys_actor_world_pose.matOrient = Matrix33Identity(); // always keep the upright orientation
	actor.SetWorldPose( phys_actor_world_pose );
}
/*
static void UpdatePoseStoredInMotionPrimitivePlayer(
	boost::shared_ptr<msynth::CMotionFSMManager>& pMotionFSMManager,
	const Matrix34& pose )
{
	shared_ptr<CMotionFSM> pFSM = pMotionFSMManager->GetMotionFSM("lower_limbs");
	if( !pFSM )
		return;

	pFSM->Player()->SetCurrentHorizontalPose( pose );
}
*/

void SkeletalCharacter::Update( float dt )
{
	shared_ptr<CMotionFSM> pFSM = m_pLowerLimbsMotionsFSM;
	if( !pFSM )
		return;

	shared_ptr<CCopyEntity> pEntity = m_Entity.Get();
	if( !pEntity )
		return;

	CCopyEntity& entity = *pEntity;

	Matrix34 world_pose = entity.GetWorldPose();

	m_PrevWorldPose = world_pose;

	// steering
	if( IsCameraDependentMotionControlEnabled() )
	{
		TurnIfNecessary( dt, m_fMaxTurnSpeed );
		world_pose.matOrient = entity.GetWorldPose().matOrient;
	}
	else
		world_pose.matOrient = world_pose.matOrient * Matrix33RotationY( GetTurnSpeed() * dt );

	pFSM->Player()->SetCurrentHorizontalPose( world_pose );

	m_pMotionFSMManager->Update( dt );

	Matrix34 updated_world_pose = pFSM->Player()->GetCurrentHorizontalPose();

	// test collision

	Vector3 vHDir = updated_world_pose.matOrient.GetColumn(2) - world_pose.matOrient.GetColumn(2);
	for( size_t i=0; i<m_Walls.size(); i++ )
	{
		if( Vec3Dot( vHDir, m_Walls[i].normal ) < -0.005 )
		{
			// blocked by the wall
			updated_world_pose = world_pose;
			break;
		}
	}

	static bool s_prev_feet_on_ground = m_FeetOnGround;

	// Update the vertical position
	UpdateStepHeight( entity );

	if( m_FeetOnGround )
//	if( s_prev_feet_on_ground )
	{
		updated_world_pose.vPosition.y = m_fFloorHeight;
	}
	else
	{
		// Freefall - update the position and the velocity.
		updated_world_pose.vPosition.y = world_pose.vPosition.y;
		entity.SetVelocity( entity.Velocity() + entity.GetStage()->GetGravityAccel() * dt );
		Vector3 vVerticalMove = entity.Velocity() * dt;
		updated_world_pose.vPosition += vVerticalMove;
	}

	s_prev_feet_on_ground = m_FeetOnGround;

	m_Walls.resize( 0 );

	// the world pose of the entity -> always stays horizontal
	entity.SetWorldPose( updated_world_pose );
//	entity.SetWorldPose( Matrix34Identity() );

	if( 0 < entity.m_vecpPhysicsActor.size()
	 && entity.m_vecpPhysicsActor[0] )
	{
		physics::CActor& actor = *(entity.m_vecpPhysicsActor[0]);
		UpdatePhysicsActorPose( actor, updated_world_pose );
	}

	for( size_t i=0; i<m_pOperations.size(); i++ )
	{
		m_pOperations[i]->Update( dt );
	}

//	Matrix34 world_pose = m_pMotionFSMManager->GetCurrentWorldPose();

/*
	const Matrix34 world_pose = m_pMotionFSMManager->GetRootNodeWorldPose();
	SetWorldPose( world_pose );
	shared_ptr<CCopyEntity> pEntity = m_Entity.Get();
*/
}


void SkeletalCharacter::Render()
{
	shared_ptr<SkeletalMesh> pSkeletalMesh = GetCharacterSkeletalMesh();
	if( !pSkeletalMesh )
	{
		int skeletal_mesh_not_found = 1;
		return;
	}

//	pSkeletalMesh->Render();

	if( !m_pSkeletonSrcMotion )
		return;

	shared_ptr<CCopyEntity> pEntity = m_Entity.Get();
	if( !pEntity )
		return;

	if( sg_DisplayCharacterPosition )
		DisplayCharacterPosition( pEntity->GetWorldPosition() );

	// Get the hierarchical transforms for the current pose of the character
	CKeyframe& current_keyframe = m_CurrentInterpolatedKeyframe;

	// The previous version of the mesh bone update routine
	// - Blend transforms are now calculated in UpdateGraphics()
	// - m_pClothSystem->UpdateCollisionObjectPoses() still needs this
	UpdateMeshBoneTransforms( current_keyframe, *(m_pSkeletonSrcMotion->GetSkeleton()), *pSkeletalMesh );

	// Render the character's skeletal mesh
	pSkeletalMesh->CalculateBlendTransformsFromCachedLocalTransforms();
///	Matrix34 pose = Matrix34Identity();
	Matrix34 pose = pEntity->GetWorldPose();
	m_pRenderMethod->RenderMeshContainer( *(m_MeshContainerRootNode.MeshContainer( 0 )), pose );

	if( m_pClothSystem )
	{
//		m_pClothSystem->UpdateCollisionObjectPoses( dest, pose );
		m_pClothSystem->UpdateCollisionObjectPoses( *pSkeletalMesh, pose );
		m_pClothSystem->RenderObjectsForDebugging();
	}

	for( int i=0; i<(int)m_pClothes.size(); i++ )
	{
		if( !m_pClothes[i] )
			continue;

		Clothing& clothes = *m_pClothes[i];

/*		if( clothes.HasSkeleton() )
		{
		}
		else
		{
		}

		if( clothes.UseClothSimulation() )
		{
		}
		else
		{
		}*/

//		clothes;
	}

}


void SkeletalCharacter::SetKeyBind( shared_ptr<CKeyBind> pKeyBind )
{
	m_pKeyBind = pKeyBind;
//	for( size_t i=0; i<m_pMotionNodes.size(); i++ )
//		m_pMotionNodes[i]->SetKeyBind( m_pKeyBind );

	// Create map from action code to GI codes

//	m_pKeyBind->Update( m_ACtoGIC );

	int action_codes_to_update[] = {
		ACTION_MOV_FORWARD,
		ACTION_MOV_BOOST,
		ACTION_MOV_JUMP,
		ACTION_CAMERA_ALIGN
	};

	for( int i=0; i<CKeyBind::NUM_ACTION_TYPES; i++ )
	{
		map<int, vector<int> >& ac_to_gics = m_ACtoGICs.m_mapActionCodeToGICodes[i];

		for( int j=0; j<numof(action_codes_to_update); j++ )
		{
			int action_code = action_codes_to_update[j];
			vector<int> gics;
			m_pKeyBind->FindGeneralInputCodesOfAllInputDevices( action_code, i, gics );
			ac_to_gics[action_code] = gics;
		}
	}
}


void SkeletalCharacter::AddOperationsAlgorithm( boost::shared_ptr<SkeletalCharacterOperations> pOperations )
{
	m_pOperations.push_back( pOperations );
	pOperations->m_pSkeletalCharacter
		= boost::dynamic_pointer_cast<SkeletalCharacter,GameItem>( m_pMyself.lock() );
}


void SkeletalCharacter::SetMotionNodeAlgorithm( const std::string& motion_node_name, boost::shared_ptr<CCharacterMotionNodeAlgorithm> pMotionNodeAlgorithm )
{
	if( !m_pLowerLimbsMotionsFSM )
		return;

	shared_ptr<CMotionPrimitiveNode> pNode = m_pLowerLimbsMotionsFSM->GetNode( motion_node_name );
	if( !pNode )
		return;

	pNode->SetAlgorithm( pMotionNodeAlgorithm );

	pMotionNodeAlgorithm->SetSkeletalCharacter( this );

	// Saves the motion node algorithm in a single array.
	m_pMotionNodes.push_back( pMotionNodeAlgorithm );
}


CInputState::Name SkeletalCharacter::GetActionInputState( int action_code, CKeyBind::ActionType action_type )
{
	map< int, vector<int> >& ac_to_gics = m_ACtoGICs.m_mapActionCodeToGICodes[action_type];

	map< int, vector<int> >::iterator itr = ac_to_gics.find( action_code );
	if( itr == ac_to_gics.end() )
		return CInputState::RELEASED;

	for( size_t i=0; i<itr->second.size(); i++ )
	{
		CInputState::Name input_stat
			= GetInputDeviceHub().GetInputDeviceGroup(0)->GetInputState( itr->second[i] ); 

		// Key is considered pressed if any one of the keys for the general input codes are pressed.
		if( input_stat == CInputState::PRESSED )
			return CInputState::PRESSED;
	}

	return CInputState::RELEASED;
}


void SkeletalCharacter::HandleInput( const InputData& input_data )
{
	int action_code = m_pKeyBind ? m_pKeyBind->GetActionCode( input_data.iGICode ) : ACTION_NOT_ASSIGNED;

	for( size_t i=0; i<m_pOperations.size(); i++ )
	{
		if( input_data.iGICode == GIC_MOUSE_BUTTON_L )
			int break_here = 1;

		if( m_pOperations[i] )
			m_pOperations[i]->HandleInput( input_data, action_code );
	}

	// The input handler for the motion FSM is registered as a child
	// of the character's main input handler, and is called after this function.
}


void SkeletalCharacter::OnPhysicsTrigger( physics::CShape& my_shape, CCopyEntity &other_entity, physics::CShape& other_shape, U32 trigger_flags )
{
	physics::CRay ray;

	// Check contact with ground
/*	ray.Origin = my_shape.GetActor().GetWorldPosition() + Vector3(0,-0.5,0);
	ray.Direction = Vector3( 0, -1, 0 );
	float ray_max_dist = 10;
	physics::CRaycastHit hit;
	bool is_hit = other_shape.Raycast( ray, ray_max_dist, 0, hit, false );
//	if( hit.pShape )

	// Check contact with ceiling

	// Check contact with the environment in the direction of the current motion
	Vector3 vCovered = world_pose.vPosition - m_PrevPose.vPosition;
	ray.Origin = m_PrevPose.vPosition;
	ray.Direction = Vec3GetNormalized(vCovered);
	is_hit = other_shape.Raycast( ray, ray_max_dist, 0, hit, false );
*/

}


void SkeletalCharacter::SetCharacterWorldPose( const Matrix34& world_pose, CCopyEntity& entity, physics::CActor &actor )
{
	Matrix34 new_pose( world_pose );

	// Revert to the prev pose.
	entity.SetWorldPose( world_pose );
	UpdatePhysicsActorPose( actor, world_pose );

	// also revert the pose stored in the motion FSM
	shared_ptr<CMotionFSM> pFSM = m_pLowerLimbsMotionsFSM;
	if( !pFSM )
		return;
	Vector3& vFwd = new_pose.matOrient.GetColumn(2);
	Vec3Normalize( vFwd, vFwd );
	new_pose.matOrient.Orthonormalize();
	pFSM->Player()->SetCurrentHorizontalPose( new_pose );
}


static inline bool contains_almost_same_plane( const std::vector<SPlane>& planes, const SPlane& plane )
{
	static const double tolerance = 0.001;
	for( size_t i=0; i<planes.size(); i++ )
	{
		if( fabs( planes[i].dist - plane.dist ) < tolerance
		 && fabs( planes[i].normal.x - plane.normal.x ) < tolerance
		 && fabs( planes[i].normal.y - plane.normal.y ) < tolerance
		 && fabs( planes[i].normal.z - plane.normal.z ) < tolerance )
		{
			return true;
		}
	}

	return false;
}

/*
void SkeletalCharacter::UpdateWhenFeetAreOnGround()
{
}


void SkeletalCharacter::UpdateWhenFeetAreOffGround()
{
}
*/

void SkeletalCharacter::OnPhysicsContact( physics::CContactPair& pair, CCopyEntity& other_entity )
{
	physics::CContactStreamIterator& itr = pair.ContactStreamIterator;

	m_Walls.resize( 0 );

	boost::shared_ptr<CCopyEntity> pEntity = GetItemEntity().Get();
	if( !pEntity )
		return;

	CCopyEntity& entity = *pEntity;

//	SetCharacterWorldPose( m_PrevWorldPose, entity, *pair.pActors[0] );

//	LOG_PRINT( " Contact detected. Reverted the world pos: " + to_string(m_PrevWorldPose.vPosition) );
	//>>---------- revert to the prev pose and return ----------
//	return;
	//<<---------- revert to the prev pose and return ----------

	Vector3 prev_pos    = m_PrevWorldPose.vPosition;
	Vector3 current_pos = entity.GetWorldPose().vPosition;
//	Vector3 prev_to_current = current_pos - prev_pos;

	Vector3 walll_contact_pos_sum = Vector3(0,0,0);
	while( itr.GoNextPair() ) // user can call getNumPairs() here 
	{
		while( itr.GoNextPatch() ) // user can also call getShape() and getNumPatches() here
		{
			bool is_wall = false;
			Vector3 normal = itr.GetPatchNormal();
			if( 0.6f < normal.y )
			{
				// ground or slope
			}
			else if( normal.y < -0.6f )
			{
				// ceiling
			}
			else if( fabs( normal.y ) < 0.25 )
			{
				// wall
				is_wall = true;
			}
			else
			{
				// i.e. 0.25 < normal.y < 0.6
				// somewhat steep slope - for now, treat this as a wall
				// Looks like this also prevents the penerations into thin plates placed horizontally at about 40cm from ground
				is_wall = true;
			}

			while( itr.GoNextPoint() ) // user can also call getPatchNormal() and getNumPoints() here
			{
				Vector3 pos = itr.GetPoint();
				const SPlane plane( normal, Vec3Dot(normal,pos) );

				if( is_wall
				 && !contains_almost_same_plane( m_Walls, plane ) )
				{
					m_Walls.push_back( plane );
					walll_contact_pos_sum += pos;
				}

//				float d0 = plane.GetDistanceFromPoint( prev_pos );
//				float d1 = plane.GetDistanceFromPoint( current_pos );
//				if( d0 < 0 && 0 < d1 || d0 > 0 && 0 > d1 )
//				{
//					entity.SetWorldPose( m_PrevWorldPose );
//				}
			}
		}
	}

	if( m_Walls.empty() )
		return; // No walls to clip the motion against

	Vector3 wall_normal_sum = Vector3(0,0,0);
	int num_wall_planes = (int)m_Walls.size();
	for( int i=0; i<num_wall_planes; i++ )
	{
		wall_normal_sum = m_Walls[i].normal;
	}

//	Vector3 ave_pos    = walll_contact_pos_sum / (float)num_wall_planes;
	Vector3 ave_normal = wall_normal_sum       / (float)num_wall_planes;

	// Push back the character in the direction of the average normal of contacted walls.

	if( 1.0f - 0.01f < fabs(ave_normal.y) )
		return;
/*
	//>>> 1. Simply push the character away from the wall (suffers oscillations)
	ave_normal.y = 0;
	Vec3Normalize( ave_normal, ave_normal );
	Matrix34 pose_to_revert_to( m_PrevWorldPose );
//	Matrix34 pose_to_revert_to( entity.GetWorldPose() );
	pose_to_revert_to.vPosition += ave_normal * 0.01f;
	//<<< 1.
*/
	ClipMotion( entity );
}


void SkeletalCharacter::ClipMotion( CCopyEntity& entity )
{
	//>>> 2. Clip the motion along the wall
	const Matrix34 current_world_pose( entity.GetWorldPose() );
	Matrix34 pose_to_revert_to( entity.GetWorldPose() );
	float cap_radius = 0.25f;
	for( int i=0; i<(int)m_Walls.size(); i++ )
	{
		const SPlane& collision_plane = m_Walls[i];
		const float dist_from_collision_plane = collision_plane.GetDistanceFromPoint( current_world_pose.vPosition );
		if( cap_radius < dist_from_collision_plane )
			continue; // enough distance from the collision wall?

		const float margin = 0.01f;
		const float dist_to_push = cap_radius - dist_from_collision_plane + margin;
		pose_to_revert_to.vPosition += collision_plane.normal * dist_to_push;

		// This alone does not prevent the character walking up a steep slope
//		pose_to_revert_to.vPosition += GetHorizontalized( collision_plane.normal ) * dist_to_push;
	}
	//>>> 2.


//	SetCharacterWorldPose( pose_to_revert_to, entity, *pair.pActors[0] );

	if( 0 < entity.m_vecpPhysicsActor.size()
		 && entity.m_vecpPhysicsActor[0] )
	{
		SetCharacterWorldPose( pose_to_revert_to, entity, *(entity.m_vecpPhysicsActor[0]) );
	}
}


void SkeletalCharacter::UpdateGraphics()
{
	if( !m_pMotionFSMManager )
		return;

	boost::shared_ptr<SkeletalMesh> pSkeletalMesh = GetCharacterSkeletalMesh();
	if( !pSkeletalMesh )
		return;

	// Update the hierarchical transforms for the current pose of the character
	CKeyframe& current_keyframe = m_CurrentInterpolatedKeyframe;
	m_pMotionFSMManager->GetCurrentKeyframe( current_keyframe );

	shared_ptr<ItemEntity> pItemEntity = GetItemEntity().Get();
	if( pItemEntity
	 && pItemEntity->MeshBonesUpdateCallback() )
	{
		vector<Transform>& mesh_bone_local_transforms
			= pItemEntity->MeshBonesUpdateCallback()->MeshBoneLocalTransforms();

		mesh_bone_local_transforms.resize( pSkeletalMesh->GetNumBones() );
		const int num_transforms = (int)mesh_bone_local_transforms.size();
		for( int i=0; i<num_transforms; i++ )
			mesh_bone_local_transforms[i].SetIdentity();

		// Get the mesh bone local transforms and store them to mesh_bone_local_transforms.
//		UpdateMeshBoneTransforms( current_keyframe, m_RootTransformNodeMap, mesh_bone_local_transforms );
		UpdateMeshBoneTransforms( current_keyframe, *(m_pSkeletonSrcMotion->GetSkeleton()), *pSkeletalMesh, mesh_bone_local_transforms );

		pItemEntity->MeshBonesUpdateCallback()->UpdateGraphics();

//		pItemEntity->MeshBonesUpdateCallback()->SetSkeletalMesh( pContainer->m_MeshObjectHandle );
	}
}


Result::Name SkeletalCharacter::LoadCharacterMesh( const std::string& skeletal_mesh_pathname )
{
	shared_ptr<MeshObjectContainer> pContainer( new MeshObjectContainer );
	pContainer->m_MeshDesc.ResourcePath = skeletal_mesh_pathname;
	pContainer->m_MeshDesc.MeshType = MeshType::SKELETAL;
//	pContainer->m_MeshDesc.pMeshGenerator.reset( new CBoxMeshGenerator() );
	m_MeshContainerRootNode.SetMeshContainer( 0, pContainer );

	bool mesh_laoded = m_MeshContainerRootNode.LoadMeshesFromDesc();

	// Update mappings of skeleton bones and mesh bones
	shared_ptr<SkeletalMesh> pSkeletalMesh = GetCharacterSkeletalMesh();
	if( pSkeletalMesh
	 && m_pSkeletonSrcMotion
	 && m_pSkeletonSrcMotion->GetSkeleton() )
	{
		CreateTransformMapTree( *(m_pSkeletonSrcMotion->GetSkeleton()), m_RootTransformNodeMap, *pSkeletalMesh );
	}
	else
		LOG_PRINT_WARNING( " Unable to set up the mappings of motion data bones to mesh bones." );

	return mesh_laoded ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


/// 1. Set velocity for the entity and the physics actor.
/// 2. Set m_FeetOnGround to false
void SkeletalCharacter::StartVerticalJump( const Vector3& velocity )
{
	if( !m_FeetOnGround )
		return; // Can't jump while in the air

	if( velocity.y < 0.001f )
		return; // Velocity is too small to jump up

	shared_ptr<CCopyEntity> pEntity = GetItemEntity().Get();
	if( !pEntity )
		return;

	CCopyEntity& entity = *pEntity;

	entity.SetVelocity( velocity );

	if( 0 < entity.m_vecpPhysicsActor.size()
	 && entity.m_vecpPhysicsActor[0] )
	{
		entity.m_vecpPhysicsActor[0]->SetLinearVelocity( velocity );
	}

	m_FeetOnGround = false;
}


/// \param turn_speed radians per second
void SkeletalCharacter::TurnIfNecessary( float dt, float turn_speed )
{
	if( !IsCameraDependentMotionControlEnabled() )
		return;

	if( m_vDesiredHorizontalDirection.GetLength() < 0.1f )
		return;

	// Do not check m_FeetOnGround
	// - The character's motion FSM nodes decide whether the character should turn or not.

	shared_ptr<CCopyEntity> pEntity = m_Entity.Get();
	if( !pEntity )
		return;

	CCopyEntity& entity = *pEntity;

	Matrix33 world_orient = entity.GetWorldPose().matOrient;

	const Matrix33 horizontal_world_orient = GetHorizontalized( world_orient );

	Vector3 vCurrentHorizontalDirection = horizontal_world_orient.GetColumn(2);

	Vec3Dot( vCurrentHorizontalDirection, m_vDesiredHorizontalDirection );
	float angles_to_cover = Vec3GetAngleBetween( vCurrentHorizontalDirection, m_vDesiredHorizontalDirection );

	float max_coverable_angles = dt * turn_speed;

	float angles = take_min( max_coverable_angles, angles_to_cover );

	Vector3 vCross = Vec3Cross( vCurrentHorizontalDirection, m_vDesiredHorizontalDirection );
	if( vCross.y < 0.0f )
		angles *= -1.0f;

	world_orient = Matrix33RotationY( angles ) * horizontal_world_orient;

	entity.SetWorldOrientation( world_orient );

//	if( 0 < entity.m_vecpPhysicsActor.size()
//	 && entity.m_vecpPhysicsActor[0] )
//	{
//		UpdatePhysicsActorPose( *(entity.m_vecpPhysicsActor[0]), entity.GetWorldPose() );
//	}

//	UpdatePoseStoredInMotionPrimitivePlayer( m_pMotionFSMManager, entity.GetWorldPose() );
}


float SkeletalCharacter::GetFloorHeight( CCopyEntity& entity, Vector3& impact_normal )
{
	physics::CScene *pPhysScene = entity.GetStage()->GetPhysicsScene();
	if( !pPhysScene )
		return 0;

	OBB3 obb( entity.GetWorldPose(), Vector3(1,1,1) * 0.05f );
//	obb.center.vPosition += entity.GetWorldPose().matOrient.GetColumn(2) * 2.0f;
	obb.center.vPosition += Vector3(0,1,0);
	Vector3 sweep = Vector3( 0, -50, 0 );
	physics::CSweepQueryHit query;
	void *pUserData = NULL;
//	U32 sweep_flags = physics::SweepFlag::ALL_HITS;
	U32 sweep_flags = physics::SweepFlag::STATICS | physics::SweepFlag::DYNAMICS;
//	physics::CGroupsMask mask;
	U32 active_groups = 0xFFFFFFFF;
	pPhysScene->LinearOBBSweep(
		obb,
		sweep,
		sweep_flags, 
		pUserData,
		1, // num max shapes
		query,
		NULL,
		active_groups
//		&mask
		);

//	if( abs(1.0f - query.t) < 0.001f )

//	return query.Point.y;

	if( 0.5f < fabs(query.Normal.y) )
	{
		return query.Point.y;
	}
	else
	{
		// Too steep for floor
		return m_fFloorHeight;
	}
}


void SkeletalCharacter::UpdateStepHeight( CCopyEntity& entity )
{
	const float max_step_height = 0.3f;

//	bool m_FeetOnGround = true;

	Vector3 vImpactNormal = Vector3(0,1,0);
	const float prev_floor_height = m_fFloorHeight;
	const float floor_height = GetFloorHeight( entity, vImpactNormal );
//	m_fFloorHeight = floor_height;

	if( m_FeetOnGround )
	{
		if( prev_floor_height < floor_height )
		{
			// There is an obstacle before the character,
			// or the character is walking up a sloped floor
			if( floor_height - prev_floor_height <= max_step_height )
			{
				// step up
				m_fFloorHeight = floor_height;
				return;
			}
			else
			{
				int cannot_simply_step_up_this_height = 1;
			}
		}
		else
		{
			// floor height is lower than that of the previous frame
			// - Adopt this as a new floor height
			m_fFloorHeight = floor_height;

			if( prev_floor_height - floor_height <= max_step_height )
				return; // step down
//				m_fFloorHeight = floor_height; // step down
			else
			{
				// fall
				m_FeetOnGround = false;
				entity.SetVelocity( Vector3(0,0,0) );
				m_pMotionFSMManager->GetMotionFSM("lower_limbs")->RequestTransition( "falling" );
			}
		}
	}
	else
	{
		// The character is currenly not on ground
		// - Update the floor height anyway
		m_fFloorHeight = floor_height;

		Matrix34 world_pose = entity.GetWorldPose();
//		if( world_pose.vPosition.y < query.Point.y )
		if( world_pose.vPosition.y <= floor_height + 0.001f // feet almost on the ground (dist from feet to the ground <= 0.001)?
		 && entity.Velocity().y < 0.005f ) // not jumping up right now
		{
			// landed
			// - Request transition to landing motion
			m_FeetOnGround = true;
			world_pose.vPosition.y = floor_height;
			m_pMotionFSMManager->GetMotionFSM("lower_limbs")->RequestTransition( "standing" );
//			entity.SetWorldPose( world_pose );
//			SetCharacterWorldPose( world_pose, entity, *(entity.m_vecpPhysicsActor[0]) );
		}
	}

/*	int num_motion_fsms = (int)m_pMotionFSMManager->GetMotionFSMs.size();
	for( int i=0; i<num_motion_fsms; i++ )
	{
		shared_ptr<CMotionFSM>& pFSM = m_pMotionFSMManager->MotionFSMs()[i];
		if( !pFSM )
			continue;

		pFSM->Player()->SetFloorHeight( floor_height );
	}*/
}


bool CCharacterMotionNodeAlgorithm::HandleInputForTransition( const InputData& input, int action_code )
{
	map<pair<int,int>,string>::iterator itr = m_ActionInputsToMotionNodes.find( pair<int,int>(action_code,input.iType) );
	if( itr != m_ActionInputsToMotionNodes.end() )
	{
		RequestTransition( itr->second );
		return true;
	}
	else
		return false;
}


CInputState::Name CCharacterMotionNodeAlgorithm::GetActionInputState( int action_code )
{
	return m_pCharacter->GetActionInputState( action_code );
}


bool CCharacterMotionNodeAlgorithm::HandleInput( const InputData& input, int action_code )
{
	bool res = HandleInputForTransition( input, action_code );
	if( res )
		return true;

	if( m_pCharacter && m_pCharacter->IsCameraDependentMotionControlEnabled() )
		return false;

	switch( action_code )
	{
	case ACTION_MOV_FORWARD:
		if( input.iType == ITYPE_KEY_PRESSED
		 || input.iType == ITYPE_VALUE_CHANGED )
		{
			float fwd_speed = 0;
			if( input.IsKeyboardInput() )
			{
				fwd_speed = (GetActionInputState(ACTION_MOV_BOOST) == CInputState::PRESSED) ? 1.0f : 0.5f;
			}
			else // analog input
				fwd_speed = input.fParam1;

			m_pCharacter->SetFwdSpeed( fwd_speed );
		}
/*		if( input.iType == ITYPE_KEY_PRESSED )
			m_pCharacter->SetFwdSpeed(  input.fParam1 );
		else if( input.iType == ITYPE_VALUE_CHANGED )
			m_pCharacter->SetFwdSpeed(  input.fParam1 );*/
		else
			m_pCharacter->SetFwdSpeed( 0 );
		break;
	case ACTION_MOV_BACKWARD:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_pCharacter->SetFwdSpeed( -input.fParam1 );
//			m_fFwdSpeed = -input.fParam1;
//			m_pLowerLimbsMotionsFSM->RequestTransition( "bwd" );
		}
		break;
	case ACTION_MOV_BOOST: // Keyboard only. Analog gamepad has analog stick to control walk/run
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			float fwd_speed = 0;
			CInputState::Name fwd_input = GetActionInputState( ACTION_MOV_FORWARD );
			if( fwd_input == CInputState::PRESSED )
				fwd_speed = 1.0f;
//			else
//				fwd_speed = 0.5f;

			m_pCharacter->SetFwdSpeed( fwd_speed );
		}
		else if( input.iType == ITYPE_KEY_RELEASED )
		{
			float fwd_speed = 0;
			CInputState::Name fwd_input = GetActionInputState( ACTION_MOV_FORWARD );
			if( fwd_input == CInputState::PRESSED )
				fwd_speed = 0.5f;
			else
				fwd_speed = 0.0f;

			m_pCharacter->SetFwdSpeed( fwd_speed );
		}
		break;
	case ACTION_MOV_TURN_R:
		if( input.iType == ITYPE_KEY_PRESSED
		 || input.iType == ITYPE_VALUE_CHANGED )
		{
			m_pCharacter->SetTurnSpeed(  input.fParam1 );
//			m_fTurnSpeed =  input.fParam1;
		}
		else
			m_pCharacter->SetTurnSpeed( 0 );
		break;
	case ACTION_MOV_TURN_L:
		if( input.iType == ITYPE_KEY_PRESSED
		 || input.iType == ITYPE_VALUE_CHANGED )
		{
			m_pCharacter->SetTurnSpeed( -input.fParam1 );
//			m_fTurnSpeed = -input.fParam1;
		}
		else
			m_pCharacter->SetTurnSpeed( 0 );
		break;
	default:
		break;
	}

	return false;
}


void CCharacterMotionNodeAlgorithm::AddActionCodeToMotionNodeMap( int action_code, int input_type, const std::string& motion_node_name )
{
	m_ActionInputsToMotionNodes[ pair<int,int>(action_code,input_type) ] = motion_node_name;
}


void CFwdMotionNode::Update( float dt )
{
	float fFwdSpeed = m_pCharacter->GetFwdSpeed();
	if( fFwdSpeed < 0.2f )
	{
		RequestTransition( "standing" );
	}
	else if( 0.55f <= fFwdSpeed )
	{
		RequestTransition( "run" );
	}

	m_pNode->SetExtraSpeedFactor( 0.5f );

	m_pCharacter->SetMaxTurnSpeed( 2.5f );
//	m_pCharacter->TurnIfNecessary( dt, 3.0f );
}


bool CFwdMotionNode::HandleInput( const InputData& input, int action_code )
{
	CCharacterMotionNodeAlgorithm::HandleInput( input, action_code );

	switch( action_code )
	{
	case ACTION_MOV_FORWARD:
		if( input.iType == ITYPE_KEY_PRESSED
		 || input.iType == ITYPE_VALUE_CHANGED )
		{
			m_pNode->SetExtraSpeedFactor( m_pCharacter->GetFwdSpeed() * 0.5f );
		}
		break;
	case ACTION_MOV_BOOST:
		if( input.iType == ITYPE_KEY_PRESSED )
			RequestTransition( "run" );
		break;
	case ACTION_MOV_JUMP:
		RequestTransition( "vertical_jump" );
		break;
/*	case ACTION_MOV_TURN_L:
		break;
	case ACTION_MOV_TURN_R:
		break;*/
	default:
		break;
	}

	return false;
}


void CFwdMotionNode::EnterState()
{
	// update the forward speed
	m_pNode->SetExtraSpeedFactor( m_pCharacter->GetFwdSpeed() );
}


void CRunMotionNode::Update( float dt )
{
	float fFwdSpeed = m_pCharacter->GetFwdSpeed();
	if( fFwdSpeed < 0.2f )
	{
		RequestTransition( "standing" );
	}
	else if( fFwdSpeed < 0.6f )
	{
		RequestTransition( "fwd" ); // walk
	}

	m_pNode->SetExtraSpeedFactor( fFwdSpeed );
}


bool CRunMotionNode::HandleInput( const InputData& input, int action_code )
{
	CCharacterMotionNodeAlgorithm::HandleInput( input, action_code );

	switch( action_code )
	{
	case ACTION_MOV_FORWARD:
		if( input.iType == ITYPE_KEY_PRESSED
		 || input.iType == ITYPE_VALUE_CHANGED )
		{
			m_pNode->SetExtraSpeedFactor( input.fParam1 * 0.5f );
		}
		break;
//	case ACTION_MOV_BOOST:
//		if( input.iType == ITYPE_KEY_RELEASED )
//			RequestTransition( "fwd" );
//		break;
	case ACTION_MOV_JUMP:
		RequestTransition( "jump" );
		break;
/*	case ACTION_MOV_TURN_L:
		break;
	case ACTION_MOV_TURN_R:
		break;*/
	default:
		break;
	}

	return false;
}


void CRunMotionNode::EnterState()
{
	// update the forward speed
	m_pNode->SetExtraSpeedFactor( m_pCharacter->GetFwdSpeed() );
}


shared_ptr<ItemEntity> CJumpMotionNode::GetCharacterEntity()
{
	return m_pCharacter ? (m_pCharacter->GetItemEntity().Get()) : shared_ptr<ItemEntity>();
}


void CJumpMotionNode::Update( float dt )
{
	shared_ptr<CCopyEntity> pEntity( GetCharacterEntity() );
	if( !pEntity )
		return;

	CCopyEntity& entity = *pEntity;

	if( entity.Velocity().y <= 0 )
		RequestTransition( "falling" );
}


void CJumpMotionNode::EnterState()
{
	m_pCharacter->StartVerticalJump( Vector3(0,5,0) );
}


bool CJumpMotionNode::HandleInput( const InputData& input, int action_code )
{
	return false;
}



void CStandingMotionNode::Update( float dt )
{
	float fFwdSpeed = m_pCharacter->GetFwdSpeed();
	if( 0.2f < fFwdSpeed )
	{
		RequestTransition( "fwd" );
	}

	m_pCharacter->SetMaxTurnSpeed( 45.0f );
//	m_pCharacter->TurnIfNecessary( dt, 50.0f );
}


bool CStandingMotionNode::HandleInput( const InputData& input, int action_code )
{
	CCharacterMotionNodeAlgorithm::HandleInput( input, action_code );

	switch( action_code )
	{
	case ACTION_MOV_FORWARD:
		if( input.iType == ITYPE_KEY_PRESSED
		 && 0.2f < input.fParam1 )
		{
			RequestTransition( "fwd" );
		}
		else if( input.iType == ITYPE_VALUE_CHANGED
		 && 0.2f < input.fParam1 )
		{
			RequestTransition( "fwd" );
		}
		break;
	case ACTION_MOV_JUMP:
		RequestTransition( "vertical_jump" );
		break;
/*	case ACTION_MOV_TURN_L:
		break;
	case ACTION_MOV_TURN_R:
		break;*/
	default:
		break;
	}

	return false;
}


} // namespace amorphous
