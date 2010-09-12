#ifndef __NxPhysContactStreamIterator_HPP__
#define __NxPhysContactStreamIterator_HPP__


#include "NxContactStreamIterator.h" // PhysX header
#include "NxMathConv.hpp" // header of this framework
#include "../ContactStreamIterator.hpp" // header of this framework


namespace physics
{


class CNxPhysContactStreamIterator : public CContactStreamIterator
{
	NxContactStreamIterator& m_Iterator;

public:

	CNxPhysContactStreamIterator( NxContactStreamIterator& itr ) : m_Iterator(itr) {}
	~CNxPhysContactStreamIterator() {}

	bool  GoNextPair() { return m_Iterator.goNextPair(); }

	bool  GoNextPatch() { return m_Iterator.goNextPatch(); }

	bool  GoNextPoint() { return m_Iterator.goNextPoint(); }

	U32  GetNumPairs() { return m_Iterator.getNumPairs(); }

	CShape *GetShape(U32 shapeIndex) { return NULL;/*m_Iterator.getShape();*/ }

	U16  GetShapeFlags() { return m_Iterator.getShapeFlags(); }

	U32  GetNumPatches() { return m_Iterator.getNumPatches(); }

	U32  GetNumPatchesRemaining() { return m_Iterator.getNumPatchesRemaining(); }

	const Vector3 GetPatchNormal() { return ToVector3( m_Iterator.getPatchNormal() ); }

	U32  GetNumPoints() { return m_Iterator.getNumPoints(); }

	U32  GetNumPointsRemaining() { return m_Iterator.getNumPointsRemaining(); }

	const Vector3 GetPoint() { return ToVector3( m_Iterator.getPoint() ); }

	float  GetSeparation() { return m_Iterator.getSeparation(); }

	U32  GetFeatureIndex0() { return m_Iterator.getFeatureIndex0(); }

	U32  GetFeatureIndex1() { return m_Iterator.getFeatureIndex1(); }

	float  GetPointNormalForce() { return m_Iterator.getPointNormalForce(); }
};


} // physics

#endif /* __NxPhysContactStreamIterator_HPP__ */
