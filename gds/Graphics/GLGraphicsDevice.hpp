#ifndef __GLGraphicsDevice_H__
#define __GLGraphicsDevice_H__


#include "GraphicsDevice.hpp"
#include <gl/gl.h>			// Header File For The OpenGL32 Library
#include <gl/glu.h>			// Header File For The GLu32 Library


#pragma comment( lib, "OpenGL32.lib" )
#pragma comment( lib, "glu32.lib" )


//------------------------------------------------------------------------------
// Function Externs
//------------------------------------------------------------------------------

extern void LogGLError( const char *fname, const char *msg );



//------------------------------------------------------------------------------
// Macro Definitions
//------------------------------------------------------------------------------

#define LOG_GL_ERROR( msg ) LogGLError( __FUNCTION__, msg );



//------------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------------

class CGLGraphicsDevice : public CGraphicsDevice
{
private:

//	std::vector<CAdapterMode> m_vecAdapterMode;

	GLenum m_SourceBlend;
	GLenum m_DestBlend;

private:

	void SetDefaultRenderStates();

	/// Retrieve possible adapter modes for the primary display adapter
	/// and store them to m_vecAdapterMode. Called in InitD3D().
	void EnumAdapterModesForDefaultAdapter();


protected:

	CGLGraphicsDevice();	// singleton

public:
	
	static CGLGraphicsDevice ms_CGLGraphicsDevice_; // singleton instance

	bool Init( int iWindowWidth, int iWindowHeight, ScreenMode::Name screen_mode );

	//bool ResetD3DDevice( HWND hWnd, int iWindowWidth = 0, int iWindowHeight = 0, bool bFullScreen = false );

	void Release();

	/// experimental: Could this be a platform-independent way to retrieve resolutions
	void GetAdapterModesForDefaultAdapter( std::vector<CAdapterMode>& dest_buffer );

	bool IsCurrentDisplayMode( const CDisplayMode& display_mode );

	Result::Name SetTexture( int stage, const CTextureHandle& texture );

	Result::Name SetRenderState( RenderStateType::Name type, bool enable );

	inline void SetSourceBlendMode( AlphaBlend::Mode src_blend_mode );

	inline void SetDestBlendMode( AlphaBlend::Mode dest_blend_mode );

	Result::Name SetFogParams( const CFogParams& fog_params );
};


inline CGLGraphicsDevice& GLGraphicsDevice()
{
	return (CGLGraphicsDevice::ms_CGLGraphicsDevice_);
}


static const GLenum g_ToGLBlendModeEnum[] =
{
	GL_ZERO,
	GL_ONE,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA_SATURATE
};


inline GLenum ToGLBlendModeEnum( AlphaBlend::Mode mode )
{
	return g_ToGLBlendModeEnum[mode];
}


inline void CGLGraphicsDevice::SetSourceBlendMode( AlphaBlend::Mode src_blend_mode )
{
	m_SourceBlend = ToGLBlendModeEnum( src_blend_mode );
	glBlendFunc( m_SourceBlend, m_DestBlend );
}


inline void CGLGraphicsDevice::SetDestBlendMode( AlphaBlend::Mode dest_blend_mode )
{
	m_DestBlend = ToGLBlendModeEnum( dest_blend_mode );
	glBlendFunc( m_SourceBlend, m_DestBlend );
}



#endif // __GLGraphicsDevice_H__
