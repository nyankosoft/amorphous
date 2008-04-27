#ifndef  __PhysShapeDesc_H__
#define  __PhysShapeDesc_H__


#include "3DMath/Matrix34.h"

#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/Serialization_3DMath.h"
using namespace GameLib1::Serialization;

#include "fwd.h"
#include "ShapeEnums.h"


namespace physics
{


class CShapeDesc : public IArchiveObjectBase
{
	/// pointer to the owner - set by the program during the actor initialization
	/// the user does not have to care about this
//	PhysicsActor *pPhysicsActor;

public:

	Matrix34 LocalPose;

	int MaterialIndex;

public:

	CShapeDesc() { SetDefault(); }

	virtual ~CShapeDesc() {}

	inline unsigned int GetType() const { return GetArchiveObjectID(); }

	void SetDefault()
	{
//		pPhysicsActor = NULL;
		MaterialIndex = 0;
		LocalPose.Identity();
	}

	inline void SetLocalPose( const Matrix34& local_pose );

//	inline void GetLocalInertiaTensor( Matrix33 &matInertia, const Scalar fMass );

	// translate / rotate inertia tensor according to the current local position / orientation of the shape
//	inline void TranslateInertia( Matrix33 &matInertia, const Scalar fMass );
//	inline void RotateInertia( Matrix33 &matInertia );

	// inertia tensor for each shape when one is at the origin and with the default orientation
//	virtual void GetDefaultLocalInertiaTensor( Matrix33 &matInertia, const Scalar fMass ) {}

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & LocalPose;
		ar & MaterialIndex;
	}

	friend class CShape;
};


inline void CShapeDesc::SetLocalPose( const Matrix34& local_pose )
{
	LocalPose = local_pose;
}

/*
inline void ShapeDesc::GetLocalInertiaTensor( Matrix33 &matInertia, const Scalar fMass )
{
	GetDefaultLocalInertiaTensor( matInertia, fMass );

	// translate inertia tensor according to the current local position of the shape
	TranslateInertia( matInertia, fMass );

	// rotate inertia tensor according to the current local orientation of the shape
	RotateInertia( matInertia );

}*/

/*
inline void ShapeDesc::TranslateInertia( Matrix33 &matInertia, const Scalar fMass )
{
	// I += mass * ( (skew_symmetric_matrix(vLocalPos))^2 - (skew_symmetric_matrix(vLocalPos + vTranslation))^2 )

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


inline void ShapeDesc::RotateInertia( Matrix33 &matInertia )
{
	Matrix33 matLocalOrientT = Matrix33Transpose( matLocalOrient );	// transpose of local orientation

	matInertia = matLocalOrient * matInertia * matLocalOrientT;

	matInertia(0,1) = matInertia(1,0);
	matInertia(0,2) = matInertia(2,0);
	matInertia(1,2) = matInertia(2,1);
}
*/


} // namespace physics


#endif		/*  __PhysShapeDesc_H__  */
