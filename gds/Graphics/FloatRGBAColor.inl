
//
// global functions
//

inline SFloatRGBAColor operator*( const float f, const SFloatRGBAColor& src )
{
//	return SFloatRGBAColor( src.fRed * f, src.fGreen * f, src.fBlue * f );
	return SFloatRGBAColor( src ) * f;
}


//===================================================================================
// SFloatRGBAColor::Operators()                                   - SFloatRGBAColor
//===================================================================================


inline SFloatRGBAColor::SFloatRGBAColor()
: fRed(0.0f), fGreen(0.0f), fBlue(0.0f), fAlpha(1.0f)
{
}


inline SFloatRGBAColor::SFloatRGBAColor( float r, float g, float b, float a )
: fRed(r), fGreen(g), fBlue(b), fAlpha(a)
{
//	SetRGBA( r, g, b, a );
}


inline void SFloatRGBAColor::SetRGBA( float r, float g, float b, float a )
{
	fRed   = r;
	fGreen = g;
	fBlue  = b;
	fAlpha = a;
}


inline void SFloatRGBAColor::SetRGB( float r, float g, float b )
{
	fRed   = r;
	fGreen = g;
	fBlue  = b;
}


inline void SFloatRGBAColor::SetRGBA( double r, double g, double b, double a )
{
	SFloatRGBAColor( (float)r, (float)g, (float)b, (float)a );
}


inline SFloatRGBColor SFloatRGBAColor::GetRGBColor() const
{
	return SFloatRGBColor( fRed, fGreen, fBlue );
}


inline unsigned long SFloatRGBAColor::GetARGB32() const
{
	return ( ((int)(fAlpha * 255.0f)) << 24 ) & 0xFF000000 |
		   ( ((int)(fRed   * 255.0f)) << 16 ) & 0x00FF0000 |
		   ( ((int)(fGreen * 255.0f)) <<  8 ) & 0x0000FF00 |
		     ((int)(fBlue  * 255.0f))         & 0x000000FF;
}


inline void SFloatRGBAColor::SetARGB32( unsigned long color )
{
	fAlpha	= (float)( (color >> 24) & 0x000000FF ) / 256.0f;
	fRed	= (float)( (color >> 16) & 0x000000FF ) / 256.0f;
	fGreen	= (float)( (color >>  8) & 0x000000FF ) / 256.0f;
	fBlue	= (float)( (color)       & 0x000000FF ) / 256.0f;
}


inline SFloatRGBAColor SFloatRGBAColor::operator *( const float f) const
{
	SFloatRGBAColor color;
	color.fAlpha = this->fAlpha * f;
	color.fRed   = this->fRed   * f;
	color.fGreen = this->fGreen * f;
	color.fBlue  = this->fBlue  * f;
	return color;
}


inline SFloatRGBAColor SFloatRGBAColor::operator /( const float f) const
{
	SFloatRGBAColor color;
	color.fAlpha = this->fAlpha / f;
	color.fRed   = this->fRed   / f;
	color.fGreen = this->fGreen / f;
	color.fBlue  = this->fBlue  / f;
	return color;
}


inline SFloatRGBAColor SFloatRGBAColor::operator +( const SFloatRGBAColor& rColor) const
{
	SFloatRGBAColor color;
	color.fAlpha = this->fAlpha + rColor.fAlpha;
	color.fRed   = this->fRed   + rColor.fRed;
	color.fGreen = this->fGreen + rColor.fGreen;
	color.fBlue  = this->fBlue  + rColor.fBlue;
	return color;
}


inline SFloatRGBAColor SFloatRGBAColor::operator -( const SFloatRGBAColor& rColor) const
{
	SFloatRGBAColor color;
	color.fAlpha = this->fAlpha - rColor.fAlpha;
	color.fRed   = this->fRed   - rColor.fRed;
	color.fGreen = this->fGreen - rColor.fGreen;
	color.fBlue  = this->fBlue  - rColor.fBlue;
	return color;
}


inline SFloatRGBAColor SFloatRGBAColor::operator +=( const SFloatRGBAColor& rColor)
{
	this->fAlpha += rColor.fAlpha;
	this->fRed   += rColor.fRed;
	this->fGreen += rColor.fGreen;
	this->fBlue  += rColor.fBlue;
	return *this;
}


inline SFloatRGBAColor SFloatRGBAColor::operator -=( const SFloatRGBAColor& rColor)
{
	this->fAlpha -= rColor.fAlpha;
	this->fRed   -= rColor.fRed;
	this->fGreen -= rColor.fGreen;
	this->fBlue  -= rColor.fBlue;
	return *this;
}


inline SFloatRGBAColor SFloatRGBAColor::operator *(const SFloatRGBAColor& rColor) const
{
	return SFloatRGBAColor(
		this->fRed   * rColor.fRed,
		this->fGreen * rColor.fGreen,
		this->fBlue  * rColor.fBlue,
		this->fAlpha * rColor.fAlpha
		);
}


/*
void SFloatRGBAColor::CopyFromD3DCOLOR(D3DCOLOR color)
{
	this->fAlpha = (float)((color & 0xFF000000) >> 24) / 255.0f;
	this->fRed   = (float)((color & 0x00FF0000) >> 16) / 255.0f;
	this->fGreen = (float)((color & 0x0000FF00) >> 8) / 255.0f;
	this->fBlue  = (float)(color & 0x000000FF) / 255.0f;
}

D3DCOLOR SFloatRGBAColor::ConvertToD3DCOLOR()
{
	return D3DCOLOR_ARGB( (int)(fAlpha * 255.0), (int)(fRed * 255.0), (int)(fGreen * 255.0f), (int)(fBlue * 255.0f) );

}*/
