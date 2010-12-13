#ifndef  __PhysSphericalJointDesc_H__
#define  __PhysSphericalJointDesc_H__


#include "3DMath/Matrix34.hpp"

#include "fwd.hpp"
#include "JointDesc.hpp"
#include "Enums.hpp"
#include "SpringDesc.hpp"


namespace physics
{


class CSphericalJointDesc : public CJointDesc
{
public:

	/// swing limit axis defined in the joint space of actor 0. 
	Vector3  SwingAxis;

	/// Distance above which to project joint. 
	Scalar  projectionDistance;

	/// limits rotation around twist axis 
//	NxJointLimitPairDesc  TwistLimit;

	/// limits swing of twist axis 
//	NxJointLimitDesc  SwingLimit;

	/// spring that works against twisting 
	CSpringDesc  TwistSpring;

	/// spring that works against swinging 
	CSpringDesc  SwingSpring;

	/// spring that lets the joint get pulled apart 
	CSpringDesc  JointSpring;

	/// This is a combination of the bits defined by NxSphericalJointFlag . 
	U32  Flags;

	/// use this to enable joint projection 
//	NxJointProjectionMode  ProjectionMode;

public:

	CSphericalJointDesc() {}

	~CSphericalJointDesc() {}

	JointType::Name GetType() const { return JointType::SPHERICAL; }

	void Accept( CJointDescVisitor& visitor ) const { visitor.VisitSphericalJointDesc( *this ); }
};


} // namespace physics


#endif		/*  __PhysSphericalJointDesc_H__  */
