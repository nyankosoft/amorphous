#ifndef __BuiltinImageTextureLoader_HPP__
#define __BuiltinImageTextureLoader_HPP__


#include "TextureFillingAlgorithm.hpp"
#include "../../Support/Log/DefaultLog.hpp"
#include "../../Support/BuiltinImage.hpp"


namespace amorphous
{


class BuiltinImageTextureLoader : public TextureFillingAlgorithm
{
	const CBuiltinImage& m_BuiltinImage;

public:
	BuiltinImageTextureLoader( const CBuiltinImage& img ) : m_BuiltinImage(img) {}

	~BuiltinImageTextureLoader(){}

	void FillTexture( LockedTexture& texture )
	{
		if( !m_BuiltinImage.IsValid() )
			return;

		BitmapImage img;
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

	unsigned int GetArchiveObjectID() const { return TG_BUILTIN_IMAGE_TEXTURE_LOADER; }

	void Serialize( IArchive& ar, const unsigned int version )
	{
		TextureFillingAlgorithm::Serialize( ar, version );

		LOG_PRINT_ERROR( " The serialization of this class is not supported." );
	}
};


inline TextureHandle CreateTextureFromBuiltinImage( const CBuiltinImage& builtin_image )
{
	TextureResourceDesc desc;
	desc.Width     = builtin_image.width;
	desc.Height    = builtin_image.height;
	desc.Format    = TextureFormat::A8R8G8B8;
	desc.MipLevels = 0;
	desc.pLoader.reset( new BuiltinImageTextureLoader( builtin_image ) );

	TextureHandle texture;
	bool loaded = texture.Load( desc );
	if( loaded )
		return texture;
	else
		return TextureHandle();
}


} // namespace amorphous



#endif /* __BuiltinImageTextureLoader_HPP__ */
