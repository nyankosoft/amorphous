#include "GLCgEffect.hpp"
#include <Cg/cgGL.h> /* Cg OpenGL API (part of Cg Toolkit) */


CGLCgEffect::CGLCgEffect()
{
	m_CgContext = cgCreateContext();
	CheckForCgError("creating context");

	cgGLRegisterStates(m_CgContext);
	CheckForCgError("registering standard CgFX states");
	cgGLSetManageTextureParameters(m_CgContext, CG_TRUE);
	CheckForCgError("manage texture parameters");
}


void CGLCgEffect::SetCGTextureParameter( CGparameter& param, CTextureHandle& texture )
{
	cgGLSetTextureParameter( param, texture.GetGLTextureID() );
}
