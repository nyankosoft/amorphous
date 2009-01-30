#ifndef  __PhysRawTriangleMeshShapeDesc_H__
#define  __PhysRawTriangleMeshShapeDesc_H__


#include "fwd.hpp"
#include "ShapeDesc.hpp"
#include "Shape.hpp"


namespace physics
{


class CRawTriangleMeshShapeDesc : public CShapeDesc
{
public:

	CTriangleMeshDesc m_Desc;

	CRawTriangleMeshShapeDesc()
	{
	}

	virtual ~CRawTriangleMeshShapeDesc() {}

	unsigned int GetArchiveObjectID() const { return PhysShape::RawTriangleMesh; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CShapeDesc::Serialize( ar, version );

		ar & m_Desc;
	}
};


} // namespace physics


#endif  /*  __PhysRawTriangleMeshShapeDesc_H__  */
