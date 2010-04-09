#include "GLGraphicsDevice.hpp"

#include "../base.hpp"
#include "Graphics/FogParams.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Macro.h"


using namespace std;


const char *get_gl_error_text( GLenum gl_error )
{
	switch( gl_error )
	{
	case GL_NO_ERROR:          return "GL_NO_ERROR";
	case GL_INVALID_ENUM:      return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:     return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
	case GL_STACK_OVERFLOW:    return "GL_STACK_OVERFLOW";
	case GL_STACK_UNDERFLOW:   return "GL_STACK_UNDERFLOW";
	case GL_OUT_OF_MEMORY:     return "GL_OUT_OF_MEMORY";
	default: return "unknown error";
	}
}


void LogGLError( const char *fname, const char *msg )
{
//	GLenum error_flags[12];

	string errors_text;
	while(1)
	{
		GLenum gl_error = glGetError();
		if( gl_error == GL_NO_ERROR )
			break;
		else
			errors_text += string( 0 < errors_text.length() ? "|" : "" ) + get_gl_error_text(gl_error);
	}

	if( 0 < errors_text.length() )
		LOG_PRINT_ERROR( " OpenGL error(s) in function " + string(fname) + "() - " + string(msg) + " " + errors_text );
}



//========================================================================
// CGLGraphicsDevice
//========================================================================

// definition of singleton instance
CGLGraphicsDevice CGLGraphicsDevice::ms_CGLGraphicsDevice_;


CGLGraphicsDevice::CGLGraphicsDevice()
{
	m_SourceBlend = ToGLBlendModeEnum( AlphaBlend::SrcAlpha );
	m_DestBlend   = ToGLBlendModeEnum( AlphaBlend::InvSrcAlpha );
}


bool CGLGraphicsDevice::Init( int iWindowWidth, int iWindowHeight, ScreenMode::Name screen_mode )
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	glEnable(GL_TEXTURE_2D);
	LOG_GL_ERROR( "glEnable() failed." );

	m_State = CGraphicsDevice::STATE_INITIALIZED;

	return true;
}


void CGLGraphicsDevice::SetDefaultRenderStates()
{
	// some default render states
}


void CGLGraphicsDevice::Release()
{
}


void CGLGraphicsDevice::GetAdapterModesForDefaultAdapter( std::vector<CAdapterMode>& dest_buffer )
{
/*	if( m_vecAdapterMode.size() == 0 )
		LOG_PRINT_WARNING( "No adapeter modes are in m_vecAdapterMode." );

	dest_buffer = m_vecAdapterMode;*/
}


bool CGLGraphicsDevice::IsCurrentDisplayMode( const CDisplayMode& display_mode )
{
	return false;
}


void CGLGraphicsDevice::EnumAdapterModesForDefaultAdapter()
{
}


Result::Name CGLGraphicsDevice::SetTexture( int stage, const CTextureHandle& texture )
{
	glBindTexture( GL_TEXTURE_2D, texture.GetGLTextureID() );

	LOG_GL_ERROR( "glBindTexture() failed." );

	return Result::SUCCESS;
}


bool ToGLenum( RenderStateType::Name type, GLenum& cap )
{
	switch(type)
	{
	case RenderStateType::DEPTH_TEST:   cap = GL_DEPTH_TEST; return true;
	case RenderStateType::ALPHA_BLEND:  cap = GL_BLEND;	    return true;
	case RenderStateType::ALPHA_TEST:   cap = GL_ALPHA_TEST; return true;
	case RenderStateType::LIGHTING:	    cap = GL_LIGHTING;   return true;
	case RenderStateType::FOG:		    cap = GL_FOG;	    return true;
	case RenderStateType::FACE_CULLING: cap = GL_CULL_FACE;  return true;
	default:
		return false;
	}
}


Result::Name CGLGraphicsDevice::SetRenderState( RenderStateType::Name type, bool enable )
{
	GLenum cap = 0;
	bool res = ToGLenum( type, cap );

	if( res )
	{
		if( enable )
			glEnable( cap );
		else
			glDisable( cap );

		return Result::SUCCESS;
	}

	// 'type' is not a state type to switch on/off by glEnable() / glDisable()
	switch(type)
	{
	case RenderStateType::WRITING_INTO_DEPTH_BUFFER:
		glDepthMask( enable ? GL_TRUE : GL_FALSE );
		break;
//	case RenderStateType::UNKNOWN:
//		break;
	default:
		return Result::INVALID_ARGS;
	}

	return Result::SUCCESS;
}


static inline int ToGLFogMode( FogMode::Name fog_mode )
{
	switch( fog_mode )
	{
	case FogMode::LINEAR: return GL_LINEAR;
	case FogMode::EXP:    return GL_EXP;
	case FogMode::EXP2:   return GL_EXP2;
	default: return GL_LINEAR;
	}
}


Result::Name CGLGraphicsDevice::SetFogParams( const CFogParams& fog_params )
{
	glFogi(GL_FOG_MODE, ToGLFogMode(fog_params.Mode) );		// Fog Mode

	float rgba[] = { fog_params.Color.fRed, fog_params.Color.fGreen, fog_params.Color.fBlue, fog_params.Color.fAlpha };
	glFogfv(GL_FOG_COLOR, rgba );

	glFogf( GL_FOG_START,   (float)fog_params.Start );

	glFogf( GL_FOG_END,     (float)fog_params.End );

	glFogf( GL_FOG_DENSITY, (float)fog_params.Density );

	glHint( GL_FOG_HINT, GL_DONT_CARE ); // or GL_NICEST / GL_FASTEST

	return Result::SUCCESS;
}
