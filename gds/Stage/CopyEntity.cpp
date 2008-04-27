
#include "CopyEntity.h"
#include "BaseEntity.h"
//#include "EntitySet.h"

#include "JigLib/JL_PhysicsActor.h"


void CCopyEntity::ApplyWorldImpulse( Vector3& vImpulse, Vector3& vContactPoint )
{
	if( !pPhysicsActor )
		return;	// entity must have a corresponding physics actor to apply impulse

	pPhysicsActor->ApplyWorldImpulse( vImpulse, vContactPoint );
}


// called during the termination if a copy entity has any parent or children
void CCopyEntity::DisconnectFromParentAndChildren()
{
	// disconnect from children
	int i;
	for( i=0; i<iNumChildren; i++ )
	{
		apChild[i]->pParent = NULL;
		apChild[i] = NULL;
	}
	iNumChildren = 0;

	// disconnect from parent
	if( GetParent() )
	{	// search myself in the parent's list of children
		for( i=0; i< pParent->iNumChildren; i++ )
		{
			if( pParent->apChild[i] == this )
			{
				pParent->apChild[i] = NULL;
				int j;
				// push the parent's children forward to fill the vacant space in the array
				for( j=i; j<pParent->iNumChildren-1; j++ )
					pParent->apChild[j] = pParent->apChild[j+1];
				pParent->iNumChildren--;
			}
		}
	}
	pParent = NULL;
}


void CCopyEntity::ReleasePhysicsActor()
{
	pPhysicsActor = NULL;
}


void CCopyEntity::UpdatePhysics()
{
	pPhysicsActor->GetWorldPose( WorldPose );

	int i;
	for( i=0; i<9; i++ )
	{
		if( WorldPose.matOrient.GetData()[i] != 0.0f )
			break;
	}
	if( i == 9 )
		int error = 1;

//	Position() = pPhysicsActor->GetPosition();
	Velocity() = pPhysicsActor->GetVelocity();
	AngularVelocity() = pPhysicsActor->GetAngularVelocity();
}
