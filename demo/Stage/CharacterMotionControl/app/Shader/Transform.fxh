#ifndef __Transform_FXH__
#define __Transform_FXH__


#include "Quaternion.fxh"


struct Transform
{
	float4 translation;
	float4 rotation; ///< quaternion
};


float3 mul( Transform transform, float3 rhs )
{
	float4 res = quat_mul( quat_mul( transform.rotation, float4( rhs.x, rhs.y, rhs.z, 0 ) ), quat_inv( transform.rotation ) );
	return res.xyz + transform.translation.xyz / transform.translation.w;
}


float4 mul( Transform transform, float4 rhs )
{
	return float4( mul( transform, rhs.xyz / rhs.w ), 1 );
}


#endif /* __Transform_FXH__ */
