#ifndef __PhysicsShapeMakerVisitor_HPP__
#define __PhysicsShapeMakerVisitor_HPP__


#include "../../Graphics/ShapeContainers.hpp"
#include "../../Graphics/3DGameMath.hpp"
#include "../../Physics/BoxShapeDesc.hpp"
#include "../../Physics/SphereShapeDesc.hpp"
#include "../../Physics/CapsuleShapeDesc.hpp"
#include "../../Support/Log/DefaultLog.hpp"
#include "../../Support/SafeDeleteVector.hpp"


class CPhysicsShapeMakerVisitor : public CShapeContainerVisitor
{
public:
	std::vector<physics::CShapeDesc *>& m_pShapeDescs;

	CPhysicsShapeMakerVisitor( std::vector<physics::CShapeDesc *>& pShapeDescs )
		:
	m_pShapeDescs(pShapeDescs)
	{}

	void VisitAABB3Container( CAABB3Container& aabb3_container )
	{
		using namespace physics;

		CBoxShapeDesc *pBox = new CBoxShapeDesc;
		pBox->vSideLength         = (aabb3_container.aabb.vMax - aabb3_container.aabb.vMin) * 0.5f;
		pBox->LocalPose.vPosition = aabb3_container.aabb.GetCenterPosition();
		pBox->LocalPose.matOrient = Matrix33Identity();

		m_pShapeDescs.push_back( pBox );
	}

	void VisitOBB3Container( COBB3Container& obb3_container )
	{
		using namespace physics;

		CBoxShapeDesc *pBox = new CBoxShapeDesc;
		pBox->vSideLength = obb3_container.obb.radii;
		pBox->LocalPose   = obb3_container.obb.center;

		m_pShapeDescs.push_back( pBox );
	}

	void VisitSphereContainer( CSphereContainer& sphere_container )
	{
		using namespace physics;

		CSphereShapeDesc *pSphere = new CSphereShapeDesc;
		pSphere->Radius = sphere_container.sphere.radius;
		pSphere->LocalPose.vPosition = sphere_container.sphere.center;
		pSphere->LocalPose.matOrient = Matrix33Identity();

		m_pShapeDescs.push_back( pSphere );
	}

	void VisitCapsuleContainer( CCapsuleContainer& capsule_container )
	{
		using namespace physics;

		CCapsuleShapeDesc *pCap = new CCapsuleShapeDesc;
		pCap->fRadius = capsule_container.capsule.radius;
		pCap->fLength = Vec3Length(capsule_container.capsule.p1 - capsule_container.capsule.p0);
		pCap->LocalPose.vPosition = (capsule_container.capsule.p1 + capsule_container.capsule.p0) * 0.5f;
		pCap->LocalPose.matOrient = CreateOrientFromFwdDir( Vec3GetNormalized(capsule_container.capsule.p1 - capsule_container.capsule.p0) );

		m_pShapeDescs.push_back( pCap );
	}

	void VisitConvexContainer( CConvexContainer& convex_container )
	{ 
		LOG_PRINT_ERROR( " Not implemented yet." );
//		m_pShapeDescs.push_back( pConvex );
	}
};



#endif /* __PhysicsShapeMakerVisitor_HPP__ */
