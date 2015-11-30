#ifndef __GradationTextureGenerators_HPP__
#define __GradationTextureGenerators_HPP__


#include "TextureFillingAlgorithm.hpp"
#include "../../Support/linear_interpolation.hpp"
#include "../../Support/Serialization/Serialization_linear_interpolation.hpp"
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

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		TextureFillingAlgorithm::Serialize( ar, version );

		ar & m_Colors;
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

		// color type remains RGBALHPHA
		// Note that the alpha is 0.0f. other non-1.0f values also did not
		// change the color type (tried 0.5f, 0.9f: no changes to the color type).
//		texture.Clear( SFloatRGBAColor(1.0f,1.0f,1.0f,0.0f) );

		// After this Clear(), FreeImage_GetColorType returns FIC_RGB because
		// FreeImage_GetColorType() considers the bitmap is of an RGB format
		// when all the alpha values are 0xFF.
		texture.Clear( SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f) );

		for( int y=0; y<h; y++ )
		{
			const SFloatRGBAColor color = m_Colors.get( (float)y / (float)h );

			for( int x=0; x<w; x++ )
//				texture.SetPixel( x, y, color );
//				texture.SetPixelARGB32( x, y, 0xFF000000 );
				texture.SetPixelARGB32( x, y, color.GetARGB32() );
		}

		// Fool FreeImage by semi-transparent (almost opaque) color to one of the pixels.
		SFloatRGBAColor src = texture.GetPixel(0,0);
		if( 0.999f <= src.alpha )
		{
			// Change the alpha to FE
			U32 non_opaque_color = ((src.GetARGB32() & 0x00FFFFFF) | 0xFE000000);
			texture.SetPixelARGB32( 0, 0, non_opaque_color );
		}
	}

	unsigned int GetArchiveObjectID() const { return TG_HORIZONTAL_GRADATION_TEXTURE_GENERATOR; }
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

	unsigned int GetArchiveObjectID() const { return TG_VERTICAL_GRADATION_TEXTURE_GENERATOR; }
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
