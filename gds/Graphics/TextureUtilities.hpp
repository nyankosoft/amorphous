#ifndef __TextureUtilities_HPP__
#define __TextureUtilities_HPP__


#include "TextureHandle.hpp"
#include "TextureGenerators/SingleColorTextureGenerator.hpp"


namespace amorphous
{


inline TextureHandle CreateSingleColorTexture( const SFloatRGBAColor& color = SFloatRGBAColor::White(), uint width = 1, uint height = 1 )
{
	CTextureResourceDesc desc;
	desc.Width     = width;
	desc.Height    = height;
	desc.MipLevels = 0;
	desc.Format    = TextureFormat::A8R8G8B8;
	desc.pLoader.reset( new CSingleColorTextureGenerator( color ) );

	TextureHandle tex;
	bool loaded = tex.Load( desc );

	return tex;
}


} // amorphous



#endif /* __TextureUtilities_HPP__ */
