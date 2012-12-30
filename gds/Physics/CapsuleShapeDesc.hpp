#ifndef  __PhysCapsuleShapeDesc_H__
#define  __PhysCapsuleShapeDesc_H__


#include "fwd.hpp"
#include "ShapeDesc.hpp"


namespace amorphous
{


namespace physics
{


class CCapsuleShapeDesc : public CShapeDesc
{
public:

	Scalar fRadius;
	Scalar fLength;

public:

	CCapsuleShapeDesc() : fRadius(1.0f), fLength(1.0f) {}

	virtual ~CCapsuleShapeDesc() {}

	unsigned int GetArchiveObjectID() const { return PhysShape::Capsule; }

	inline virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CShapeDesc::Serialize( ar, version );

		ar & fRadius & fLength;
	}


/*
	void GetDefaultLocalInertiaTensor( Matrix33 &matInertia, const Scalar fMass )
	{
		// TODO: introduce correct inertia tensor for capsule. the following is the inertia for cylinder

		Scalar& r = fRadius;
		Scalar h = fLength + fRadius * 2.0f;

		Scalar _11 = fMass * (3.0f*r*r + h*h) / 12.0f;
		Scalar _22 = fMass * (3.0f*r*r + h*h) / 12.0f;
		Scalar _33 = fMass * (6.0f*r*r) / 12.0f;

		matInertia = Matrix33( _11,   0,   0,
			                     0, _22,   0,
								 0,   0, _33 );
	}

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CShapeDesc::Serialize( ar, version );

		ar & fLength;
		ar & fRadius;
	}*/
};


} // namespace physics

} // namespace amorphous



#endif  /*  __PhysCapsuleShapeDesc_H__  */
