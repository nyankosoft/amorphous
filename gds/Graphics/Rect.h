#ifndef  __GRAPHICS_RECT_H__
#define  __GRAPHICS_RECT_H__


#include "Point.h"


namespace Graphics
{


struct SRect
{
	int left;
	int top;
	int right;
	int bottom;

	SRect() { SetPositionLTRB(0,0,0,0); }

	/// takes left, top, right, and bottom
	SRect( int l, int t, int r, int b ) { SetPositionLTRB(l,t,r,b); }

	/// takes left, top, right, and bottom
	SRect( float l, float t, float r, float b ) { SetPositionLTRB(l,t,r,b); }

	void SetValues( int l, int t, int r, int b) { left = l; top = t; right = r; bottom = b; }

	void SetPositionLTWH( int _left, int _top, int width, int height )   { left = _left; top = _top; right = _left + width - 1; bottom = _top + height - 1; }
	void SetPositionLTRB( int _left, int _top, int _right, int _bottom ) { left = _left; top = _top; right = _right; bottom = _bottom; }

	void SetPositionLTWH( float _left, float _top, float width, float height )   { left = (int)_left; top = (int)_top; right = (int)(_left + width - 1); bottom = (int)(_top + height - 1); }
	void SetPositionLTRB( float _left, float _top, float _right, float _bottom ) { left = (int)_left; top = (int)_top; right = (int)_right; bottom = (int)_bottom; }

	int GetWidth() const { return right - left + 1; }
	int GetHeight() const { return bottom - top + 1; }

	inline bool ContainsPoint( SPoint& pt ) const;

	inline void Offset( int dx, int dy );

	inline void Inflate( int dx, int dy );

	/// gives the same result as '*=' operator
	/// the caller instance updates itself with the results
	inline void Scale( float factor );

	inline SRect operator *( float factor ) const;

	inline SRect operator *=( float factor );

	inline SRect operator /( float factor ) const;

	inline SRect operator /=( float factor );
};



//============================= inline implementations =============================

inline bool SRect::ContainsPoint( SPoint& pt ) const
{
	if( left <= pt.x && pt.x <= right && top <= pt.y && pt.y <= bottom )
		return true;
	else
		return false;
}


inline void SRect::Offset( int dx, int dy )
{
	left   += dx;
	top    += dy;
	right  += dx;
	bottom += dy;
}


inline void SRect::Inflate( int dx, int dy )
{
	left   -= dx;
	top    -= dy;
	right  += dx;
	bottom += dy;
}


inline void SRect::Scale( float factor )
{
	left   = (int)(left   * factor);
	top    = (int)(top    * factor);
	right  = (int)(right  * factor);
	bottom = (int)(bottom * factor);
}


inline SRect SRect::operator *( float factor ) const
{
	return SRect( (int)(left   * factor),
		          (int)(top    * factor),
                  (int)(right  * factor),
                  (int)(bottom * factor) );
}


inline SRect SRect::operator *=( float factor )
{
	Scale( factor );
	return *this;
}


inline SRect SRect::operator /( float factor ) const
{
	return SRect( (int)(left   / factor),
		          (int)(top    / factor),
                  (int)(right  / factor),
                  (int)(bottom / factor) );
}


inline SRect SRect::operator /=( float factor )
{
	Scale( 1.0f / factor );
	return *this;
}


inline SRect RectLTWH( int left, int top, int width, int height )
{
	return SRect( left, top, left + width - 1, top + height - 1 );
}


inline SRect RectLTRB( int left, int top, int right, int bottom )
{
	return SRect( left, top, right, bottom );
}


inline SRect RectCWH( int center_x, int center_y, int width, int height )
{
	return RectLTWH( center_x - width/2, center_y - height/2, width, height );
}


inline SRect RectCWH( const SPoint& center, int width, int height )
{
	return RectLTWH( center.x - width/2, center.y - height/2, width, height );
}


}  /*  Graphics  */


#endif  /*  __GRAPHICS_RECT_H__  */
