#ifndef  __PhysMaterialDesc_H__
#define  __PhysMaterialDesc_H__

#include "3DMath/precision.h"

#include "Support/Serialization/Serialization.h"
using namespace GameLib1::Serialization;


namespace physics
{


class CMaterialDesc : public IArchiveObjectBase
{
public:

	Scalar StaticFriction;
	Scalar DynamicFriction;
	Scalar Restitution;      ///< coefficient of restitution (a.k.a. elasticity) - Gets multiplied with the other body in collisions. default = 0

public:

	inline CMaterialDesc();

	inline void SetDefault();

	inline void Serialize( IArchive& ar, const unsigned int version );
};


//============================= inline implementations =============================

inline CMaterialDesc::CMaterialDesc()
{
	SetDefault();
}

inline void CMaterialDesc::SetDefault()
{
	StaticFriction  = 1.0f;
	DynamicFriction = 1.0f;
	Restitution     = 0.0f;
}

inline void CMaterialDesc::Serialize( IArchive& ar, const unsigned int version )
{
	ar & StaticFriction;
	ar & DynamicFriction;
	ar & Restitution;
}


} // namespace physics


#endif		/*  __PhysMaterialDesc_H__  */
