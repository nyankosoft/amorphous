#ifndef __ContactStreamIterator_HPP__
#define __ContactStreamIterator_HPP__


#include "../base.hpp"
#include "../3DMath/Vector3.hpp"


namespace amorphous
{


namespace physics
{


class CContactStreamIterator
{
public:

	CContactStreamIterator() {}

	virtual ~CContactStreamIterator() {}

	virtual bool  GoNextPair() = 0;

	virtual bool  GoNextPatch() = 0;

	virtual bool  GoNextPoint() = 0;

	virtual U32  GetNumPairs() = 0;

	virtual CShape *GetShape(U32 shapeIndex) = 0;

	virtual U16  GetShapeFlags() = 0;

	virtual U32  GetNumPatches() = 0;

	virtual U32  GetNumPatchesRemaining() = 0;

	virtual const Vector3 GetPatchNormal() = 0;

	virtual U32  GetNumPoints() = 0;

	virtual U32  GetNumPointsRemaining() = 0;

	virtual const Vector3 GetPoint() = 0;

	virtual float  GetSeparation() = 0;

	virtual U32  GetFeatureIndex0() = 0;

	virtual U32  GetFeatureIndex1() = 0;

	virtual float  GetPointNormalForce() = 0;
};


} // physics
} // namespace amorphous



#endif /* __ContactStreamIterator_HPP__ */
