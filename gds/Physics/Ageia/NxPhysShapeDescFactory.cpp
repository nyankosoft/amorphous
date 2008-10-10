#include "NxPhysShapeDescFactory.h"
#include "NxMathConv.h"
#include "../ShapeDesc.h"
#include "../BoxShapeDesc.h"
#include "../SphereShapeDesc.h"
#include "../CapsuleShapeDesc.h"
#include "../TriangleMeshDesc.h"
#include "../TriangleMeshShapeDesc.h"
#include "NxPhysTriangleMesh.h"


namespace physics
{


NxShapeDesc *CNxPhysShapeDescFactory::CreateNxShapeDesc( CShapeDesc &src_desc )
{
	switch( src_desc.GetArchiveObjectID() )
	{
	case PhysShape::Box:
		{
			CBoxShapeDesc *pSrcBoxDesc = dynamic_cast<CBoxShapeDesc *>(&src_desc);
			NxBoxShapeDesc *pDesc = new NxBoxShapeDesc();
			pDesc->dimensions = ToNxVec3( pSrcBoxDesc->vSideLength );
			//pDesc->mass = src_desc.???
			return pDesc;
		}
		break;

	case PhysShape::Sphere:
		{
			CSphereShapeDesc *pSrcSphereDesc = dynamic_cast<CSphereShapeDesc *>(&src_desc);
			NxSphereShapeDesc *pDesc = new NxSphereShapeDesc();
			pDesc->radius = pSrcSphereDesc->Radius;
			return pDesc;
		}
		break;

	case PhysShape::Capsule:
		{
			CCapsuleShapeDesc *pSrcCapsuleDesc = dynamic_cast<CCapsuleShapeDesc *>(&src_desc);
			NxCapsuleShapeDesc *pDesc = new NxCapsuleShapeDesc();
			pDesc->radius = pSrcCapsuleDesc->fRadius;
			pDesc->height = pSrcCapsuleDesc->fLength;
			return pDesc;
		}
		break;

	case PhysShape::TriangleMesh:
		{
			CTriangleMeshShapeDesc *pSrcTriMeshDesc = dynamic_cast<CTriangleMeshShapeDesc *>(&src_desc);
			NxTriangleMeshShapeDesc *pDesc = new NxTriangleMeshShapeDesc();

			// retrieve the triangle mesh of Ageia PhysX
			CNxPhysTriangleMesh *pNxPhysMesh = dynamic_cast<CNxPhysTriangleMesh *>(pSrcTriMeshDesc->pTriangleMesh);
			pDesc->meshData = pNxPhysMesh->GetNxTriangleMesh();
			return pDesc;
		}
		break;

	default:
		break;
	}

	return NULL;
}


}
