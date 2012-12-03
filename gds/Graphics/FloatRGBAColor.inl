
//
// global functions
//

inline SFloatRGBAColor operator*( const float f, const SFloatRGBAColor& src )
{
//	return SFloatRGBAColor( src.red * f, src.green * f, src.blue * f );
	return SFloatRGBAColor( src ) * f;
}


//===================================================================================
// SFloatRGBAColor::Operators()                                   - SFloatRGBAColor
//===================================================================================


inline SFloatRGBAColor::SFloatRGBAColor()
: red(0.0f), green(0.0f), blue(0.0f), alpha(1.0f)
{
}


inline SFloatRGBAColor::SFloatRGBAColor( float r, float g, float b, float a )
: red(r), green(g), blue(b), alpha(a)
{
//	SetRGBA( r, g, b, a );
}


inline void SFloatRGBAColor::SetRGBA( float r, float g, float b, float a )
{
	red   = r;
	green = g;
	blue  = b;
	alpha = a;
}


inline void SFloatRGBAColor::SetRGB( float r, float g, float b )
{
	red   = r;
	green = g;
	blue  = b;
}


inline void SFloatRGBAColor::SetRGBA( double r, double g, double b, double a )
{
	SFloatRGBAColor( (float)r, (float)g, (float)b, (float)a );
}


inline SFloatRGBColor SFloatRGBAColor::GetRGBColor() const
{
	return SFloatRGBColor( red, green, blue );
}


inline U32 SFloatRGBAColor::GetARGB32() const
{
	return ( ((int)(alpha * 255.0f)) << 24 ) & 0xFF000000 |
		   ( ((int)(red   * 255.0f)) << 16 ) & 0x00FF0000 |
		   ( ((int)(green * 255.0f)) <<  8 ) & 0x0000FF00 |
		     ((int)(blue  * 255.0f))         & 0x000000FF;
}


inline void SFloatRGBAColor::SetARGB32( U32 color )
{
	alpha	= (float)( (color >> 24) & 0x000000FF ) / 256.0f;
	red	= (float)( (color >> 16) & 0x000000FF ) / 256.0f;
	green	= (float)( (color >>  8) & 0x000000FF ) / 256.0f;
	blue	= (float)( (color)       & 0x000000FF ) / 256.0f;
}


inline SFloatRGBAColor SFloatRGBAColor::operator *( const float f) const
{
	SFloatRGBAColor color;
	color.alpha = this->alpha * f;
	color.red   = this->red   * f;
	color.green = this->green * f;
	color.blue  = this->blue  * f;
	return color;
}


inline SFloatRGBAColor SFloatRGBAColor::operator /( const float f) const
{
	SFloatRGBAColor color;
	color.alpha = this->alpha / f;
	color.red   = this->red   / f;
	color.green = this->green / f;
	color.blue  = this->blue  / f;
	return color;
}


inline SFloatRGBAColor SFloatRGBAColor::operator +( const SFloatRGBAColor& rColor) const
{
	SFloatRGBAColor color;
	color.alpha = this->alpha + rColor.alpha;
	color.red   = this->red   + rColor.red;
	color.green = this->green + rColor.green;
	color.blue  = this->blue  + rColor.blue;
	return color;
}


inline SFloatRGBAColor SFloatRGBAColor::operator -( const SFloatRGBAColor& rColor) const
{
	SFloatRGBAColor color;
	color.alpha = this->alpha - rColor.alpha;
	color.red   = this->red   - rColor.red;
	color.green = this->green - rColor.green;
	color.blue  = this->blue  - rColor.blue;
	return color;
}


inline SFloatRGBAColor SFloatRGBAColor::operator +=( const SFloatRGBAColor& rColor)
{
	this->alpha += rColor.alpha;
	this->red   += rColor.red;
	this->green += rColor.green;
	this->blue  += rColor.blue;
	return *this;
}


inline SFloatRGBAColor SFloatRGBAColor::operator -=( const SFloatRGBAColor& rColor)
{
	this->alpha -= rColor.alpha;
	this->red   -= rColor.red;
	this->green -= rColor.green;
	this->blue  -= rColor.blue;
	return *this;
}


inline SFloatRGBAColor SFloatRGBAColor::operator *(const SFloatRGBAColor& rColor) const
{
	return SFloatRGBAColor(
		this->red   * rColor.red,
		this->green * rColor.green,
		this->blue  * rColor.blue,
		this->alpha * rColor.alpha
		);
}


/*
void SFloatRGBAColor::CopyFromD3DCOLOR(D3DCOLOR color)
{
	this->alpha = (float)((color & 0xFF000000) >> 24) / 255.0f;
	this->red   = (float)((color & 0x00FF0000) >> 16) / 255.0f;
	this->green = (float)((color & 0x0000FF00) >> 8) / 255.0f;
	this->blue  = (float)(color & 0x000000FF) / 255.0f;
}

D3DCOLOR SFloatRGBAColor::ConvertToD3DCOLOR()
{
	return D3DCOLOR_ARGB( (int)(alpha * 255.0), (int)(red * 255.0), (int)(green * 255.0f), (int)(blue * 255.0f) );

}*/
