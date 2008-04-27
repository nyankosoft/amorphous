#ifndef __BASICGAMEMATH_H__
#define __BASICGAMEMATH_H__

#include <stdlib.h>


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


/*
inline float RangedRand(float fMax)
{
	return fMax * (float)rand() / (float)RAND_MAX;
}


inline float RangedRand(float fMin, float fMax)
{
	return fMin + (fMax - fMin) * (float)rand() / (float)RAND_MAX;
}


inline int RangedRand(int iMax)
{
	return iMax * rand() / RAND_MAX;
}*/


#endif /*  __BASICGAMEMATH_H__  */
