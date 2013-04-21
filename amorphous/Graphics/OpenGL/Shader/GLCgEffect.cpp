#include "GLCgEffect.hpp"
#include "../GLGraphicsResources.hpp"
#include <Cg/cgGL.h> /* Cg OpenGL API (part of Cg Toolkit) */


namespace amorphous
{

extern CGcontext g_myCgContext;
extern void InitCg();


class GL_Cg_TextureResourceVisitor : public TextureResourceVisitor
{
	CGparameter& m_Param;

public:

	GL_Cg_TextureResourceVisitor(CGparameter& param) : m_Param(param) {}
	~GL_Cg_TextureResourceVisitor(){}

	Result::Name Visit( CGLTextureResource& texture_resource )
	{
		cgGLSetTextureParameter( m_Param, texture_resource.GetGLTextureID() );

//		LOG_GL_ERROR( "glBindTexture() failed." );

		return Result::SUCCESS;
	}

	Result::Name Visit( CGLCubeTextureResource& texture_resource )
	{
		return Result::UNKNOWN_ERROR;

//		const GLenum cube_map_targets[] =
//		{
//			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
//			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
//			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
//			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
//			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
//			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
//		};
//
//		for( int i=0; i<numof(cube_map_targets); i++ )
//		{
//			glBindTexture( cube_map_targets[i], texture_resource.GetCubeMapTexture(i) );
//
//			LOG_GL_ERROR( "glBindTexture() failed." );
//		}
//
//		return Result::SUCCESS;
	}
};


CGLCgEffect::CGLCgEffect()
{
	m_CgContext = cgCreateContext();
	CheckForCgError("creating context");

	ONCE( InitCg() );

	ONCE( InitCgContext() );
}


void CGLCgEffect::SetCGTextureParameter( CGparameter& param, TextureHandle& texture )
{
//	cgGLSetTextureParameter( param, texture.GetGLTextureID() );
	GL_Cg_TextureResourceVisitor visitor(param);
	texture.AcceptTextureResourceVisitor( visitor );
}


void CGLCgEffect::InitCgContext()
{
//	cgGLRegisterStates(m_CgContext);
	cgGLRegisterStates(g_myCgContext);
	CheckForCgError("registering standard CgFX states");
//	cgGLSetManageTextureParameters(m_CgContext, CG_TRUE);
	cgGLSetManageTextureParameters(g_myCgContext, CG_TRUE);
	CheckForCgError("manage texture parameters");
}


} // namespace amorphous
