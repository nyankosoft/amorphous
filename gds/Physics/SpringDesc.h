#ifndef  __PhysSpringDesc_H__
#define  __PhysSpringDesc_H__

#include "3DMath/precision.h"
#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;

#include "fwd.h"


namespace physics
{


class CSpringDesc : public IArchiveObjectBase
{
public:

	/// spring coefficient 
	Scalar  Spring;
	 
	/// damper coefficient 
	Scalar  Damper;
	 
	/// target value (angle/position) of spring where the spring force is zero. 
	Scalar  TargetValue;
 

public:

	CSpringDesc() : Spring(1.0f), Damper(0.1f), TargetValue(0) {}

	~CSpringDesc() {}

	void SetDefault()
	{
		Spring = 1.0f;
		Damper = 0.1f;
		TargetValue = 0.0f;
	}
};


} // namespace physics


#endif		/*  __PhysSpringDesc_H__  */
