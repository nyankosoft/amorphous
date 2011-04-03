#ifndef __Direct3D_Conversions_HPP__
#define __Direct3D_Conversions_HPP__


#include <d3dx9.h>
#include "../../3DMath/Vector3.hpp"

// Direct3D to 3DMath

inline Vector3 ToVector3( const D3DXVECTOR3& src )
{
	return Vector3(src.x,src.y,src.z);
}


// 3DMath to Direct3D

inline D3DVECTOR ToD3DVECTOR( const Vector3& src )
{
	D3DVECTOR dest;
	dest.x = src.x;
	dest.y = src.y;
	dest.z = src.z;
	return dest;
}


inline D3DXVECTOR3 ToD3DXVECTOR3( const Vector3& src )
{
	return D3DXVECTOR3(src.x,src.y,src.z);
}



#endif /* __Direct3D_Conversions_HPP__ */
