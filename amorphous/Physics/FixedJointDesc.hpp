#ifndef __FixedJointDesc_HPP__
#define __FixedJointDesc_HPP__


#include "JointDesc.hpp"
#include "Enums.hpp"


namespace amorphous
{


namespace physics
{


class CFixedJointDesc : public CJointDesc
{
public:
	CFixedJointDesc(){}
	~CFixedJointDesc(){}

	JointType::Name GetType() const { return JointType::FIXED; }

	void Accept( CJointDescVisitor& visitor ) const { visitor.VisitFixedJointDesc( *this ); }
};


} // namespace physics


} // amorphous



#endif /* __FixedJointDesc_HPP__ */
