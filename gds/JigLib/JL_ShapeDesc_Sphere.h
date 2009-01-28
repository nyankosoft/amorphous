
#ifndef  __JL_SHAPEDESC_SPHERE_H__
#define  __JL_SHAPEDESC_SPHERE_H__


#include "JL_ShapeDesc.h"



#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;


class CJL_ShapeDesc_Sphere : public CJL_ShapeDesc
{
public:

	Scalar fRadius;

	CJL_ShapeDesc_Sphere()
	{
		sShape = JL_SHAPE_SPHERE;
		fRadius = 0;
	}

	~CJL_ShapeDesc_Sphere() {}


	void GetDefaultLocalInertiaTensor( Matrix33 &matInertia, const Scalar fMass )
	{
		Scalar _11 = fMass * 2.0f / 5.0f * fRadius * fRadius;

		matInertia = Matrix33( _11,   0,   0,
			                     0, _11,   0,
								 0,   0, _11 );
	}


	unsigned int GetArchiveObjectID() const { return JL_SHAPEDESC_SPHERE; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CJL_ShapeDesc::Serialize( ar, version );

		ar & fRadius;
	}
};


#endif  /*  __JL_SHAPEDESC_SPHERE_H__  */