#ifndef  __PhysFixedJoint_H__
#define  __PhysFixedJoint_H__


#include "Joint.hpp"


namespace amorphous
{


namespace physics
{


class CFixedJoint : public CJoint
{

public:

	CFixedJoint() {}

	~CFixedJoint() {}

	/// Use this for changing a significant number of joint parameters at once. 
//	virtual void  loadFromDesc (const NxFixedJointDesc &desc) = 0;

	/// Writes all of the object's attributes to the desc struct. 
//	virtual void  saveToDesc (NxFixedJointDesc &desc) = 0;
};


} // namespace physics

} // amorphous



#endif		/*  __PhysFixedJoint_H__  */



