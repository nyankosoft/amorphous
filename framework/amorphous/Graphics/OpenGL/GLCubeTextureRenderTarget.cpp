#include "GLCubeTextureRenderTarget.hpp"
#include "amorphous/Support/Profile.hpp"


namespace amorphous
{


GLCubeTextureRenderTarget::GLCubeTextureRenderTarget()
{
}


GLCubeTextureRenderTarget::GLCubeTextureRenderTarget( uint texture_size, TextureFormat::Format texture_format, uint option_flags )
//:
//TextureRenderTarget( texture_width, texture_height, texture_format, option_flags )
{
}


GLCubeTextureRenderTarget::GLCubeTextureRenderTarget( const TextureResourceDesc& texture_desc )
//:
//TextureRenderTarget(texture_desc)
{
}


GLCubeTextureRenderTarget::~GLCubeTextureRenderTarget()
{
	ReleaseGraphicsResources();
}


void GLCubeTextureRenderTarget::ReleaseTextures()
{
}


bool GLCubeTextureRenderTarget::CreateTextures( uint texture_size, TextureFormat::Format texture_format )
{
	return false;
}


bool GLCubeTextureRenderTarget::IsReady()
{
//	if( m_pCurrentCubeMap && m_pDepthCube )
//		return true;
//	else
//		return false;

	return true;
}


void GLCubeTextureRenderTarget::Begin()
{
}


void GLCubeTextureRenderTarget::SetRenderTarget( unsigned int face_index )
{
}


void GLCubeTextureRenderTarget::End()
{
}


void GLCubeTextureRenderTarget::LoadGraphicsResources( const GraphicsParameters& rParam )
{
}


void GLCubeTextureRenderTarget::ReleaseGraphicsResources()
{
}


} // namespace amorphous
