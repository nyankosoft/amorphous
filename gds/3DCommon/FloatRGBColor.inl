
//================================================================================
// SFloatRGBColor::Operators()                                   - SFloatRGBColor
//================================================================================

inline SFloatRGBColor::SFloatRGBColor()
: fRed(0), fGreen(0), fBlue(0)
{
//	fRed = fGreen = fBlue = 0.0f;
}

inline SFloatRGBColor::SFloatRGBColor( float r, float g, float b )
: fRed(r), fGreen(g), fBlue(b)
{
}


inline void SFloatRGBColor::SetRGB( float r, float g, float b )
{
	fRed   = r;
	fGreen = g;
	fBlue  = b;
}


inline void SFloatRGBColor::SetBlack()
{
	fRed = fGreen = fBlue = 0.0f;
}


inline SFloatRGBColor SFloatRGBColor::operator *(const float f) const
{
	SFloatRGBColor color;
	color.fRed   = this->fRed * f;
	color.fGreen = this->fGreen * f;
	color.fBlue  = this->fBlue * f;
	return color;
}

inline SFloatRGBColor SFloatRGBColor::operator /(const float f) const
{
	SFloatRGBColor color;
	color.fRed   = this->fRed / f;
	color.fGreen = this->fGreen / f;
	color.fBlue  = this->fBlue / f;
	return color;
}

inline SFloatRGBColor SFloatRGBColor::operator +(const SFloatRGBColor& rColor) const
{
	SFloatRGBColor color;
	color.fRed   = this->fRed + rColor.fRed;
	color.fGreen = this->fGreen + rColor.fGreen;
	color.fBlue  = this->fBlue + rColor.fBlue;
	return color;
}

inline SFloatRGBColor SFloatRGBColor::operator -(const SFloatRGBColor& rColor) const
{
	SFloatRGBColor color;
	color.fRed   = this->fRed - rColor.fRed;
	color.fGreen = this->fGreen - rColor.fGreen;
	color.fBlue  = this->fBlue - rColor.fBlue;
	return color;
}

inline SFloatRGBColor SFloatRGBColor::operator +=(const SFloatRGBColor& rColor)
{
	this->fRed += rColor.fRed;
	this->fGreen += rColor.fGreen;
	this->fBlue += rColor.fBlue;
	return *this;
}

inline SFloatRGBColor SFloatRGBColor::operator -=(const SFloatRGBColor& rColor)
{
	this->fRed -= rColor.fRed;
	this->fGreen -= rColor.fGreen;
	this->fBlue -= rColor.fBlue;
	return *this;
}


inline SFloatRGBColor SFloatRGBColor::operator *(const SFloatRGBColor& rColor) const
{
	return SFloatRGBColor(
		this->fRed   * rColor.fRed,
		this->fGreen * rColor.fGreen,
		this->fBlue  * rColor.fBlue
		);
}


/*
inline void SFloatRGBColor::CopyFromD3DCOLOR(D3DCOLOR color)
{
	this->fRed   = (float)((color & 0x00FF0000) >> 16) / 255.0f;
	this->fGreen = (float)((color & 0x0000FF00) >> 8) / 255.0f;
	this->fBlue  = (float)(color & 0x000000FF) / 255.0f;
}

inline D3DCOLOR SFloatRGBColor::ConvertToD3DCOLOR()
{
	return D3DCOLOR_ARGB( 0, (int)(fRed * 255.0), (int)(fGreen * 255.0f), (int)(fBlue * 255.0f) );

}
*/


inline unsigned long SFloatRGBColor::GetARGB32() const
{
	return /*( ((int)(fAlpha * 255.0f)) << 24 ) &*/ 0xFF000000 |
		   ( ((int)(fRed   * 255.0f)) << 16 ) & 0x00FF0000 |
		   ( ((int)(fGreen * 255.0f)) <<  8 ) & 0x0000FF00 |
		     ((int)(fBlue  * 255.0f))         & 0x000000FF;
}


inline void SFloatRGBColor::SetARGB32( unsigned long color )
{
//	fAlpha	= (float)( (color >> 24) & 0x000000FF ) / 256.0f;
	fRed	= (float)( (color >> 16) & 0x000000FF ) / 256.0f;
	fGreen	= (float)( (color >>  8) & 0x000000FF ) / 256.0f;
	fBlue	= (float)( (color)       & 0x000000FF ) / 256.0f;
}
