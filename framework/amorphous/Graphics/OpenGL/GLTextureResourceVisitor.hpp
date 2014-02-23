#ifndef __GLTextureResourceVisitor_HPP__
#define __GLTextureResourceVisitor_HPP__


#include "../TextureResourceVisitor.hpp"
#include "GLGraphicsResources.hpp"
#include "GLGraphicsDevice.hpp"
#include "GLExtensions.hpp"
#include <gl/gl.h>


namespace amorphous
{


class GL_FFP_TextureResourceVisitor : public TextureResourceVisitor
{
	uint m_Stage;

public:

	GL_FFP_TextureResourceVisitor(uint stage) : m_Stage(stage) {}
	~GL_FFP_TextureResourceVisitor(){}

	Result::Name Visit( CGLTextureResource& texture_resource )
	{
		LOG_GL_ERROR( " Clearing OpenGL errors..." );

		glBindTexture( GL_TEXTURE_2D, texture_resource.GetGLTextureID() );

		LOG_GL_ERROR( "glBindTexture() failed." );

		return Result::SUCCESS;
	}

	Result::Name Visit( CGLCubeTextureResource& texture_resource )
	{
		const GLenum cube_map_targets[] =
		{
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
		};

		for( int i=0; i<numof(cube_map_targets); i++ )
		{
			glBindTexture( cube_map_targets[i], texture_resource.GetCubeMapTexture(i) );

			LOG_GL_ERROR( "glBindTexture() failed." );
		}

		return Result::SUCCESS;
	}
};


class GLSLTextureResourceVisitor : public TextureResourceVisitor
{
	uint m_Stage;

public:

	GLSLTextureResourceVisitor(uint stage) : m_Stage(stage) {}
	~GLSLTextureResourceVisitor(){}

	Result::Name Visit( CGLTextureResource& texture_resource )
	{
//		glActiveTexture( GL_TEXTURE0 + m_Stage );

		glBindTexture( GL_TEXTURE_2D, texture_resource.GetGLTextureID() );

		LOG_GL_ERROR( "glBindTexture() failed." );

		return Result::SUCCESS;
	}
};


inline Result::Name SetTextureGL_FFP( uint stage, const TextureHandle& texture )
{
	GL_FFP_TextureResourceVisitor visitor( stage );
	return texture.AcceptTextureResourceVisitor( visitor );
}



} // namespace amorphous


#endif /* __GLTextureResourceVisitor_HPP__ */
