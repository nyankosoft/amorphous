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

	void ActiveTexture()
	{
		LOG_GL_ERROR( "Clearing OpenGL error(s)..." );

		if( glActiveTexture )
			glActiveTexture( GL_TEXTURE0 + m_Stage );
		else if( glActiveTextureARB )
			glActiveTextureARB( GL_TEXTURE0_ARB + m_Stage );
		else
			LOG_PRINT_ERROR( " You don't have glActiveTexture()/glActiveTextureARB()?! Seriously?!" );

//		std::string error_message = fmt_string( " glActiveTexture() failed (stage: %d).", m_Stage );
//		LOG_GL_ERROR( error_message.c_str() );
		LOG_GL_ERROR( "glActiveTexture() failed." );
	}

	Result::Name Visit( CGLTextureResource& texture_resource )
	{
		ActiveTexture();

		glBindTexture( GL_TEXTURE_2D, texture_resource.GetGLTextureID() );

		LOG_GL_ERROR( "glBindTexture() failed." );

		return Result::SUCCESS;
	}

	Result::Name Visit( CGLCubeTextureResource& texture_resource )
	{
		ActiveTexture();

		static const GLenum cube_map_targets[] =
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


inline Result::Name SetTextureGL( uint stage, const TextureHandle& texture )
{
	GLSLTextureResourceVisitor visitor( stage );
	return texture.AcceptTextureResourceVisitor( visitor );
}



} // namespace amorphous


#endif /* __GLTextureResourceVisitor_HPP__ */
