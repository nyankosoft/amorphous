#ifndef __TCBSpline_HPP__
#define __TCBSpline_HPP__


#include "Matrix44.hpp"
#include "Vector2.hpp"
#include "Quaternion.hpp"


namespace amorphous
{


inline Vector2 TCBSplineMultiply( const Vector2& p1, const Vector2& p2, const Vector2& incoming_tangent, const Vector2& outgoing_tangent, const Vector4& hS )
{
	Vector2 dest;
	dest.x = p1.x * hS.x + p2.x * hS.y + incoming_tangent.x * hS.z + outgoing_tangent.x * hS.w;
	dest.y = p1.y * hS.x + p2.y * hS.y + incoming_tangent.y * hS.z + outgoing_tangent.y * hS.w;
//	dest = (1.0f - hS.z) * p1 + hS.z * p2;
	return dest;
}


inline Vector3 TCBSplineMultiply( const Vector3& p1, const Vector3& p2, const Vector3& incoming_tangent, const Vector3& outgoing_tangent, const Vector4& hS )
{
	Vector3 dest;
	dest.x = p1.x * hS.x + p2.x * hS.y + incoming_tangent.x * hS.z + outgoing_tangent.x * hS.w;
	dest.y = p1.y * hS.x + p2.y * hS.y + incoming_tangent.y * hS.z + outgoing_tangent.y * hS.w;
	dest.z = p1.z * hS.x + p2.z * hS.y + incoming_tangent.z * hS.z + outgoing_tangent.z * hS.w;
	return dest;
}


inline Quaternion TCBSplineMultiply( const Quaternion& p1, const Quaternion& p2, const Quaternion& incoming_tangent, const Quaternion& outgoing_tangent, const Vector4& hS )
{
	Quaternion dest;
	dest.x = p1.x * hS.x + p2.x * hS.y + incoming_tangent.x * hS.z + outgoing_tangent.x * hS.w;
	dest.y = p1.y * hS.x + p2.y * hS.y + incoming_tangent.y * hS.z + outgoing_tangent.y * hS.w;
	dest.z = p1.z * hS.x + p2.z * hS.y + incoming_tangent.z * hS.z + outgoing_tangent.z * hS.w;
	dest.w = p1.w * hS.x + p2.w * hS.y + incoming_tangent.w * hS.z + outgoing_tangent.w * hS.w;
	return dest;
}


template<typename T>
inline T InterpolateWithTCBSpline( float fraction, const T& p0, const T& p1, const T& p2, const T& p3, float tension = 0, float continuity = 0, float bias = 0 )
{
	const float& t = tension;
	const float& c = continuity;
	const float& b = bias;

	T incoming_tangent
		= (1-t) * (1-c) * (1+b) * 0.5f * (p2-p1)
		+ (1-t) * (1+c) * (1-b) * 0.5f * (p3-p2);

	T outgoing_tangent
		= (1-t) * (1+c) * (1+b) * 0.5f * (p1-p0)
		+ (1-t) * (1-c) * (1-b) * 0.5f * (p2-p1);

//	const Matrix44 h(
//		 2,-2, 1, 1,
//		-3, 3,-2,-1,
//		 0, 0, 1, 0,
//		 1, 0, 0, 0 );
	Vector4 c0( 2,-2, 1, 1 );
	Vector4 c1(-3, 3,-2,-1 );
	Vector4 c2( 0, 0, 1, 0 );
	Vector4 c3( 1, 0, 0, 0 );
	Matrix44 h;
	h.SetColumn( 0, c0 );
	h.SetColumn( 1, c1 );
	h.SetColumn( 2, c2 );
	h.SetColumn( 3, c3 );

	float f = fraction;
	Vector4 S(
		f*f*f,
		f*f,
		f,
		1 );

	Vector4 hS = h * S;

	// C * h * S
//	return TCB_Multiply()( p1, p2, outgoing_tangent, incoming_tangent, hS );
	return TCBSplineMultiply( p1, p2, outgoing_tangent, incoming_tangent, hS );

//	T P = C * h * S;

//	return P;
}


} // amorphous



#endif /* __TCBSpline_HPP__ */
