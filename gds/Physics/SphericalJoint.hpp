#ifndef  __PhysSphericalJoint_H__
#define  __PhysSphericalJoint_H__


#include "3DMath/Matrix34.hpp"
#include "fwd.hpp"
#include "Joint.hpp"


namespace physics
{


class CSphericalJoint : public CJoint
{

public:

	CSphericalJoint() {}

	~CSphericalJoint() {}

	/// Use this for changing a significant number of joint parameters at once. 
//	virtual void  loadFromDesc (const NxSphericalJointDesc &desc) = 0;
	 
	/// Writes all of the object's attributes to the desc struct. 
//	virtual void  saveToDesc (NxSphericalJointDesc &desc) = 0;
	 
	/// Sets the flags to enable/disable the spring/motor/limit. 
	virtual void SetFlags (U32 flags) = 0;
	 
	/// Returns the current flag settings. 
	virtual U32 GetFlags () = 0;
	 
	/// Sets the joint projection mode. 
//	virtual void SetProjectionMode (JointProjectionMode projectionMode) = 0;
	 
	/// Returns the current flag settings. 
//	virtual JointProjectionMode GetProjectionMode () = 0;
};


} // namespace physics


#endif		/*  __PhysSphericalJoint_H__  */



