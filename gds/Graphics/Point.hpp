#ifndef  __GRAPHICS_POINT_H__
#define  __GRAPHICS_POINT_H__


#include "../3DMath/Vector2_Gen.hpp"


namespace amorphous
{


// TVector2 for interger type
typedef TVector2<int> SPoint;

/**
 * holds a point in 2D coordinates
 */
/*
struct SPoint
{
	int x, y;

	SPoint() {}
	SPoint( int ix, int iy ) : x(ix), y(iy) {}

	inline SPoint operator -( const SPoint pnt ) { 

	inline SPoint operator *( float factor );
	inline SPoint operator *=( float factor );
};


inline SPoint SPoint::operator *( float factor )
{
	return SPoint( (int)(x * factor), (int)(y * factor) );
}


inline SPoint SPoint::operator *=( float factor )
{
	x = (int)(x * factor);
	y = (int)(y * factor);
	return *this;
}
*/

} // namespace amorphous


#endif  /*  __GRAPHICS_POINT_H__  */
