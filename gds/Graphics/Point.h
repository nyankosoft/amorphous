
#ifndef  __GRAPHICS_POINT_H__
#define  __GRAPHICS_POINT_H__

namespace Graphics
{

/**
 * holds a point in 2D coordinates
 */
struct SPoint
{
	int x, y;

	SPoint() {}
	SPoint( int ix, int iy ) : x(ix), y(iy) {}

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
 

}  /*  Graphics  */


#endif  /*  __GRAPHICS_POINT_H__  */
