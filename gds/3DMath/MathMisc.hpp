#ifndef  __MATHMISC_H__
#define  __MATHMISC_H__


namespace amorphous
{

template<typename T>
void Limit( T& val, const T& min, const T& max )
{
	if( val < min )
		val = min;
	else if( max < val )
		val = max;
}


template<typename T>
inline float TakeMax( T a, T b )
{
	return (b < a) ? a : b;
}


template<typename T>
inline float TakeMin( T a, T b )
{
	return (a < b) ? a : b;
}


template<typename T>
inline float TakeMax( T a, T b, T c )
{
	return TakeMax( TakeMax(a,b), c );
}


template<typename T>
inline float TakeMin( T a, T b, T c )
{
	return TakeMin( TakeMin(a,b), c );
}

} // namespace amorphous


#endif		/*  __MATHMISC_H__  */
