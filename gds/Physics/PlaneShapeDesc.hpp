#ifndef  __PhysPlaneShapeDesc_H__
#define  __PhysPlaneShapeDesc_H__


#include "fwd.hpp"
#include "ShapeDesc.hpp"
#include "../3DMath/Plane.hpp"


namespace physics
{


class CPlaneShapeDesc : public CShapeDesc
{
public:

	/// holds radii of the box
	Plane plane;

public:

	CPlaneShapeDesc()
	{}

	~CPlaneShapeDesc() {}

	unsigned int GetArchiveObjectID() const { return PhysShape::Plane; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CShapeDesc::Serialize( ar, version );

		ar & plane;
	}
};


} // namespace physics


#endif  /*  __PhysPlaneShapeDesc_H__  */
