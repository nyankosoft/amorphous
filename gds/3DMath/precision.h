#ifndef __3DMATH_PRECISION_H__
#define __3DMATH_PRECISION_H__

//#define USING_DOUBLE

// do not define USING_DOUBLE and USE_D3DXVECTOR3 at the same time
// precision has to be float in order to use D3DX library as Vector3 class


#ifdef USING_DOUBLE
typedef double Scalar;
#else
typedef float Scalar;
#endif


#define SCALAR_TINY		0.0000001f


#endif  /*  __3DMATH_PRECISION_H__  */
