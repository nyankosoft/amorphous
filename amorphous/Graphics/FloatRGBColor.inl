namespace amorphous
{

//
// global functions
//

inline SFloatRGBColor operator*( const float f, const SFloatRGBColor& src )
{
//	return SFloatRGBColor( src.red * f, src.green * f, src.blue * f );
	return SFloatRGBColor( src ) * f;
}


//================================================================================
// SFloatRGBColor::Operators()                                   - SFloatRGBColor
//================================================================================

inline SFloatRGBColor::SFloatRGBColor()
: red(0), green(0), blue(0)
{
//	red = green = blue = 0.0f;
}

inline SFloatRGBColor::SFloatRGBColor( float r, float g, float b )
: red(r), green(g), blue(b)
{
}


inline void SFloatRGBColor::SetRGB( float r, float g, float b )
{
	red   = r;
	green = g;
	blue  = b;
}


inline SFloatRGBColor SFloatRGBColor::operator *(const float f) const
{
	SFloatRGBColor color;
	color.red   = this->red * f;
	color.green = this->green * f;
	color.blue  = this->blue * f;
	return color;
}

inline SFloatRGBColor SFloatRGBColor::operator /(const float f) const
{
	SFloatRGBColor color;
	color.red   = this->red / f;
	color.green = this->green / f;
	color.blue  = this->blue / f;
	return color;
}

inline SFloatRGBColor SFloatRGBColor::operator +(const SFloatRGBColor& rColor) const
{
	SFloatRGBColor color;
	color.red   = this->red + rColor.red;
	color.green = this->green + rColor.green;
	color.blue  = this->blue + rColor.blue;
	return color;
}

inline SFloatRGBColor SFloatRGBColor::operator -(const SFloatRGBColor& rColor) const
{
	SFloatRGBColor color;
	color.red   = this->red - rColor.red;
	color.green = this->green - rColor.green;
	color.blue  = this->blue - rColor.blue;
	return color;
}

inline SFloatRGBColor SFloatRGBColor::operator +=(const SFloatRGBColor& rColor)
{
	this->red += rColor.red;
	this->green += rColor.green;
	this->blue += rColor.blue;
	return *this;
}

inline SFloatRGBColor SFloatRGBColor::operator -=(const SFloatRGBColor& rColor)
{
	this->red -= rColor.red;
	this->green -= rColor.green;
	this->blue -= rColor.blue;
	return *this;
}


bool SFloatRGBColor::operator==( const SFloatRGBColor& rhs ) const
{
	return( red   == rhs.red
		 && green == rhs.green
		 && blue  == rhs.blue );
}


bool SFloatRGBColor::operator!=( const SFloatRGBColor& rhs ) const
{
	return !(*this == rhs);
}


inline SFloatRGBColor SFloatRGBColor::operator *(const SFloatRGBColor& rColor) const
{
	return SFloatRGBColor(
		this->red   * rColor.red,
		this->green * rColor.green,
		this->blue  * rColor.blue
		);
}


inline U32 SFloatRGBColor::GetARGB32() const
{
	return /*( ((int)(alpha * 255.0f)) << 24 ) &*/ 0xFF000000 |
		   ( ((int)(red   * 255.0f)) << 16 ) & 0x00FF0000 |
		   ( ((int)(green * 255.0f)) <<  8 ) & 0x0000FF00 |
		     ((int)(blue  * 255.0f))         & 0x000000FF;
}


inline void SFloatRGBColor::SetARGB32( U32 color )
{
//	alpha	= (float)( (color >> 24) & 0x000000FF ) / 256.0f;
	red	= (float)( (color >> 16) & 0x000000FF ) / 256.0f;
	green	= (float)( (color >>  8) & 0x000000FF ) / 256.0f;
	blue	= (float)( (color)       & 0x000000FF ) / 256.0f;
}

} // namespace amorphous
