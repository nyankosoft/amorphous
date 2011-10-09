#ifndef __Quaternion_FXH__
#define __Quaternion_FXH__

// Use float4 as quaternion

float length_sq( float4 src )
{
	return src.x*src.x + src.y*src.y + src.z*src.z + src.w*src.w;
}


float4 quat_inv( float4 src )
{
//	Scalar fNorm = (Scalar)0.0;

	float fNormSq = length_sq(src);

	float4 inv;
//	if ( fNormSq > (Scalar)0.0 )
//	{
		float fInvNormSq = 1.0 / fNormSq;
		inv.x = -src.x*fInvNormSq;
		inv.y = -src.y*fInvNormSq;
		inv.z = -src.z*fInvNormSq;
		inv.w =  src.w*fInvNormSq;
/*	}
	else
	{
		// return an invalid result to flag the error
		inv.x = (Scalar)0.0;
		inv.y = (Scalar)0.0;
		inv.z = (Scalar)0.0;
		inv.w = (Scalar)0.0;
	}*/

	// Replace to this after cofirming the above code works
//	return float4( -src.x, -src.y, -src.z, src.w ) / fNormSq;

	return inv;
}


float4 quat_mul( float4 lhs, float4 rhs )
{
    float4 res;

    res.w =
        lhs.w * rhs.w -
        lhs.x * rhs.x -
        lhs.y * rhs.y -
        lhs.z * rhs.z;

    res.x =
        lhs.w * rhs.x +
        lhs.x * rhs.w +
        lhs.y * rhs.z -
        lhs.z * rhs.y;

    res.y =
        lhs.w * rhs.y +
        lhs.y * rhs.w +
        lhs.z * rhs.x -
        lhs.x * rhs.z;

    res.z =
        lhs.w * rhs.z +
        lhs.z * rhs.w +
        lhs.x * rhs.y -
        lhs.y * rhs.x;

    return res;
}


#endif /* __Quaternion_FXH__ */
