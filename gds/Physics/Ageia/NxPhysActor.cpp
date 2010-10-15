#include "NxPhysActor.hpp"
#include "NxPhysShapeDescFactory.hpp"
#include "NxPhysShapeFactory.hpp"


namespace physics
{


CShape* CNxPhysActor::CreateShape( CShapeDesc& desc )
{
	CNxPhysShapeDescFactory desc_factory;
	NxShapeDesc *pNxShapeDesc = desc_factory.CreateNxShapeDesc( desc );
	if( !pNxShapeDesc )
		return NULL;

	NxShape *pNxShape = m_pActor->createShape( *pNxShapeDesc );

	CNxPhysShapeFactory factory;
	CShape *pShape = factory.CreateShape( pNxShape );
	if( !pShape )
		return NULL;

	AddPhysShape( pShape );

	return pShape;
}


} // namespace physics
