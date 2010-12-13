#ifndef __NxPhysJointDescVisitor_HPP__
#define __NxPhysJointDescVisitor_HPP__


#include "NxJoint.h"
#include "../JointDesc.hpp"
#include "../FixedJointDesc.hpp"
#include "../RevoluteJointDesc.hpp"
#include "../SphericalJointDesc.hpp"


namespace physics
{

/*
inline NxU32 ToNxJointFlags( U32 src )
{
	NxU32 dest = 0;
	if( src & JointFlag::COLLISION_ENABLED ) dest |= NX_JF_COLLISION_ENABLED;
}
*/

class CNxPhysJointDescVisitor : public CJointDescVisitor
{
public:

	CNxPhysJointDescVisitor() : m_pNxJointDesc(NULL) {}

	~CNxPhysJointDescVisitor(){}

public:

	NxJointDesc *m_pNxJointDesc;

	NxFixedJointDesc m_FixedJointDesc;
	NxRevoluteJointDesc m_RevoluteJointDesc;
	NxSphericalJointDesc m_SphericalJointDesc;

	void SetBaseNxJointDesc( const CJointDesc& src, NxJointDesc& dest )
	{
		dest.maxForce   = src.MaxForce;
		dest.maxTorque  = src.MaxTorque;
		dest.jointFlags = 0;//ToNxJointFlags( src.JointFlags );
	}

	void VisitFixedJointDesc( const CFixedJointDesc& desc )
	{
		m_pNxJointDesc = &m_FixedJointDesc;
		SetBaseNxJointDesc( desc, m_FixedJointDesc );
	}

	void VisitRevoluteJointDesc( const CRevoluteJointDesc& desc )
	{
		m_pNxJointDesc = &m_RevoluteJointDesc;
		SetBaseNxJointDesc( desc, m_RevoluteJointDesc );
	}

	void VisitSphericalJointDesc( const CSphericalJointDesc& desc )
	{
		m_pNxJointDesc = &m_SphericalJointDesc;
		SetBaseNxJointDesc( desc, m_SphericalJointDesc );
	}
};


} // namespace physics



#endif /* __NxPhysJointDescVisitor_HPP__ */
