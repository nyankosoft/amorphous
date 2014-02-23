#ifndef __TCBSplineGradationTextureGenerators_HPP__
#define __TCBSplineGradationTextureGenerators_HPP__


#include "../GraphicsResources.hpp"
#include "../../3DMath/TCBSpline.hpp"
#include "GradationTextureGenerators.hpp"


namespace amorphous
{


inline U32 GetARGB32_checked( const SFloatRGBAColor& src)
{
	return ( ( ( (unsigned char)get_clamped((int)(src.alpha * 255.0f),0,255)) << 24 ) & 0xFF000000 ) |
		   ( ( ( (unsigned char)get_clamped((int)(src.red   * 255.0f),0,255)) << 16 ) & 0x00FF0000 ) |
		   ( ( ( (unsigned char)get_clamped((int)(src.green * 255.0f),0,255)) <<  8 ) & 0x0000FF00 ) |
		   (   ( (unsigned char)get_clamped((int)(src.blue  * 255.0f),0,255))         & 0x000000FF );
}


inline SFloatRGBAColor TCBSplineMultiply( const SFloatRGBAColor& p1, const SFloatRGBAColor& p2, const SFloatRGBAColor& incoming_tangent, const SFloatRGBAColor& outgoing_tangent, const Vector4& hS )
{
	SFloatRGBAColor dest;
	dest.red   = p1.red   * hS.x + p2.red   * hS.y + incoming_tangent.red   * hS.z + outgoing_tangent.red   * hS.w;
	dest.green = p1.green * hS.x + p2.green * hS.y + incoming_tangent.green * hS.z + outgoing_tangent.green * hS.w;
	dest.blue  = p1.blue  * hS.x + p2.blue  * hS.y + incoming_tangent.blue  * hS.z + outgoing_tangent.blue  * hS.w;
	dest.alpha = p1.alpha * hS.x + p2.alpha * hS.y + incoming_tangent.alpha * hS.z + outgoing_tangent.alpha * hS.w;
	clamp( dest.red,   0.0f, 1.0f );
	clamp( dest.green, 0.0f, 1.0f );
	clamp( dest.blue,  0.0f, 1.0f );
	clamp( dest.alpha, 0.0f, 1.0f );
	return dest;
}


//template<typename T>
//class tcb_control_point
//{
//public:
//
//	float tension;
//	float continuity;
//	float bias;
//
//	T value;
//
//public:
//
//	tcb_control_point() : tension(0), continuity(0), bias(0) {}
//
//	tcb_control_point( float t, float c, float b ) : tension(t), continuity(c), bias(b) {}
//
//	~tcb_control_point(){}
//
//	void MyMethod(){}
//	void Accept( MyVisitor& visitor ){ visitor.Visit( *this ) }
//};


class tcb_section
{
public:

	float tension;
	float continuity;
	float bias;

public:

	tcb_section() : tension(0), continuity(0), bias(0) {}

	tcb_section( float t, float c, float b ) : tension(t), continuity(c), bias(b) {}

	~tcb_section(){}
};


class TCBSplineGradationTextureGenerator : public TextureFillingAlgorithm
{
public:

	typedef std::pair<float,SFloatRGBAColor> float_and_rgba_color;

//	std::pair< float, tcb_control_point<SFloatRGBAColor> > m_texel_pos_and_cp;
	std::vector< std::pair<float,SFloatRGBAColor> > m_Colors;
	std::vector<tcb_section> m_TCBSections; ///< There should be m_Colors.size() - 3 sections in total

public:

	TCBSplineGradationTextureGenerator() {}

	virtual ~TCBSplineGradationTextureGenerator() {}
};


class TCBSplineHorizontalGradationTextureGenerator : public TCBSplineGradationTextureGenerator
{
public:

	TCBSplineHorizontalGradationTextureGenerator() {}

	TCBSplineHorizontalGradationTextureGenerator( const SFloatRGBAColor& top, const SFloatRGBAColor& bottom, float tension, float continuity, float bias )
	{
		m_Colors.resize( 4 );
		m_Colors[0] = float_and_rgba_color( -0.5f, top );
		m_Colors[1] = float_and_rgba_color(  0.0f, top );
		m_Colors[2] = float_and_rgba_color(  1.0f, bottom );
		m_Colors[3] = float_and_rgba_color(  1.5f, bottom );
		m_TCBSections.push_back( tcb_section(tension,continuity,bias) );
	}          

	TCBSplineHorizontalGradationTextureGenerator( const SFloatRGBAColor& top, const SFloatRGBAColor& mid, const SFloatRGBAColor& bottom, float tension, float continuity, float bias )
	{
		m_Colors.resize( 5 );
		m_Colors[0] = float_and_rgba_color( -0.5f, top );
		m_Colors[1] = float_and_rgba_color(  0.0f, top );
		m_Colors[2] = float_and_rgba_color(  0.5f, mid );
		m_Colors[3] = float_and_rgba_color(  1.0f, bottom );
		m_Colors[4] = float_and_rgba_color(  1.5f, bottom );
		m_TCBSections.resize( 2 );
		m_TCBSections[0] = tcb_section(tension,continuity,bias);
		m_TCBSections[1] = tcb_section(tension,continuity,bias);
	}

	// When the application is running on OpenGL, after this function is called
	// the color type of freeimage bitmap is altered from FIC_RGBALPHA to FIC_RGB.
	// Why?
	// This does not happen when texture.SetPixelARGB32() at the end of the function is commented out.
	void FillTexture( LockedTexture& texture )
	{
		// Commented out: LockedTexture::Clear() does not prevent the color type altered from FIC_RGBALPHA to FIC_RGB
//		texture.Clear( SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 0.0f ) );

		const int w = texture.GetWidth();
		const int h = texture.GetHeight();

		for( int y=0; y<h; y++ )
		{
			const float pos = ((float)y + 0.5f) / (float)h;
			int i2 = -1;
			for( size_t i=2; i<m_Colors.size(); i++ )
			{
				if( pos <= m_Colors[i].first )
				{
					i2 = (int)i;
					break;
				}
			}

			if( i2 == -1 )
				return;

			if( (int)m_Colors.size() <= i2 + 1 )
				return;

			const int i0 = i2 - 2;
			const int i1 = i2 - 1;
			const int i3 = i2 + 1;

			if( (int)m_TCBSections.size() <= i0 )
				return;

			const float local_pos = (pos - m_Colors[i1].first) / (m_Colors[i2].first - m_Colors[i1].first);

			const tcb_section& section = m_TCBSections[i0];

			const SFloatRGBAColor color = InterpolateWithTCBSpline(
				local_pos,
				m_Colors[i0].second,
				m_Colors[i1].second,
				m_Colors[i2].second,
				m_Colors[i3].second,
				section.tension,
				section.continuity,
				section.bias
				);

			for( int x=0; x<w; x++ )
			{
//				texture.SetPixel( x, y, color );
				texture.SetPixelARGB32( x, y, color.GetARGB32() ); // Altered from FIC_RGBALPHA to FIC_RGB
//				texture.SetPixelARGB32( x, y, 0 ); // Maintained to FIC_RGBALPHA
//				texture.SetPixelARGB32( x, y, GetARGB32_checked(color) ); // Altered from FIC_RGBALPHA to FIC_RGB
//				texture.SetPixelARGB32( x, y, 0xFFFFFFFF ); // Altered from FIC_RGBALPHA to FIC_RGB
//				texture.SetPixelARGB32( x, y, 0xFF000000 ); // Altered from FIC_RGBALPHA to FIC_RGB
//				texture.SetPixelARGB32( x, y, 0x00FFFFFF ); // Maintained to FIC_RGBALPHA
				texture.SetPixelARGB32( x, y, 0xFEFFFFFF ); // Maintained to FIC_RGBALPHA

				// Altered from FIC_RGBALPHA to FIC_RGB
//				SFloatRGBAColor clamped_alpha_color = color;
//				clamped_alpha_color.alpha = get_clamped( color.alpha, 0.0f, 250.0f );
//				texture.SetPixelARGB32( x, y, clamped_alpha_color.GetARGB32() );
			}
		}
	}
};


TextureHandle CreateTCBSplineHorizontalGradationTexture( uint width, uint height, TextureFormat::Format format,
	const SFloatRGBAColor& top_color,
	const SFloatRGBAColor& mid_color,
	const SFloatRGBAColor& bottom_color,
	float tension = 0,
	float continuity = 0,
	float bias = 0
	)
{
	boost::shared_ptr<TCBSplineHorizontalGradationTextureGenerator> pGenerator( new TCBSplineHorizontalGradationTextureGenerator(top_color,mid_color,bottom_color,tension,continuity,bias) );
	return CreateTextureFromGenerator( width, height, format, pGenerator );
}

} // amorphous



#endif /* __TCBSplineGradationTextureGenerators_HPP__ */
