#ifndef __GradationTextureGenerators_HPP__
#define __GradationTextureGenerators_HPP__


#include "TextureFillingAlgorithm.hpp"
#include "../../Support/linear_interpolation.hpp"
#include "../TextureHandle.hpp"


namespace amorphous
{


class GradationTextureGenerator : public TextureFillingAlgorithm
{
public:

	linear_interpolation_table<SFloatRGBAColor> m_Colors;

public:

	GradationTextureGenerator() {}

	virtual ~GradationTextureGenerator() {}

	void SetColors( linear_interpolation_table<SFloatRGBAColor>& colors )
	{
		m_Colors = colors;
	}
};


class HorizontalGradationTextureGenerator : public GradationTextureGenerator
{
public:

	HorizontalGradationTextureGenerator() {}

	HorizontalGradationTextureGenerator( const SFloatRGBAColor& top, const SFloatRGBAColor& bottom )
	{
		m_Colors.set( 0.0f, top );
		m_Colors.set( 1.0f, bottom );
	}

	HorizontalGradationTextureGenerator( const SFloatRGBAColor& top, const SFloatRGBAColor& mid, const SFloatRGBAColor& bottom )
	{
		m_Colors.set( 0.0f, top );
		m_Colors.set( 0.5f, mid );
		m_Colors.set( 1.0f, bottom );
	}

	void FillTexture( LockedTexture& texture )
	{
		const int w = texture.GetWidth();
		const int h = texture.GetHeight();

		for( int y=0; y<h; y++ )
		{
			const SFloatRGBAColor color = m_Colors.get( (float)y / (float)h );

			for( int x=0; x<w; x++ )
//				texture.SetPixel( x, y, color );
				texture.SetPixelARGB32( x, y, color.GetARGB32() );
		}
	}
};


class VerticalGradationTextureGenerator : public GradationTextureGenerator
{
public:

	VerticalGradationTextureGenerator() {}

	VerticalGradationTextureGenerator( const SFloatRGBAColor& left, const SFloatRGBAColor& right )
	{
		m_Colors.set( 0.0f, left );
		m_Colors.set( 1.0f, right );
	}

	VerticalGradationTextureGenerator( const SFloatRGBAColor& left, const SFloatRGBAColor& mid, const SFloatRGBAColor& right )
	{
		m_Colors.set( 0.0f, left );
		m_Colors.set( 0.5f, mid );
		m_Colors.set( 1.0f, right );
	}

	void FillTexture( LockedTexture& texture )
	{
		const int w = texture.GetWidth();
		const int h = texture.GetHeight();

		for( int x=0; x<w; x++ )
		{
			const SFloatRGBAColor color = m_Colors.get( (float)x / (float)w );

			for( int y=0; y<h; y++ )
//				texture.SetPixel( x, y, color );
				texture.SetPixelARGB32( x, y, color.GetARGB32() );
		}
	}
};


inline TextureHandle CreateTextureFromGenerator( uint width, uint height, TextureFormat::Format format, boost::shared_ptr<TextureFillingAlgorithm> pTextureGenerator )
{
	TextureResourceDesc desc;
	desc.Width  = (int)width;
	desc.Height = (int)height;
	desc.Format = format;
	desc.pLoader = pTextureGenerator;

	TextureHandle tex;
	bool loaded = tex.Load( desc );
//	if( !loaded )
//		LOG_PRINT_WARNING(  );

	return tex;
}


inline TextureHandle CreateHorizontalGradationTexture( uint width, uint height, TextureFormat::Format format, const SFloatRGBAColor& top_color, const SFloatRGBAColor& bottom_color )
{
	boost::shared_ptr<HorizontalGradationTextureGenerator> pGenerator( new HorizontalGradationTextureGenerator(top_color,bottom_color) );
	return CreateTextureFromGenerator( width, height, format, pGenerator );
}


inline TextureHandle CreateHorizontalGradationTexture( uint width, uint height, TextureFormat::Format format, const SFloatRGBAColor& top_color, const SFloatRGBAColor& mid_color, const SFloatRGBAColor& bottom_color )
{
	boost::shared_ptr<HorizontalGradationTextureGenerator> pGenerator( new HorizontalGradationTextureGenerator(top_color,mid_color,bottom_color) );
	return CreateTextureFromGenerator( width, height, format, pGenerator );
}


inline TextureHandle CreateVerticalGradationTexture( uint width, uint height, TextureFormat::Format format, const SFloatRGBAColor& left_color, const SFloatRGBAColor& right_color )
{
	boost::shared_ptr<VerticalGradationTextureGenerator> pGenerator( new VerticalGradationTextureGenerator(left_color,right_color) );
	return CreateTextureFromGenerator( width, height, format, pGenerator );
}


inline TextureHandle CreateVerticalGradationTexture( uint width, uint height, TextureFormat::Format format, const SFloatRGBAColor& left_color, const SFloatRGBAColor& mid_color, const SFloatRGBAColor& right_color )
{
	boost::shared_ptr<VerticalGradationTextureGenerator> pGenerator( new VerticalGradationTextureGenerator(left_color,mid_color,right_color) );
	return CreateTextureFromGenerator( width, height, format, pGenerator );
}


} // namespace amorphous



#endif /* __GradationTextureGenerators_HPP__ */
