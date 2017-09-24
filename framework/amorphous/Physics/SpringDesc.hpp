#ifndef  __PhysSpringDesc_H__
#define  __PhysSpringDesc_H__

#include "amorphous/3DMath/precision.h"
#include "amorphous/Support/Serialization/Serialization.hpp"
#include "amorphous/Support/Serialization/ArchiveObjectFactory.hpp"
#include "fwd.hpp"


namespace amorphous
{
using namespace serialization;


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

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & Spring;
		ar & Damper;
		ar & TargetValue;
	}
};


} // namespace physics

} // namespace amorphous



#endif		/*  __PhysSpringDesc_H__  */
