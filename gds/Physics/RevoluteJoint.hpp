#ifndef  __PhysRevoluteJoint_H__
#define  __PhysRevoluteJoint_H__


#include "Joint.hpp"


namespace physics
{


class CRevoluteJoint : public CJoint
{

public:

	CRevoluteJoint() {}

	~CRevoluteJoint() {}
/*
	/// Sets angular joint limits. 
	SetLimits( const NxJointLimitPairDesc &pair ) { return m_pImpl->(); }

	/// Retrieves the joint limits. 
	virtual bool  GetLimits(NxJointLimitPairDesc &pair ) { return m_pImpl->(); }

	/// Sets motor parameters for the joint. 
	virtual void  SetMotor( const NxMotorDesc &motorDesc ) { return m_pImpl->(); }

	/// Reads back the motor parameters. 
	virtual bool  GetMotor(NxMotorDesc &motorDesc ) { return m_pImpl->(); }

	/// Sets spring parameters. 
	virtual void  SetSpring( const SpringDesc &springDesc ) { return m_pImpl->(); }

	/// Retrieves spring settings. 
	virtual bool  GetSpring(NxSpringDesc &springDesc ) { return m_pImpl->(); }

	/// Retrieves the current revolute joint angle. 
	virtual Scalar  GetAngle() { return m_pImpl->(); }

	/// Retrieves the revolute joint angle's rate of change (angular velocity). 
	virtual Scalar  GetVelocity() { return m_pImpl->(); }

	/// Sets the flags to enable/disable the spring/motor/limit. 
	virtual void  SetFlags (U32 flags ) { return m_pImpl->(); }

	/// Retrieve the revolute joints flags. 
	virtual U32  GetFlags() { return m_pImpl->(); }

	/// Sets the joint projection mode. 
	virtual void  SetProjectionMode (NxJointProjectionMode projectionMode ) { return m_pImpl->(); }

	/// Retrieves the joints projection mode. 
	virtual NxJointProjectionMode  GetProjectionMode() { return m_pImpl->(); }
*/
};


} // namespace physics


#endif		/*  __PhysRevoluteJoint_H__  */



