#ifndef __FLOATRGBCOLOR_H__
#define __FLOATRGBCOLOR_H__


#include "../base.h"


struct SFloatRGBColor
{
	float fRed;
	float fGreen;
	float fBlue;

	inline SFloatRGBColor operator *(const float f) const;
	inline SFloatRGBColor operator /(const float f) const;
	inline SFloatRGBColor operator +(const SFloatRGBColor& rColor) const;
	inline SFloatRGBColor operator -(const SFloatRGBColor& rColor) const;
	inline SFloatRGBColor operator +=(const SFloatRGBColor& rColor);
	inline SFloatRGBColor operator -=(const SFloatRGBColor& rColor);

	/// do multiplication operations for each component
	/// - same as '*' in programmable shader
	inline SFloatRGBColor operator *(const SFloatRGBColor& rColor) const;

//	inline void CopyFromD3DCOLOR(D3DCOLOR color);
	/// set color expressed as 32-bit integer with 8-bit for each component
	inline void SetARGB32( U32 color );

	/// alpha is set to 255(0xFF)
	inline U32 GetARGB32() const;

	inline U8 GetRedByte()   const { return (U8)( ( ((int)(fRed   * 255.0f)) << 16 ) & 0x00FF0000 ); }
	inline U8 GetGreenByte() const { return (U8)( ( ((int)(fGreen * 255.0f)) <<  8 ) & 0x0000FF00 ); }
	inline U8 GetBlueByte()  const { return (U8)(   ((int)(fBlue  * 255.0f))         & 0x000000FF ); }

	inline SFloatRGBColor();

	inline SFloatRGBColor( float r, float g, float b );

	inline void SetRGB( float r, float g, float b );

	void SetBlack();
};


#include "FloatRGBColor.inl"


#endif  /*  __FLOATRGBCOLOR_H__  */
