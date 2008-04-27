#ifndef  __D3DXVEC3_COPY_H__
#define  __D3DXVEC3_COPY_H__


#ifdef  USE_D3DXVECTOR

inline void Vec3Copy( D3DXVECTOR3& dest, const D3DXVECTOR3& src )
{
	dest.x = src.x;
	dest.y = src.y;
	dest.z = src.z;
}


#else

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

#endif


#endif  /*  __D3DXVEC3_COPY_H__  */



