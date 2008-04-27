#ifndef  __PhysJointDesc_H__
#define  __PhysJointDesc_H__

#include <float.h>

#include "3DMath/precision.h"
#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;

#include "fwd.h"


namespace physics
{


class CJointDesc : public IArchiveObjectBase
{
public:

	/// The two actors connected by the joint. 
	CActor *pActor[2];

	/// X axis of joint space, in actor[i]'s space, orthogonal to localAxis[i]. 
	Vector3  LocalNormal[2];

	/// Z axis of joint space, in actor[i]'s space. This is the primary axis of the joint. 
	Vector3  LocalAxis[2];

	/// Attachment point of joint in actor[i]'s space. 
	Vector3  LocalAnchor[2];

	/// local pose in actor[i]'s space
//	Matrix34 LocalPose[2];

	/// Maximum angular force (torque) that the joint can withstand before breaking, must be positive. 
	Scalar  MaxForce;
	Scalar  MaxTorque;

	/// Will be copied to NxJoint::userData. 
//	void *  userData;

	/// Possible debug name. The string is not copied by the SDK, only the pointer is stored. 
//	const char *  name;

	U32  JointFlags;

public:

	CJointDesc() { SetDefault(); }

	~CJointDesc() {}

	void SetDefault()
	{
		pActor[0] = NULL;
		pActor[1] = NULL;

		LocalNormal[0] = LocalNormal[1] = Vector3(1,0,0);
		LocalAxis[0]   = LocalAxis[1]   = Vector3(0,0,1);
		LocalAnchor[0] = LocalAnchor[1] = Vector3(0,0,0);

		MaxForce  = FLT_MAX;
		MaxTorque = FLT_MAX;
	}

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		// how to save the actors info???
		// - save string names?

		ar & LocalNormal[0] & LocalNormal[1];
		ar & LocalAxis[0]   & LocalAxis[1];
		ar & LocalAnchor[0] & LocalAnchor[1];

		ar & MaxForce & MaxTorque;
	}
};


} // namespace physics



#endif		/*  __PhysJointDesc_H__  */
