#ifndef  __3DMATH_VECTOR2_H__
#define  __3DMATH_VECTOR2_H__


#include "3DMathLibBase.h"


#ifdef USE_D3DXVECTOR

// use D3DXVECTOR2 class as Vector2
#include "D3DX_Adapter.h"

#else /* USE_D3DXVECTOR */

// use the original Vector2 class
#include "Vector2_Gen.h"
typedef TVector2<Scalar> Vector2;

#endif /* USE_D3DXVECTOR */


#endif  /*  __3DMATH_VECTOR2_H__  */
