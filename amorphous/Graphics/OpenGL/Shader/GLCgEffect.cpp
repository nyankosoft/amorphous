#include "GLCgEffect.hpp"
#include <Cg/cgGL.h> /* Cg OpenGL API (part of Cg Toolkit) */


namespace amorphous
{

extern CGcontext g_myCgContext;
extern void InitCg();


CGLCgEffect::CGLCgEffect()
{
	m_CgContext = cgCreateContext();
	CheckForCgError("creating context");

	ONCE( InitCg() );

	ONCE( InitCgContext() );
}


void CGLCgEffect::SetCGTextureParameter( CGparameter& param, TextureHandle& texture )
{
	cgGLSetTextureParameter( param, texture.GetGLTextureID() );
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
