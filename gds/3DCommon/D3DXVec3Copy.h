#ifndef  __D3DXVEC3_COPY_H__
#define  __D3DXVEC3_COPY_H__


#include "3DMath/3DMathLibBase.h"


#ifdef USE_D3DXVECTOR

inline void Vec3Copy( D3DXVECTOR3& dest, const D3DXVECTOR3& src )
{
	dest.x = src.x;
	dest.y = src.y;
	dest.z = src.z;
}


#else /* USE_D3DXVECTOR */

#include "3DMath/Vector3.h"

inline void Vec3Copy( D3DXVECTOR3& dest, const Vector3& src )
{
	dest.x = src.x;
	dest.y = src.y;
	dest.z = src.z;
}

inline void Vec3Copy( Vector3& dest, const D3DXVECTOR3& src )
{
	dest.x = src.x;
	dest.y = src.y;
	dest.z = src.z;
}

#endif /* USE_D3DXVECTOR */


#endif  /*  __D3DXVEC3_COPY_H__  */
