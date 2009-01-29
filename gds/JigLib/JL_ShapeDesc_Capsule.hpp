
#ifndef  __JL_SHAPEDESC_CAPSULE_H__
#define  __JL_SHAPEDESC_CAPSULE_H__


#include "JL_ShapeDesc.h"



#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;


class CJL_ShapeDesc_Capsule : public CJL_ShapeDesc
{
public:
	Scalar fLength;
	Scalar fRadius;

	CJL_ShapeDesc_Capsule()
	{
		sShape = JL_SHAPE_CAPSULE;
		fLength = 0;
		fRadius = 0;
	}

	~CJL_ShapeDesc_Capsule() {}


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


	unsigned int GetArchiveObjectID() const { return JL_SHAPEDESC_CAPSULE; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CJL_ShapeDesc::Serialize( ar, version );

		ar & fLength;
		ar & fRadius;
	}
};


#endif  /*  __JL_SHAPEDESC_CAPSULE_H__  */