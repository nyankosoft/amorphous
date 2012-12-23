#ifndef  __TRIANGLE_H__
#define  __TRIANGLE_H__

#include "Plane.hpp"
#include "MathMisc.hpp"


class CTriangle
{
	Vector3 m_avVertex[3];
	Vector3 m_vNormal;	// plane normal (the direction of the surface of this triangle)
	float m_fDist;	// plane distance (dist from origin to the surface of this triangle along the direction of 'm_vNormal')

public:

	CTriangle();

	CTriangle( const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& vNormal )
		:
	m_vNormal(vNormal)
	{
		m_avVertex[0] = v0;
		m_avVertex[1] = v1;
		m_avVertex[2] = v2;
		m_fDist = Vec3Dot( vNormal, m_avVertex[0] );
	}

	inline const Vector3& GetVertex(int i) const { return m_avVertex[i]; }

	// get edge 0-2 in cyclic order
	inline Vector3 GetEdge(int i) const { return m_avVertex[(i+1)%3] - m_avVertex[i]; }

	inline const Vector3& GetNormal() const { return m_vNormal; }

	inline Vector3 GetCenter() const { return ( m_avVertex[0] + m_avVertex[1] + m_avVertex[2] ) * 0.333333333333f; }

	inline void GetSpan( float& rfMin, float& rfMax, Vector3 vAxis ) const;

	/// check if the ray intersects with the triangle
	/// if an intersection is found, 'vGoal' is replaced by contact point
	/// and the function returns true
	inline bool RayIntersect( const Vector3& vStart, Vector3& vGoal );

	inline SPlane GetPlane() { SPlane plane; plane.normal = m_vNormal; plane.dist = m_fDist; return plane; }
	


	/// used by distance calculation with segment
	inline Vector3 GetPoint( Scalar t0, Scalar t1 ) { return GetOrigin() + t0 * GetEdge0() + t1 * GetEdge1(); }

	inline const Vector3 GetOrigin() const { return m_avVertex[0]; }

	inline Vector3 GetEdge0() const { return m_avVertex[1] - m_avVertex[0]; }
	inline Vector3 GetEdge1() const { return m_avVertex[2] - m_avVertex[0]; }
	inline Vector3 GetEdge2() const { return m_avVertex[2] - m_avVertex[1]; }
};



// ================================ inline implementations ================================ 


inline void CTriangle::GetSpan( float& rfMin, float& rfMax, Vector3 vAxis ) const
{
	Scalar d0, d1, d2;
	d0 = Vec3Dot( m_avVertex[0], vAxis );
	d1 = Vec3Dot( m_avVertex[1], vAxis );
	d2 = Vec3Dot( m_avVertex[2], vAxis );
	rfMin = TakeMin( d0, d1, d2 );
	rfMax = TakeMax( d0, d1, d2 );
}


inline bool CTriangle::RayIntersect( const Vector3& vStart, Vector3& vGoal )
{
	float d0, d1;
	d0 = Vec3Dot( m_vNormal, vStart ) - m_fDist;
	d1 = Vec3Dot( m_vNormal, vGoal  ) - m_fDist;
	if( (0 < d0 && 0 < d1) || (d0 < 0 && d1 < 0) )
		return false;	// no intersection

	// calculate the point of intersection on the plane
	Vector3 vOnPlanePos = vStart + (vGoal - vStart) * d0 / (d0 - d1);

	// Now, we know that the line segment is crossing the plane of the polygon.
	// check if it is intersecting with the polygon
	Vector3 vLocalPointPos;
	Vector3 vCross[3];
	vLocalPointPos = vOnPlanePos - GetVertex(0);
	Vec3Cross( vCross[0], GetEdge(0), vLocalPointPos );

	vLocalPointPos = vOnPlanePos - GetVertex(1);
	Vec3Cross( vCross[1], GetEdge(1), vLocalPointPos );

	if( Vec3Dot(vCross[0], vCross[1]) <= 0 )
		return false;	// not intersecting with this triangle

	vLocalPointPos = vOnPlanePos - GetVertex(2);
	Vec3Cross( vCross[2], GetEdge(2), vLocalPointPos );

	if( Vec3Dot(vCross[1], vCross[2]) <= 0 )
		return false;	// not intersecting with this triangle

	if( Vec3Dot(vCross[2], vCross[0]) <= 0 )
		return false;	// not intersecting with this triangle

	// found intersection - replace 'vGoal' with the contact point
	vGoal = vOnPlanePos;

	return true;
}


#endif  /*  __TRIANGLE_H__  */
