#ifndef  __D3DXVEC3_ADAPTER_H__
#define  __D3DXVEC3_ADAPTER_H__


#ifdef  USE_D3DXVECTOR3

inline void Vec3Copy( D3DXVECTOR3& dest, const D3DXVECTOR3& src )
{
	dest.x = src.x;
	dest.y = src.y;
	dest.z = src.z;
}


#else

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


#endif  /*  __D3DXVEC3_ADAPTER_H__  */



