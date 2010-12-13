#ifndef __RevoluteJointDesc_HPP__
#define __RevoluteJointDesc_HPP__


#include "JointDesc.hpp"
#include "Enums.hpp"


namespace physics
{


class CRevoluteJointDesc : public CJointDesc
{
public:
	int Limit;
	int Motor;
	int Spring;
	float ProjectDistance;
	float ProjectAngle;
	U32 Flags;
	int ProjectionMode;
public:
	CRevoluteJointDesc() {}
	~CRevoluteJointDesc() {}

	JointType::Name GetType() const { return JointType::REVOLUTE; }

	void SetToDefault()
	{
	}

	void Accept( CJointDescVisitor& visitor ) const { visitor.VisitRevoluteJointDesc( *this ); }
};


} // namespace physics




#endif /* __RevoluteJointDesc_HPP__ */
