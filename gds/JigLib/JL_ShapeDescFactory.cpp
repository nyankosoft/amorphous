#include "JL_ShapeDescFactory.hpp"
#include "JL_ShapeDesc.hpp"
#include "JL_ShapeDesc_Box.hpp"
#include "JL_ShapeDesc_Capsule.hpp"
#include "JL_ShapeDesc_Sphere.hpp"
#include "JL_ShapeDesc_TriangleMesh.hpp"


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
