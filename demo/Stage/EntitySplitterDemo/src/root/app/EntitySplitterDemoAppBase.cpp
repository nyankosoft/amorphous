#include "EntitySplitterDemoAppBase.hpp"

#include "amorphous/Graphics/GraphicsElementManager.hpp"
#include "amorphous/Graphics/MeshUtilities.hpp"
#include "amorphous/Input.hpp"
#include "amorphous/Stage.hpp"
#include "amorphous/Stage/Trace.hpp"
#include "amorphous/Stage/EntitySplitter.hpp"
#include "amorphous/Stage/StageUtility.hpp"
#include "amorphous/Physics/Actor.hpp"
#include "amorphous/Task.hpp"
#include "amorphous/Script.hpp"


using std::string;
using std::vector;
using boost::shared_ptr;

using namespace amorphous::physics;


static string sg_TestStageScriptToLoad = "./Script/default.bin";


extern ApplicationBase *amorphous::CreateApplicationInstance() { return new EntitySplitterDemoAppBase(); }


EntitySplitterDemoAppTask::EntitySplitterDemoAppTask()
{
	ScriptManager::ms_UseBoostPythonModules = true;

	StageLoader stg_loader;
	m_pStage = stg_loader.LoadStage( sg_TestStageScriptToLoad );

	GetCameraController()->SetPose( Matrix34( Vector3(0,20,-15), Matrix33Identity() ) );

	Matrix34 pose = GetCamera().GetPose();
	pose.vPosition += pose.matOrient.GetColumn(2) * 3.5f;

	MeshHandle mesh = CreateBoxMesh( Vector3(3.00f,0.05f,3.00f), SFloatRGBAColor(1.0f,1.0f,1.0f,0.6f) );

	StageMiscUtility stg_util( m_pStage );
	m_SplitPlaneEntity = stg_util.CreateNonCollidableEntityFromMesh( mesh, pose, "split_plane" );

	shared_ptr<CCopyEntity> pSplitPlaneEntity = m_SplitPlaneEntity.Get();
	if( pSplitPlaneEntity )
	{
		pSplitPlaneEntity->RaiseEntityFlags( BETYPE_USE_ZSORT );
	}
}


void EntitySplitterDemoAppTask::OnTriggerPulled()
{
	if( !m_pStage )
		return;

	shared_ptr<CCopyEntity> pSplitPlaneEntity = m_SplitPlaneEntity.Get();
	if( !pSplitPlaneEntity )
		return;

	Matrix34 world_pose = pSplitPlaneEntity->GetWorldPose();

	

	vector<CCopyEntity *> pEntities;
	OverlapTestAABB aabb_test( AABB3(world_pose.vPosition + Vector3(-1,-1,-1), world_pose.vPosition + Vector3(1,1,1)), &pEntities );

	m_pStage->GetEntitySet()->GetOverlappingEntities( aabb_test );

	Vector3 normal = pSplitPlaneEntity->GetWorldPose().matOrient.GetColumn(1);
	Plane split_plane( normal, Vec3Dot( normal, pSplitPlaneEntity->GetWorldPosition() ) );

	LOG_PRINTF(( "%d overlapping entities", (int)pEntities.size() ));

	int num_entities_to_split = take_min( (int)pEntities.size(), 16 );

	for( int i=0; i<num_entities_to_split; i++ )
	{
		if( !pEntities[i] )
			continue;

		// Skip the skybox
		if( !(pEntities[i]->GetEntityFlags() & BETYPE_RIGIDBODY) )
			continue;

		if( pEntities[i]->bNoClip )
			continue;

		// Skip the floor
		CActor *pActor = pEntities[i]->GetPrimaryPhysicsActor();
		if( pActor && pActor->GetBodyFlags() & (PhysBodyFlag::Kinematic | PhysBodyFlag::Static) )
			continue;

		EntityHandle<> entity( pEntities[i]->Self() );

		EntitySplitter entity_splitter;
		EntityHandle<> dest0, dest1;
		entity_splitter.Split( entity, split_plane, EntitySplitterParams(), dest0, dest1 );

		for( int j=0; j<2; j++ )
		{
			shared_ptr<CCopyEntity> pDest = (j==0) ? dest0.Get() : dest1.Get();
			if( !pDest )
				continue;

			if( pDest->m_vecpPhysicsActor.empty() || !pDest->m_vecpPhysicsActor[0] )
				continue;
			{
				Vector3 impulse = (j==0) ? split_plane.normal : -split_plane.normal;

				Vector3 point = pDest->GetWorldPosition();

				pDest->ApplyWorldImpulse( impulse, point );
			}
		}
	}
}


int EntitySplitterDemoAppTask::FrameMove( float dt )
{
	int ret = StageViewerGameTask::FrameMove( dt );
	if( ret != ID_INVALID )
		return ret;

	shared_ptr<CCopyEntity> pSplitPlaneEntity = m_SplitPlaneEntity.Get();
	if( pSplitPlaneEntity )
	{
		Matrix34 pose = GetCamera().GetPose();
		pose.vPosition += pose.matOrient.GetColumn(2) * 3.5f;
		pose = pose * Matrix34( Vector3(0.0f,-0.5f,0.0f), Matrix33Identity() );

		pSplitPlaneEntity->SetWorldPose( pose );
	}

	return ID_INVALID;
}


void EntitySplitterDemoAppTask::HandleInput( const InputData& input )
{
	switch( input.iGICode )
    {
	case GIC_MOUSE_BUTTON_L:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			OnTriggerPulled();
		}
		break;
	default:
		break;
	}
}


//========================================================================================
// EntitySplitterDemoAppBase
//========================================================================================

EntitySplitterDemoAppBase::EntitySplitterDemoAppBase()
{
}


EntitySplitterDemoAppBase::~EntitySplitterDemoAppBase()
{
}


int EntitySplitterDemoAppBase::GetStartTaskID() const
{
	return GAMETASK_ID_ENTITY_SPLITTER_APP;
}
