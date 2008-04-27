#ifndef  __PhysBoxShapeDesc_H__
#define  __PhysBoxShapeDesc_H__


#include "fwd.h"
#include "ShapeDesc.h"


namespace physics
{


class CBoxShapeDesc : public CShapeDesc
{
	/// holds radii of the box
	Vector3 SideLength;

public:

	CBoxShapeDesc()
	{
		SetSideLength( Vector3(1,1,1) );
	}

	~CBoxShapeDesc() {}

	inline void SetSideLength( Vector3 vSideLength );	// lengths of the edges (radii)

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


	unsigned int GetArchiveObjectID() const { return PhysShape::Box; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CShapeDesc::Serialize( ar, version );

		ar & SideLength;
	}
};


//inline void CBoxShapeDesc::SetSideLength( Vector3 vSideLength ){}

// lengths of the edges (full length)


} // namespace physics


#endif  /*  __PhysBoxShapeDesc_H__  */
