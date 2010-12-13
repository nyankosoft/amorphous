#ifndef __NxPhysJointImpls_HPP__
#define __NxPhysJointImpls_HPP__


#include "../Joint.hpp"
#include "NxJoint.h"


namespace physics
{


class CNxPhysJointImpl : public CJointImpl
{
	NxJoint *m_pNxJoint;
public:
	CNxPhysJointImpl() : m_pNxJoint(NULL) {}
	CNxPhysJointImpl( NxJoint *pNxJoint ) : m_pNxJoint(pNxJoint) {}
	virtual ~CNxPhysJointImpl(){}

	JointState::Name GetState() { return JointState::UNKNOWN; }

	friend class CNxPhysScene;
};


class CNxPhysFixedJointImpl : public CNxPhysJointImpl
{
	NxFixedJoint *m_pNxFixedJoint;
public:
	CNxPhysFixedJointImpl() : m_pNxFixedJoint(NULL) {}
	~CNxPhysFixedJointImpl(){}

	JointType::Name GetType() const { return JointType::FIXED; }
};


class CNxPhysRevoluteJointImpl : public CNxPhysJointImpl
{
	NxRevoluteJoint *m_pNxRevoluteJoint;
public:
	CNxPhysRevoluteJointImpl() : m_pNxRevoluteJoint(NULL) {}
	~CNxPhysRevoluteJointImpl(){}

	JointType::Name GetType() const { return JointType::REVOLUTE; }
};


class CNxPhysSphericalJointImpl : public CNxPhysJointImpl
{
	NxSphericalJoint *m_pNxSphericalJoint;
public:
	CNxPhysSphericalJointImpl() : m_pNxSphericalJoint(NULL) {}
	~CNxPhysSphericalJointImpl(){}

	JointType::Name GetType() const { return JointType::SPHERICAL; }
};


} // namespace physics



#endif /* __NxPhysJointImpls_HPP__ */
