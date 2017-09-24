#ifndef  __JL_SHAPEDESC_H__
#define  __JL_SHAPEDESC_H__


#include "amorphous/3DMath/Matrix33.hpp"

#include "amorphous/Support/Serialization/Serialization.hpp"
#include "amorphous/Support/Serialization/Serialization_3DMath.hpp"


namespace amorphous
{
using namespace serialization;


enum eJL_ShapeType
{
	JL_SHAPE_BOX = 0,
	JL_SHAPE_CAPSULE,
	JL_SHAPE_SPHERE,
	JL_SHAPE_CONVEX,
	JL_SHAPE_TRIANGLEMESH
};

#define JL_SHAPE_INVALID	-1


class CJL_ShapeBase;
class CJL_PhysicsActor;



class CJL_ShapeDesc : public IArchiveObjectBase
{
	/// pointer to the owner - set by the program during the actor initialization
	/// the user does not have to care about this
	CJL_PhysicsActor *pPhysicsActor;

public:

	enum eShapeDescID
	{
		JL_SHAPEDESC_BOX,
		JL_SHAPEDESC_CAPSULE,
		JL_SHAPEDESC_SPHERE,
		JL_SHAPEDESC_TRIANGLEMESH,
	};


	short sShape;
	short sMaterialIndex;

	Vector3 vLocalPos;
	Matrix33 matLocalOrient;

	CJL_ShapeDesc() { SetDefault(); }
	virtual ~CJL_ShapeDesc() {}

	void SetDefault()
	{
		pPhysicsActor = NULL;
		sShape = JL_SHAPE_INVALID;
		sMaterialIndex = 0;
		vLocalPos = Vector3(0,0,0);
		matLocalOrient = Matrix33Identity();
	}

	inline void GetLocalInertiaTensor( Matrix33 &matInertia, const Scalar fMass );

	// translate / rotate inertia tensor according to the current local position / orientation of the shape
	inline void TranslateInertia( Matrix33 &matInertia, const Scalar fMass );
	inline void RotateInertia( Matrix33 &matInertia );

	// inertia tensor for each shape when one is at the origin and with the default orientation
	virtual void GetDefaultLocalInertiaTensor( Matrix33 &matInertia, const Scalar fMass ) {}

	friend class CJL_ShapeBase;
	friend class CJL_PhysicsActor;

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & sShape;
		ar & sMaterialIndex;
		ar & vLocalPos;
		ar & matLocalOrient;
	}
};



inline void CJL_ShapeDesc::GetLocalInertiaTensor( Matrix33 &matInertia, const Scalar fMass )
{
	GetDefaultLocalInertiaTensor( matInertia, fMass );

	// translate inertia tensor according to the current local position of the shape
	TranslateInertia( matInertia, fMass );

	// rotate inertia tensor according to the current local orientation of the shape
	RotateInertia( matInertia );

}


inline void CJL_ShapeDesc::TranslateInertia( Matrix33 &matInertia, const Scalar fMass )
{
	/* I += mass * ( (skew_symmetric_matrix(vLocalPos))^2 - (skew_symmetric_matrix(vLocalPos + vTranslation))^2 )*/

	Matrix33 mat1;

//	mat0 = GetSkewSymmetricMatrix( Vector3(0,0,0) );
	mat1 = GetSkewSymmetricMatrix( vLocalPos );

//	mat2 = mat0 * mat0;
//	mat2 -= mat1 * mat1;

//	matInertia += fMass * mat2;

	matInertia -= fMass * mat1 * mat1;

	matInertia(0,1) = matInertia(1,0);
	matInertia(0,2) = matInertia(2,0);
	matInertia(1,2) = matInertia(2,1);
}


inline void CJL_ShapeDesc::RotateInertia( Matrix33 &matInertia )
{
	Matrix33 matLocalOrientT = Matrix33Transpose( matLocalOrient );	// transpose of local orientation

	matInertia = matLocalOrient * matInertia * matLocalOrientT;

	matInertia(0,1) = matInertia(1,0);
	matInertia(0,2) = matInertia(2,0);
	matInertia(1,2) = matInertia(2,1);
}



} // namespace amorphous



#endif		/*  __JL_SHAPEDESC_H__  */