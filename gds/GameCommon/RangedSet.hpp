#ifndef __RangedSet_H__
#define __RangedSet_H__


#include "MTRand.hpp"
#include "Support/Serialization/Serialization.hpp"
using namespace GameLib1::Serialization;


template<class T>
class RangedSet : public IArchiveObjectBase
{
public:

	T min, max;

	void Limit( T& x ) const { x = min < x ? x : min; x = max < x ? max : x; }

	/// returns a random value in the range of [min,max]
	/// - availability: int, float
	T GetRangedRand() const { return RangedRand( min, max ); }

	void Serialize( IArchive& ar, const unsigned int version ) { ar & min & max; }
};


/*
template <class T>
inline IArchive& operator & ( IArchive& ar, RangedSet<T>& range )
{
	return ar & range.min & range.max;
}
*/


#endif /* __RangedSet_H__ */
