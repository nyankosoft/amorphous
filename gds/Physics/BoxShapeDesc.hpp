#ifndef  __PhysBoxShapeDesc_H__
#define  __PhysBoxShapeDesc_H__


#include "fwd.h"
#include "ShapeDesc.h"


namespace physics
{


class CBoxShapeDesc : public CShapeDesc
{
public:

	/// holds radii of the box
	Vector3 vSideLength;

public:

	CBoxShapeDesc()
		:
	vSideLength(Vector3(1,1,1))
	{}

	~CBoxShapeDesc() {}

	unsigned int GetArchiveObjectID() const { return PhysShape::Box; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CShapeDesc::Serialize( ar, version );

		ar & vSideLength;
	}


/*	void GetDefaultLocalInertiaTensor( Matrix33 &matInertia, const Scalar fMass )
	{
		Scalar& x = vSideLength.x;
		Scalar& y = vSideLength.y;
		Scalar& z = vSideLength.z;

		Scalar _11 = fMass * (y*y+z*z) / 12.0f;
		Scalar _22 = fMass * (x*x+z*z) / 12.0f;
		Scalar _33 = fMass * (x*x+y*y) / 12.0f;

		matInertia = Matrix33( _11,   0,   0,
			                     0, _22,   0,
								 0,   0, _33 );
	}*/
};


} // namespace physics


#endif  /*  __PhysBoxShapeDesc_H__  */
