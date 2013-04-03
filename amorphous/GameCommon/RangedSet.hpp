#ifndef __RangedSet_H__
#define __RangedSet_H__


#include "gds/Support/MTRand.hpp"
#include "gds/Support/Serialization/Serialization.hpp"


namespace amorphous
{
using namespace serialization;


template<class T>
class RangedSet : public IArchiveObjectBase
{
public:

	T min, max;

	void Limit( T& x ) const { x = min < x ? x : min; x = max < x ? max : x; }

	bool IsInRange( const T& x ) const { return min <= x && x <= max; }

	/// returns a random value in the range of [min,max]
	/// - availability: int, float
	T GetRangedRand() const { return RangedRand( min, max ); }

	void Serialize( IArchive& ar, const unsigned int version ) { ar & min & max; }
};


template<typename T>
const T& GetLimited( const T& val, const T& min = 0.0f, const T& max = 1.0f )
{
	if( val < min )
		return min;
	else if( max < val )
		return max;

	return val;
}


/*
template <class T>
inline IArchive& operator & ( IArchive& ar, RangedSet<T>& range )
{
	return ar & range.min & range.max;
}
*/

} // namespace amorphous



#endif /* __RangedSet_H__ */
