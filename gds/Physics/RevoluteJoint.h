#ifndef  __PhysRevoluteJoint_H__
#define  __PhysRevoluteJoint_H__


#include "3DMath/Matrix34.h"
#include "fwd.h"
#include "Joint.h"


namespace physics
{


class CRevoluteJoint : public CJoint
{

public:

	CRevoluteJoint() {}

	~CRevoluteJoint() {}
/*
	/// Sets angular joint limits. 
	SetLimits (const NxJointLimitPairDesc &pair)=0 

	/// Retrieves the joint limits. 
	virtual bool  GetLimits (NxJointLimitPairDesc &pair)=0 

	/// Sets motor parameters for the joint. 
	virtual void  SetMotor (const NxMotorDesc &motorDesc)=0 

	/// Reads back the motor parameters. 
	virtual bool  GetMotor (NxMotorDesc &motorDesc)=0 

	/// Sets spring parameters. 
	virtual void  SetSpring (const SpringDesc &springDesc)=0 

	/// Retrieves spring settings. 
	virtual bool  GetSpring (NxSpringDesc &springDesc)=0 

	/// Retrieves the current revolute joint angle. 
	virtual Scalar  GetAngle ()=0 

	/// Retrieves the revolute joint angle's rate of change (angular velocity). 
	virtual Scalar  GetVelocity ()=0 

	/// Sets the flags to enable/disable the spring/motor/limit. 
	virtual void  SetFlags (U32 flags)=0 

	/// Retrieve the revolute joints flags. 
	virtual U32  GetFlags ()=0 

	/// Sets the joint projection mode. 
	virtual void  SetProjectionMode (NxJointProjectionMode projectionMode)=0 

	/// Retrieves the joints projection mode. 
	virtual NxJointProjectionMode  GetProjectionMode ()=0 
*/
};


} // namespace physics


#endif		/*  __PhysRevoluteJoint_H__  */



