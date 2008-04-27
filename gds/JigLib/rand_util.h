#ifndef __RAND_UTIL_H__
#define __RAND_UTIL_H__


/*!
  Returns a random number between v1 and v2
*/
inline Scalar ranged_random(Scalar v1,Scalar v2)
{
	return v1 + (v2-v1)*((Scalar)rand())/((Scalar)RAND_MAX);
}


#endif  /*  __RAND_UTIL_H__  */