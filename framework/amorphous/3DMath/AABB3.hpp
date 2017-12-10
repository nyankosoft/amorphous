#ifndef  __AXISALIGNEDBOUNDINGBOX_3D_H__
#define  __AXISALIGNEDBOUNDINGBOX_3D_H__


#include "Plane.hpp"
#include "Sphere.hpp"


namespace amorphous
{


/// Axis-Aligned Bounding Box (3D)
class AABB3
{
public:

	Vector3 vMin;
	Vector3 vMax;

public:

	inline AABB3() : vMin(Vector3(0,0,0)), vMax(Vector3(0,0,0)) {}
	inline AABB3( const Vector3& min, const Vector3& max ) : vMin(min), vMax(max) {}

	inline bool operator==(const AABB3& other) const;

	inline void Nullify();

	inline void SetMaxAndMin( const Vector3& vMax, const Vector3& vMin ) { this->vMax = vMax; this->vMin = vMin; }

	inline Vector3 GetCenterPosition() const { return (vMax + vMin) / 2.0f; }

	/// \return The radii along the axes, i.e. ( width/2, height/2, depth/2 ).
	inline Vector3 GetExtents() const { return ( vMax - vMin ) / 2.0f; }

	inline void AddPoint( const Vector3& v );

	inline void AddSphere( const Sphere& rSphere );

	inline void Merge2AABBs( const AABB3& b0, const AABB3& b1);
	inline void MergeAABB( const AABB3& b );	// add another aabb

	inline void operator+=( const AABB3& aabb ) { MergeAABB(aabb); }

	inline float GetVolume() const { return (vMax.x - vMin.x) * (vMax.y - vMin.y) * (vMax.z - vMin.z); }

	inline bool IsIntersectingWith( const AABB3& raabb ) const;
	inline bool IsPointInside( const Vector3& rvPoint ) const;

	inline float GetRadiusForPlane( const SPlane& rPlane ) const;

	inline void TransformCoord( const AABB3& raabb, const Vector3& rvOrigin );

	/// \return A sphere that contains aabb
	Sphere CreateBoundingSphere() const;
};


// ================================ inline implementations ================================

inline bool AABB3::operator==(const AABB3& other) const
{
	return (vMin == other.vMin) && (vMax == other.vMax);
}

inline void AABB3::AddPoint( const Vector3& v )
{
	if( v.x < vMin.x ) vMin.x = v.x;
	if( vMax.x < v.x ) vMax.x = v.x;

	if( v.y < vMin.y ) vMin.y = v.y;
	if( vMax.y < v.y ) vMax.y = v.y;

	if( v.z < vMin.z ) vMin.z = v.z;
	if( vMax.z < v.z ) vMax.z = v.z;
}


inline void AABB3::AddSphere( const Sphere& rSphere )
{
	AABB3 aabb;
	aabb.vMin = rSphere.center - Vector3( rSphere.radius, rSphere.radius, rSphere.radius );
	aabb.vMax = rSphere.center + Vector3( rSphere.radius, rSphere.radius, rSphere.radius );
	MergeAABB( aabb );
}


inline bool AABB3::IsIntersectingWith( const AABB3& raabb ) const
{
	if( vMax.x < raabb.vMin.x || raabb.vMax.x < vMin.x ||
		vMax.y < raabb.vMin.y || raabb.vMax.y < vMin.y ||
		vMax.z < raabb.vMin.z || raabb.vMax.z < vMin.z )
		return false;
	else
		return true;
}


inline bool AABB3::IsPointInside( const Vector3& rvPoint ) const
{
	if( vMin.x <= rvPoint.x && rvPoint.x <= vMax.x && 
		vMin.y <= rvPoint.y && rvPoint.y <= vMax.y &&
		vMin.z <= rvPoint.z && rvPoint.z <= vMax.z )
		return true;
	else
		return false;
}


inline void AABB3::Merge2AABBs( const AABB3& b0, const AABB3& b1)
{
	vMax.x = b0.vMax.x > b1.vMax.x ? b0.vMax.x : b1.vMax.x;
	vMax.y = b0.vMax.y > b1.vMax.y ? b0.vMax.y : b1.vMax.y;
	vMax.z = b0.vMax.z > b1.vMax.z ? b0.vMax.z : b1.vMax.z;

	vMin.x = b0.vMin.x < b1.vMin.x ? b0.vMin.x : b1.vMin.x;
	vMin.y = b0.vMin.y < b1.vMin.y ? b0.vMin.y : b1.vMin.y;
	vMin.z = b0.vMin.z < b1.vMin.z ? b0.vMin.z : b1.vMin.z;
}


// merge this aabb and another aabb
inline void AABB3::MergeAABB( const AABB3& b )
{
	vMax.x = vMax.x > b.vMax.x ? vMax.x : b.vMax.x;
	vMax.y = vMax.y > b.vMax.y ? vMax.y : b.vMax.y;
	vMax.z = vMax.z > b.vMax.z ? vMax.z : b.vMax.z;

	vMin.x = vMin.x < b.vMin.x ? vMin.x : b.vMin.x;
	vMin.y = vMin.y < b.vMin.y ? vMin.y : b.vMin.y;
	vMin.z = vMin.z < b.vMin.z ? vMin.z : b.vMin.z;
}


inline void AABB3::Nullify()
{
	vMax = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	vMin = Vector3( FLT_MAX,  FLT_MAX,  FLT_MAX);
}


inline void AABB3::TransformCoord( const AABB3& raabb, const Vector3& rvOrigin )
{
	vMax = rvOrigin + raabb.vMax;
	vMin = rvOrigin + raabb.vMin;
}


inline float AABB3::GetRadiusForPlane( const SPlane& rPlane ) const
{
	switch(rPlane.type)
	{
	case 0: return ( vMax.x - vMin.x ) / 2.0f;	//plane is perpendicular to x-axis
	case 1: return ( vMax.y - vMin.y ) / 2.0f;	//plane is perpendicular to y-axis
	case 2: return ( vMax.z - vMin.z ) / 2.0f;	//plane is perpendicular to z-axis
//	case 3: return ( vMax.x - vMin.x ) / 2.0f;	//vertical wall (y == 0)			/* actually, not desirable solution */
	default:
		Vector3 vExtents = GetExtents();
		return  vExtents.x * fabsf(rPlane.normal.x)
			  + vExtents.y * fabsf(rPlane.normal.y)
		   	  + vExtents.z * fabsf(rPlane.normal.z);
	}

/*	if( rPlane.type < 3 )
		return ( vMax[rPlane.type] - vMin[rPlane.type] ) / 2.0f;
	else
	{	
		Vector3 vExtents = GetExtents();
		return vExtents.x * fabs(rPlane.normal.x)
			+ vExtents.y * fabs(rPlane.normal.y)
			+ vExtents.z * fabs(rPlane.normal.z);
	}*/
}


} // namespace amorphous



#endif		/*  __AXISALIGNEDBOUNDINGBOX_H__  */
