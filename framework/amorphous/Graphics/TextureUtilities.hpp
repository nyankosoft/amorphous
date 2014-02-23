#ifndef __TextureUtilities_HPP__
#define __TextureUtilities_HPP__


#include "TextureHandle.hpp"
#include "TextureGenerators/SingleColorTextureGenerator.hpp"


namespace amorphous
{


inline void SetSingleColorTextureDesc( TextureResourceDesc& desc,
							   const SFloatRGBAColor& color = SFloatRGBAColor::White(),
							   TextureFormat::Format format = TextureFormat::A8R8G8B8,
							   int tex_width = 1,
							   int tex_height = 1
							   )
{
	desc.Width  = tex_width;
	desc.Height = tex_height;
	desc.Format = TextureFormat::A8R8G8B8;
	desc.pLoader.reset( new SingleColorTextureGenerator(SFloatRGBAColor::White()) );
}


inline TextureHandle CreateSingleColorTexture( const SFloatRGBAColor& color = SFloatRGBAColor::White(), uint width = 1, uint height = 1 )
{
	TextureResourceDesc desc;
	desc.Width     = width;
	desc.Height    = height;
	desc.MipLevels = 0;
	desc.Format    = TextureFormat::A8R8G8B8;
	desc.pLoader.reset( new SingleColorTextureGenerator( color ) );

	TextureHandle tex;
	bool loaded = tex.Load( desc );

	if( !loaded )
		LOG_PRINTF_ERROR(( " Failed to create the specified texture (size: %dx%d).", width, height ));

	return tex;
}


} // amorphous



#endif /* __TextureUtilities_HPP__ */
