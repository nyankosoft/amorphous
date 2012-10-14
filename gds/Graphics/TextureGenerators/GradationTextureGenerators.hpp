#ifndef __GradationTextureGenerators_HPP__
#define __GradationTextureGenerators_HPP__


#include "../GraphicsResources.hpp"
#include "../../Support/linear_interpolation_table.hpp"
#include "../TextureHandle.hpp"


class CGradationTextureGenerator : public CTextureFillingAlgorithm
{
public:

	linear_interpolation_table<SFloatRGBAColor> m_Colors;

public:

	CGradationTextureGenerator() {}

	virtual ~CGradationTextureGenerator() {}

	void SetColors( linear_interpolation_table<SFloatRGBAColor>& colors )
	{
		m_Colors = colors;
	}
};


class CHorizontalGradationTextureGenerator : public CGradationTextureGenerator
{
public:

	CHorizontalGradationTextureGenerator() {}

	CHorizontalGradationTextureGenerator( const SFloatRGBAColor& top, const SFloatRGBAColor& bottom )
	{
		m_Colors.set( 0.0f, top );
		m_Colors.set( 1.0f, bottom );
	}

	CHorizontalGradationTextureGenerator( const SFloatRGBAColor& top, const SFloatRGBAColor& mid, const SFloatRGBAColor& bottom )
	{
		m_Colors.set( 0.0f, top );
		m_Colors.set( 0.5f, mid );
		m_Colors.set( 1.0f, bottom );
	}

	void FillTexture( CLockedTexture& texture )
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


class CVerticalGradationTextureGenerator : public CGradationTextureGenerator
{
public:

	CVerticalGradationTextureGenerator() {}

	CVerticalGradationTextureGenerator( const SFloatRGBAColor& left, const SFloatRGBAColor& right )
	{
		m_Colors.set( 0.0f, left );
		m_Colors.set( 1.0f, right );
	}

	CVerticalGradationTextureGenerator( const SFloatRGBAColor& left, const SFloatRGBAColor& mid, const SFloatRGBAColor& right )
	{
		m_Colors.set( 0.0f, left );
		m_Colors.set( 0.5f, mid );
		m_Colors.set( 1.0f, right );
	}

	void FillTexture( CLockedTexture& texture )
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


CTextureHandle CreateGradationTexture( uint width, uint height, TextureFormat::Format format, boost::shared_ptr<CGradationTextureGenerator> pGradationTextureGenerator )
{
	CTextureResourceDesc desc;
	desc.Width  = (int)width;
	desc.Height = (int)height;
	desc.Format = format;
	desc.pLoader = pGradationTextureGenerator;

	CTextureHandle tex;
	bool loaded = tex.Load( desc );
//	if( !loaded )
//		LOG_PRINT_WARNING(  );

	return tex;
}


CTextureHandle CreateHorizontalGradationTexture( uint width, uint height, TextureFormat::Format format, const SFloatRGBAColor& top_color, const SFloatRGBAColor& bottom_color )
{
	boost::shared_ptr<CHorizontalGradationTextureGenerator> pGenerator( new CHorizontalGradationTextureGenerator(top_color,bottom_color) );
	return CreateGradationTexture( width, height, format, pGenerator );
}


CTextureHandle CreateHorizontalGradationTexture( uint width, uint height, TextureFormat::Format format, const SFloatRGBAColor& top_color, const SFloatRGBAColor& mid_color, const SFloatRGBAColor& bottom_color )
{
	boost::shared_ptr<CHorizontalGradationTextureGenerator> pGenerator( new CHorizontalGradationTextureGenerator(top_color,mid_color,bottom_color) );
	return CreateGradationTexture( width, height, format, pGenerator );
}


CTextureHandle CreateVerticalGradationTexture( uint width, uint height, TextureFormat::Format format, const SFloatRGBAColor& left_color, const SFloatRGBAColor& right_color )
{
	boost::shared_ptr<CVerticalGradationTextureGenerator> pGenerator( new CVerticalGradationTextureGenerator(left_color,right_color) );
	return CreateGradationTexture( width, height, format, pGenerator );
}


CTextureHandle CreateVerticalGradationTexture( uint width, uint height, TextureFormat::Format format, const SFloatRGBAColor& left_color, const SFloatRGBAColor& mid_color, const SFloatRGBAColor& right_color )
{
	boost::shared_ptr<CVerticalGradationTextureGenerator> pGenerator( new CVerticalGradationTextureGenerator(left_color,mid_color,right_color) );
	return CreateGradationTexture( width, height, format, pGenerator );
}



#endif /* __GradationTextureGenerators_HPP__ */
