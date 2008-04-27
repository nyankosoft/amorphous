#include "NxPhysShapeDescFactory.h"
#include "NxMathConv.h"
#include "../ShapeDesc.h"
#include "../TriangleMeshDesc.h"
#include "../TriangleMeshShapeDesc.h"


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
			pDesc->dimensions = ToNxVec3( pSrcBoxDesc->SideLength );
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
	}

	case PhysShape::Capsule:
		{
			CCapsuleShapeDesc *pSrcCapsuleDesc = dynamic_cast<CCapsuleShapeDesc *>(&src_desc);
			NxCapsuleShapeDesc *pDesc = new NxCapsuleShapeDesc();
			pDesc->radius = pSrcCapsuleDesc->Radius;
			pDesc->height = pSrcCapsuleDesc->Length;
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

	default:
		break;
	}

	return NULL;
}
