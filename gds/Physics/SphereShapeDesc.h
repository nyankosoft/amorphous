#ifndef  __PhysSphereShapeDesc_H__
#define  __PhysSphereShapeDesc_H__


#include "fwd.h"
#include "ShapeDesc.h"


#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;


namespace physics
{


class CSphereShapeDesc : public CShapeDesc
{
public:

	Scalar Radius;

public:

	CSphereShapeDesc()
		:
	Radius(1.0f)
	{
	}

	virtual ~CSphereShapeDesc() {}

	virtual void SetDefault()
	{
		Radius = 1.0f;
	}

	unsigned int GetArchiveObjectID() const { return PhysShape::Sphere; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CShapeDesc::Serialize( ar, version );

		ar & Radius;
	}

/*
	void GetDefaultLocalInertiaTensor( Matrix33 &matInertia, const Scalar fMass )
	{
		Scalar _11 = fMass * 2.0f / 5.0f * fRadius * fRadius;

		matInertia = Matrix33( _11,   0,   0,
			                     0, _11,   0,
								 0,   0, _11 );
	}
*/
};


} // namespace physics


#endif  /*  __PhysSphereShapeDesc_H__  */
