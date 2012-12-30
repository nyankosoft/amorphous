#ifndef __BuiltinImageTextureLoader_HPP__
#define __BuiltinImageTextureLoader_HPP__


#include "TextureFillingAlgorithm.hpp"
#include "../../Support/BuiltinImage.hpp"


namespace amorphous
{


class CBuiltinImageTextureLoader : public CTextureFillingAlgorithm
{
	const CBuiltinImage& m_BuiltinImage;

public:
	CBuiltinImageTextureLoader( const CBuiltinImage& img ) : m_BuiltinImage(img) {}

	~CBuiltinImageTextureLoader(){}

	void FillTexture( CLockedTexture& texture )
	{
		if( !m_BuiltinImage.IsValid() )
			return;

		CBitmapImage img;
		bool loaded = LoadBuiltinImage( m_BuiltinImage, img );
		if( !loaded )
			return;

		img.SaveToFile( ".debug/buitin_image.png" );

		const int w = texture.GetWidth();
		const int h = texture.GetHeight();

		if( img.GetWidth()  != w
		 || img.GetHeight() != h )
		{
			return;
		}

		img.FlipVertical();

		for( int y=0; y<h; y++ )
		{
			for( int x=0; x<w; x++ )
			{
				U32 pixel = img.GetPixelARGB32(x,y);
				texture.SetPixelARGB32( x, y, img.GetPixelARGB32(x,y) );
			}
		}
	}
};


CTextureHandle CreateTextureFromBuiltinImage( const CBuiltinImage& builtin_image )
{
	CTextureResourceDesc desc;
	desc.Width     = builtin_image.width;
	desc.Height    = builtin_image.height;
	desc.Format    = TextureFormat::A8R8G8B8;
	desc.MipLevels = 0;
	desc.pLoader.reset( new CBuiltinImageTextureLoader( builtin_image ) );

	CTextureHandle texture;
	bool loaded = texture.Load( desc );
	if( loaded )
		return texture;
	else
		return CTextureHandle();
}


} // namespace amorphous



#endif /* __BuiltinImageTextureLoader_HPP__ */
