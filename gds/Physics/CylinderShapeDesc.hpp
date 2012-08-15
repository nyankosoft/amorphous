#ifndef  __PhysCylinderShapeDesc_H__
#define  __PhysCylinderShapeDesc_H__


#include "fwd.hpp"
#include "ShapeDesc.hpp"


namespace physics
{


class CCylinderShapeDesc : public CShapeDesc
{
public:

	Scalar fRadius;
	Scalar fLength;
	unsigned int NumSides;

public:

	CCylinderShapeDesc() : fRadius(1.0f), fLength(1.0f), NumSides(16) {}

	virtual ~CCylinderShapeDesc() {}

	unsigned int GetArchiveObjectID() const { return PhysShape::Cylinder; }

	inline virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CShapeDesc::Serialize( ar, version );

		ar & fRadius & fLength;
		ar & NumSides;
	}
};


} // namespace physics


#endif  /*  __PhysCylinderShapeDesc_H__  */
