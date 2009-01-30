#include "BaseEntity.hpp"
#include "CopyEntity.hpp"
#include "Stage.hpp"
#include "EntityMotionPathRequest.hpp"
#include "Support/Log/DefaultLog.hpp"

#include "Physics/Actor.hpp"

using namespace physics;


void CBaseEntity::UpdateScriptedMotionPath( CCopyEntity* pCopyEnt, CBEC_MotionPath& path )
{
	const float time_in_stage = (float)m_pStage->GetElapsedTime();

	if( path.IsAvailable(time_in_stage) )
	{
		if( 9.0f < time_in_stage )
			int do_break = 1;

		// follow the scripted path
		Matrix34 world_pose = path.GetPose( time_in_stage );
		pCopyEnt->SetWorldPose( world_pose );
		if( pCopyEnt->pPhysicsActor )
			pCopyEnt->pPhysicsActor->SetWorldPose( world_pose );

		// calc velocity - used when the entity is destroyed
		// and the vel of the frags have to be calculated
		float dt = m_pStage->GetFrameTime();
		pCopyEnt->Velocity() = ( pCopyEnt->Position() - pCopyEnt->vPrevPosition ) / dt;
	}
	else
	{
		const vector<KeyPose>& vecKeyPose = path.GetKeyPose();
		if( 0 < vecKeyPose.size() && vecKeyPose.back().time < time_in_stage )
            path.ReleaseMotionPath();	// done with the scripted motion
	}
}
