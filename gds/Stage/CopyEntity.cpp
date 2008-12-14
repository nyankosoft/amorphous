#include "CopyEntity.h"
#include "BaseEntity.h"
//#include "EntitySet.h"

//#include "JigLib/JL_PhysicsActor.h"
#include "Physics/Actor.h"
using namespace physics;

using namespace std;
using namespace boost;


void CCopyEntity::ApplyWorldImpulse( Vector3& vImpulse, Vector3& vContactPoint )
{
	if( !pPhysicsActor )
		return;	// entity must have a corresponding physics actor to apply impulse

//	pPhysicsActor->ApplyWorldImpulse( vImpulse, vContactPoint );
	pPhysicsActor->AddWorldForceAtWorldPos( vImpulse, vContactPoint, ForceMode::Impulse );
}


// called during the termination if a copy entity has any parent or children
void CCopyEntity::DisconnectFromParentAndChildren()
{
	// disconnect from children
	int i;
	for( i=0; i<iNumChildren; i++ )
	{
		shared_ptr<CCopyEntity> pChild = m_aChild[i].Get();
		if( pChild )
		{
			pChild->m_pParent = NULL;
			m_aChild[i].Reset();
		}
//		m_aChild[i] = NULL;
	}
	iNumChildren = 0;

	// disconnect from parent
	if( GetParent() )
	{
		// search myself in the parent's list of children
		for( i=0; i< m_pParent->iNumChildren; i++ )
		{
			if( m_pParent->m_aChild[i].GetRawPtr() == this )
			{
				m_pParent->m_aChild[i].Reset();
				int j;
				// push the parent's children forward to fill the vacant space in the array
				for( j=i; j<m_pParent->iNumChildren-1; j++ )
					m_pParent->m_aChild[j] = m_pParent->m_aChild[j+1];
				m_pParent->iNumChildren--;
			}
		}
	}
	m_pParent = NULL;
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
	Velocity() = pPhysicsActor->GetLinearVelocity();
	AngularVelocity() = pPhysicsActor->GetAngularVelocity();
}
