#ifndef  __AXISALIGNEDBOUNDINGBOX_2D_H__
#define  __AXISALIGNEDBOUNDINGBOX_2D_H__


#include <float.h>
#include <string.h> // for memset()
#include "Vector2.hpp"


namespace amorphous
{


/// Axis-Aligned Bounding Box (2D)
class AABB2
{
public:

	Vector2 vMin;
	Vector2 vMax;

public:

	AABB2() { memset(this, 0, sizeof(AABB2)); }

	inline AABB2( const Vector2& min, const Vector2& max ) : vMin(min), vMax(max) {}

	inline void Nullify();

	inline void SetMaxAndMin(Vector2& vMax, Vector2& vMin) { this->vMax = vMax; this->vMin = vMin; }

	inline Vector2 GetCenterPosition() const { return (vMax + vMin) / 2.0f; }
	inline Vector2 GetExtents() const { return ( vMax - vMin ) / 2.0f; }

	inline void AddPoint( const Vector2& v );

	inline void Merge2AABBs( const AABB2& b0, const AABB2& b1 );
	inline void MergeAABB( const AABB2& b );	// add another aabb

	inline void operator+=( const AABB2& aabb ) { MergeAABB(aabb); }

	inline float GetArea() const { return (vMax.x - vMin.x) * (vMax.y - vMin.y); }

	inline bool IsIntersectingWith( const AABB2& raabb ) const;
	inline bool IsPointInside( const Vector2& rvPoint ) const;

};


// ================================ inline implementations ================================ 

inline void AABB2::AddPoint( const Vector2& v )
{
	if( v.x < vMin.x ) vMin.x = v.x;
	if( vMax.x < v.x ) vMax.x = v.x;

	if( v.y < vMin.y ) vMin.y = v.y;
	if( vMax.y < v.y ) vMax.y = v.y;
}


inline bool AABB2::IsIntersectingWith( const AABB2& raabb ) const
{
	if( vMax.x < raabb.vMin.x || raabb.vMax.x < vMin.x ||
		vMax.y < raabb.vMin.y || raabb.vMax.y < vMin.y )
		return false;
	else
		return true;
}


inline bool AABB2::IsPointInside( const Vector2& rvPoint ) const
{
	if( vMin.x <= rvPoint.x && rvPoint.x <= vMax.x && 
		vMin.y <= rvPoint.y && rvPoint.y <= vMax.y )
		return true;
	else
		return false;
}


inline void AABB2::Merge2AABBs( const AABB2& b0,  const AABB2& b1 )
{
	vMax.x = b0.vMax.x > b1.vMax.x ? b0.vMax.x : b1.vMax.x;
	vMax.y = b0.vMax.y > b1.vMax.y ? b0.vMax.y : b1.vMax.y;

	vMin.x = b0.vMin.x < b1.vMin.x ? b0.vMin.x : b1.vMin.x;
	vMin.y = b0.vMin.y < b1.vMin.y ? b0.vMin.y : b1.vMin.y;
}


// merge this aabb and another aabb
inline void AABB2::MergeAABB( const AABB2& b )
{
	vMax.x = vMax.x > b.vMax.x ? vMax.x : b.vMax.x;
	vMax.y = vMax.y > b.vMax.y ? vMax.y : b.vMax.y;

	vMin.x = vMin.x < b.vMin.x ? vMin.x : b.vMin.x;
	vMin.y = vMin.y < b.vMin.y ? vMin.y : b.vMin.y;
}


inline void AABB2::Nullify()
{
	vMax = Vector2(-FLT_MAX, -FLT_MAX);
	vMin = Vector2( FLT_MAX,  FLT_MAX);
}


} // namespace amorphous



#endif		/*  __AXISALIGNEDBOUNDINGBOX_2D_H__  */
