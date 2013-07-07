#include "NxPhysShapeDescFactory.hpp"
#include "NxPhysConv.hpp"
#include "NxMathConv.hpp"
#include "amorphous/Physics/ShapeDesc.hpp"
#include "amorphous/Physics/BoxShapeDesc.hpp"
#include "amorphous/Physics/SphereShapeDesc.hpp"
#include "amorphous/Physics/CapsuleShapeDesc.hpp"
#include "amorphous/Physics/TriangleMeshDesc.hpp"
#include "amorphous/Physics/TriangleMeshShapeDesc.hpp"
#include "amorphous/Physics/ConvexShapeDesc.hpp"
#include "amorphous/Physics/PlaneShapeDesc.hpp"
#include "NxPhysTriangleMesh.hpp"
#include "NxPhysConvexMesh.hpp"


namespace amorphous
{


namespace physics
{


NxShapeDesc *CNxPhysShapeDescFactory::CreateNxShapeDesc( CShapeDesc &src_desc )
{
	NxShapeDesc *pNxShapeDesc = NULL;
	switch( src_desc.GetArchiveObjectID() )
	{
	case PhysShape::Box:
		{
			CBoxShapeDesc *pSrcBoxDesc = dynamic_cast<CBoxShapeDesc *>(&src_desc);
			NxBoxShapeDesc *pDesc = new NxBoxShapeDesc();
			pDesc->dimensions = ToNxVec3( pSrcBoxDesc->vSideLength );
			//pDesc->mass = src_desc.???
			pNxShapeDesc = pDesc;
		}
		break;

	case PhysShape::Sphere:
		{
			CSphereShapeDesc *pSrcSphereDesc = dynamic_cast<CSphereShapeDesc *>(&src_desc);
			NxSphereShapeDesc *pDesc = new NxSphereShapeDesc();
			pDesc->radius = pSrcSphereDesc->Radius;
			pNxShapeDesc = pDesc;
		}
		break;

	case PhysShape::Cylinder:
		{
			LOG_PRINT_ERROR( " Cylinder shape is not supported." );
//			CCylinderShapeDesc *pSrcCylinderDesc = dynamic_cast<CCylinderShapeDesc *>(&src_desc);
//			NxCapsuleShapeDesc *pDesc = new NxCapsuleShapeDesc();
//			pDesc->radius = pSrcCapsuleDesc->fRadius;
//			pDesc->height = pSrcCapsuleDesc->fLength;
//			pNxShapeDesc = pDesc;
		}
		break;

	case PhysShape::Capsule:
		{
			CCapsuleShapeDesc *pSrcCapsuleDesc = dynamic_cast<CCapsuleShapeDesc *>(&src_desc);
			NxCapsuleShapeDesc *pDesc = new NxCapsuleShapeDesc();
			pDesc->radius = pSrcCapsuleDesc->fRadius;
			pDesc->height = pSrcCapsuleDesc->fLength;
			pNxShapeDesc = pDesc;
		}
		break;

	case PhysShape::TriangleMesh:
		{
			CTriangleMeshShapeDesc *pSrcTriMeshDesc = dynamic_cast<CTriangleMeshShapeDesc *>(&src_desc);
			NxTriangleMeshShapeDesc *pDesc = new NxTriangleMeshShapeDesc();

			// retrieve the triangle mesh of Ageia PhysX
			CNxPhysTriangleMesh *pNxPhysMesh = dynamic_cast<CNxPhysTriangleMesh *>(pSrcTriMeshDesc->pTriangleMesh);
			pDesc->meshData = pNxPhysMesh->GetNxTriangleMesh();
			pNxShapeDesc = pDesc;
		}
		break;

	case PhysShape::Convex:
		{
			CConvexShapeDesc *pSrConvexDesc = dynamic_cast<CConvexShapeDesc *>(&src_desc);
			NxConvexShapeDesc *pDesc = new NxConvexShapeDesc();

			// retrieve the convex mesh of Ageia PhysX
			CNxPhysConvexMesh *pNxPhysMesh = dynamic_cast<CNxPhysConvexMesh *>(pSrConvexDesc->pConvexMesh);
			pDesc->meshData = pNxPhysMesh->GetNxConvexMesh();
			pNxShapeDesc = pDesc;
		}
		break;

	case PhysShape::Plane:
		{
			CPlaneShapeDesc *pSrcPlaneDesc = dynamic_cast<CPlaneShapeDesc *>(&src_desc);
			NxPlaneShapeDesc *pDesc = new NxPlaneShapeDesc();
			pDesc->d      = pSrcPlaneDesc->plane.dist;
			pDesc->normal = ToNxVec3( pSrcPlaneDesc->plane.normal );
			pNxShapeDesc = pDesc;
		}
		break;

	default:
		break;
	}

	if( !pNxShapeDesc )
		return NULL;

	// Copy the values of member variables from CShapeDesc to NxShapeDesc
	NxShapeDesc& desc = *pNxShapeDesc;
	desc.localPose        = ToNxMat34( src_desc.LocalPose );
	desc.materialIndex    = src_desc.MaterialIndex;
	desc.shapeFlags       = ToNxShapeFlags( src_desc.ShapeFlags );
	desc.group            = 0;//src_shape_desc.CollisionGroup;
	desc.groupsMask.bits0 = 0;
	desc.groupsMask.bits1 = 0;
	desc.groupsMask.bits2 = 0;
	desc.groupsMask.bits3 = 0;

	return pNxShapeDesc;
}


}


} // namespace amorphous
