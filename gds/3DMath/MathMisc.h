#ifndef  __MATHMISC_H__
#define  __MATHMISC_H__


template<typename T>
void Limit( T& val, const T& min, const T& max )
{
	if( val < min )
		val = min;
	else if( max < val )
		val = max;
}


#endif		/*  __MATHMISC_H__  */
