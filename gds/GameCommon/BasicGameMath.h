#ifndef __BASICGAMEMATH_H__
#define __BASICGAMEMATH_H__


inline float fmax(float a, float b)
{
	return (b < a) ? a : b;
}


inline float fmin(float a, float b)
{
	return (a < b) ? a : b;
}


inline float fmax(float a, float b, float c)
{
	return fmax( fmax(a,b), c );
}


inline float fmin(float a, float b, float c)
{
	return fmin( fmin(a,b), c );
}


#endif /*  __BASICGAMEMATH_H__  */
