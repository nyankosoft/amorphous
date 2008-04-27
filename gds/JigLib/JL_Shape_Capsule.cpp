
#include "JL_Shape_Capsule.h"

#include "JL_ShapeDesc_Capsule.h"
#include "JL_PhysicsActor.h"



CJL_Shape_Capsule::CJL_Shape_Capsule()
{
	m_fLength = 0;
	m_fRadius = 0;
}


void CJL_Shape_Capsule::InitSpecific( CJL_ShapeDesc& rShapeDesc )
{
	CJL_ShapeDesc_Capsule *pCapsuleDesc = (CJL_ShapeDesc_Capsule *)&rShapeDesc;

	m_fLength = pCapsuleDesc->fLength;
	m_fRadius = pCapsuleDesc->fRadius;
}


void CJL_Shape_Capsule::UpdateWorldProperties()
{
	// update world pose
	m_WorldPose.vPosition = m_pPhysicsActor->GetOrientation() * m_LocalPose.vPosition + m_pPhysicsActor->GetPosition();
	m_WorldPose.matOrient = m_pPhysicsActor->GetOrientation() * m_LocalPose.matOrient;

	// update world aabb
	m_WorldAABB.Nullify();

	Vector3 v;
	v = GetWorldPosition() + GetWorldOrient().GetColumn(2) * m_fLength * 0.5f;
	m_WorldAABB.AddPoint( v );

	v = GetWorldPosition() - GetWorldOrient().GetColumn(2) * m_fLength * 0.5f;
	m_WorldAABB.AddPoint( v );

	Scalar r = m_fRadius;
	m_WorldAABB.vMin -= Vector3(r,r,r);
	m_WorldAABB.vMax += Vector3(r,r,r);
}