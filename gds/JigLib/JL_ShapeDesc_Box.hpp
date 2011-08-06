#ifndef  __JL_SHAPEDESC_BOX_H__
#define  __JL_SHAPEDESC_BOX_H__


#include "JL_ShapeDesc.hpp"


class CJL_ShapeDesc_Box : public CJL_ShapeDesc
{
public:
	Vector3 vSideLength;	// lengths of the edges (full length)

	CJL_ShapeDesc_Box()
	{
		sShape = JL_SHAPE_BOX;
		vSideLength = Vector3(0,0,0);
	}

	~CJL_ShapeDesc_Box() {}


	void GetDefaultLocalInertiaTensor( Matrix33 &matInertia, const Scalar fMass )
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
	}


	unsigned int GetArchiveObjectID() const { return JL_SHAPEDESC_BOX; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CJL_ShapeDesc::Serialize( ar, version );

		ar & vSideLength;
	}
};


#endif  /*  __JL_SHAPEDESC_BOX_H__  */