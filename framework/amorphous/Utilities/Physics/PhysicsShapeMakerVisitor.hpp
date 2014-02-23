#ifndef __PhysicsShapeMakerVisitor_HPP__
#define __PhysicsShapeMakerVisitor_HPP__


#include "../../Graphics/ShapeContainers.hpp"
#include "../../3DMath/3DGameMath.hpp"
#include "../../Physics/BoxShapeDesc.hpp"
#include "../../Physics/SphereShapeDesc.hpp"
#include "../../Physics/CapsuleShapeDesc.hpp"
#include "../../Physics/MeshConvenienceFunctions.hpp"
#include "../../Support/Log/DefaultLog.hpp"
#include "../../Support/SafeDeleteVector.hpp"


namespace amorphous
{


class PhysicsShapeMakerVisitor : public ShapeContainerVisitor
{
public:
	std::vector<physics::CShapeDesc *>& m_pShapeDescs;

	PhysicsShapeMakerVisitor( std::vector<physics::CShapeDesc *>& pShapeDescs )
		:
	m_pShapeDescs(pShapeDescs)
	{}

	void VisitAABB3Container( AABB3Container& aabb3_container )
	{
		using namespace physics;

		CBoxShapeDesc *pBox = new CBoxShapeDesc;
		pBox->vSideLength         = (aabb3_container.aabb.vMax - aabb3_container.aabb.vMin) * 0.5f;
		pBox->LocalPose.vPosition = aabb3_container.aabb.GetCenterPosition();
		pBox->LocalPose.matOrient = Matrix33Identity();

		m_pShapeDescs.push_back( pBox );
	}

	void VisitOBB3Container( OBB3Container& obb3_container )
	{
		using namespace physics;

		CBoxShapeDesc *pBox = new CBoxShapeDesc;
		pBox->vSideLength = obb3_container.obb.radii;
		pBox->LocalPose   = obb3_container.obb.center;

		m_pShapeDescs.push_back( pBox );
	}

	void VisitSphereContainer( SphereContainer& sphere_container )
	{
		using namespace physics;

		CSphereShapeDesc *pSphere = new CSphereShapeDesc;
		pSphere->Radius = sphere_container.sphere.radius;
		pSphere->LocalPose.vPosition = sphere_container.sphere.center;
		pSphere->LocalPose.matOrient = Matrix33Identity();

		m_pShapeDescs.push_back( pSphere );
	}

	void VisitCapsuleContainer( CapsuleContainer& capsule_container )
	{
		using namespace physics;

		CCapsuleShapeDesc *pCap = new CCapsuleShapeDesc;
		pCap->fRadius = capsule_container.capsule.radius;
		pCap->fLength = Vec3Length(capsule_container.capsule.p1 - capsule_container.capsule.p0);
		pCap->LocalPose.vPosition = (capsule_container.capsule.p1 + capsule_container.capsule.p0) * 0.5f;
		pCap->LocalPose.matOrient = CreateOrientFromFwdDir( Vec3GetNormalized(capsule_container.capsule.p1 - capsule_container.capsule.p0) );

		m_pShapeDescs.push_back( pCap );
	}

	void VisitConvexContainer( ConvexContainer& convex_container )
	{ 
		using namespace physics;

		physics::CTriangleMeshDesc convex_mesh_desc;
		convex_mesh_desc.m_vecVertex = convex_container.points;
//		convex_mesh_desc.m_vecIndex  = convex_container.indices;
		convex_mesh_desc.m_vecIndex.resize( 0 );
		convex_mesh_desc.m_vecIndex.resize( convex_container.indices.size() );
		for( size_t i=0; i<convex_container.indices.size(); i++ )
			convex_mesh_desc.m_vecIndex[i] = (int)convex_container.indices[i];


		const int default_material_index = 0;
		convex_mesh_desc.m_vecMaterialIndex.resize( convex_container.indices.size() / 3, default_material_index );

		CConvexShapeDesc *pConvex = new CConvexShapeDesc;

		bool res = SetConvexShapeDesc( convex_mesh_desc, *pConvex );
		if( !res )
		{
			LOG_PRINT_ERROR( "SetConvexShapeDesc() failed." );
			return;
		}

		m_pShapeDescs.push_back( pConvex );
	}
};


} // namespace amorphous



#endif /* __PhysicsShapeMakerVisitor_HPP__ */
