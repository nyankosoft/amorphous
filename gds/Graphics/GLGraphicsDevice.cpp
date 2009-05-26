#include "GLGraphicsDevice.hpp"

#include "../base.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Macro.h"


using namespace std;



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


bool ToGLenum( RenderStateType::Name type, GLenum cap )
{
	switch(type)
	{
	case RenderStateType::DEPTH_TEST:   cap = GL_DEPTH_TEST; return true;
	case RenderStateType::ALPHA_BLEND:  cap = GL_BLEND;	    return true;
	case RenderStateType::ALPHA_TEST:   cap = GL_ALPHA_TEST; return true;
	case RenderStateType::LIGHTING:	   cap = GL_LIGHTING;   return true;
	case RenderStateType::FOG:		   cap = GL_FOG;	    return true;
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
