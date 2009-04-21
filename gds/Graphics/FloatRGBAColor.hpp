#ifndef __FLOATRGBACOLOR_H__
#define __FLOATRGBACOLOR_H__


#include "FloatRGBColor.hpp"


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

	void SetToWhite()   { *this = White(); }
	void SetToBlack()   { *this = Black(); }
	void SetToRed()     { *this = Red(); }
	void SetToGreen()   { *this = Green(); }
	void SetToBlue()    { *this = Blue(); }
	void SetToYellow()  { *this = Yellow(); }
	void SetToMagenta() { *this = Magenta(); }
	void SetToAqua()    { *this = Aqua(); }

	static const SFloatRGBAColor White()   { return SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f ); }
	static const SFloatRGBAColor Black()   { return SFloatRGBAColor( 0.0f, 0.0f, 0.0f, 1.0f ); }
	static const SFloatRGBAColor Red()     { return SFloatRGBAColor( 1.0f, 0.0f, 0.0f, 1.0f ); }
	static const SFloatRGBAColor Green()   { return SFloatRGBAColor( 0.0f, 1.0f, 0.0f, 1.0f ); }
	static const SFloatRGBAColor Blue()    { return SFloatRGBAColor( 0.0f, 0.0f, 1.0f, 1.0f ); }
	static const SFloatRGBAColor Yellow()  { return SFloatRGBAColor( 1.0f, 1.0f, 0.0f, 1.0f ); }
	static const SFloatRGBAColor Magenta() { return SFloatRGBAColor( 1.0f, 0.0f, 1.0f, 1.0f ); }
	static const SFloatRGBAColor Aqua()    { return SFloatRGBAColor( 0.0f, 1.0f, 1.0f, 1.0f ); }
};


#include "FloatRGBAColor.inl"


#endif  /*  __FLOATRGBACOLOR_H__  */
