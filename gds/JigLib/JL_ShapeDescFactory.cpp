
#include "JL_ShapeDescFactory.h"


#include "JL_ShapeDesc.h"
#include "JL_ShapeDesc_Box.h"
#include "JL_ShapeDesc_Capsule.h"
#include "JL_ShapeDesc_Sphere.h"
#include "JL_ShapeDesc_TriangleMesh.h"


IArchiveObjectBase *CJL_ShapeDescFactory::CreateObject( const unsigned int id )
{
	switch( id )
	{
	case CJL_ShapeDesc::JL_SHAPEDESC_BOX:			return new CJL_ShapeDesc_Box;
	case CJL_ShapeDesc::JL_SHAPEDESC_CAPSULE:		return new CJL_ShapeDesc_Capsule;
	case CJL_ShapeDesc::JL_SHAPEDESC_SPHERE:		return new CJL_ShapeDesc_Sphere;
	case CJL_ShapeDesc::JL_SHAPEDESC_TRIANGLEMESH:	return new CJL_ShapeDesc_TriangleMesh;
	default:	return NULL;
	}
}