#ifndef __FLOATRGBACOLOR_H__
#define __FLOATRGBACOLOR_H__


#include "FloatRGBColor.h"


/**
 floating point r,g,b,a color
 - range of each component is usually [ 0.0, 1.0 ]
 - default color: opaque black (0,0,0,1)
*/
struct SFloatRGBAColor
{
	float fRed;
	float fGreen;
	float fBlue;
	float fAlpha;

	inline SFloatRGBAColor operator *( const float f) const;
	inline SFloatRGBAColor operator /( const float f) const;
	inline SFloatRGBAColor operator +( const SFloatRGBAColor& rColor) const;
	inline SFloatRGBAColor operator -( const SFloatRGBAColor& rColor) const;
	inline SFloatRGBAColor operator +=( const SFloatRGBAColor& rColor);
	inline SFloatRGBAColor  operator -=( const SFloatRGBAColor& rColor);

	inline SFloatRGBAColor operator *(const SFloatRGBAColor& rColor) const;

	inline SFloatRGBAColor();

	inline SFloatRGBAColor( float r, float g, float b, float a );

	inline void SetRGBA( float r, float g, float b, float a );

	inline void SetRGBA( double r, double g, double b, double a );

	/// sets (r,g,b) components
	/// - NOTE: does NOT change alpha component
	inline void SetRGB( float r, float g, float b );

	inline SFloatRGBColor GetRGBColor() const;

	inline U32 GetARGB32() const;

	/// set color expressed as 32-bit integer with 8-bit for each component
	/// \param [in] 32-bit integer that represents ARGB color using 8 bits for each component
	inline void SetARGB32( U32 color );

	// deprecated
//	void CopyFromD3DCOLOR(D3DCOLOR color);
//	D3DCOLOR ConvertToD3DCOLOR();
};


#include "FloatRGBAColor.inl"


#endif  /*  __FLOATRGBACOLOR_H__  */
