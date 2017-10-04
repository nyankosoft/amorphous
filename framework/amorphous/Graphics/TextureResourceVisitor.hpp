#ifndef __TextureResourceVisitor_HPP__
#define __TextureResourceVisitor_HPP__


#include "amorphous/base.hpp"
#include "Direct3D/fwd.hpp"
#include "OpenGL/fwd.hpp"


namespace amorphous
{


class TextureResourceVisitor
{
public:
	TextureResourceVisitor(){}
	~TextureResourceVisitor(){}

	virtual Result::Name Visit( CD3DTextureResource& texture_resource )     { return Result::UNKNOWN_ERROR; };
	virtual Result::Name Visit( CD3DCubeTextureResource& texture_resource ) { return Result::UNKNOWN_ERROR; };
	virtual Result::Name Visit( CGLTextureResource& texture_resource )      { return Result::UNKNOWN_ERROR; };
	virtual Result::Name Visit( CGLCubeTextureResource& texture_resource )  { return Result::UNKNOWN_ERROR; };
};


} // namespace amorphous


#endif /* __TextureResourceVisitor_HPP__ */
