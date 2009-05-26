#ifndef __GLGraphicsDevice_H__
#define __GLGraphicsDevice_H__


#include "GraphicsDevice.hpp"
#include <gl/gl.h>			// Header File For The OpenGL32 Library
#include <gl/glu.h>			// Header File For The GLu32 Library


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

	Result::Name SetRenderState( RenderStateType::Name type, bool enable );

	inline void SetSourceBlendMode( AlphaBlend::Mode src_blend_mode );

	inline void SeDestBlendMode( AlphaBlend::Mode dest_blend_mode );
};


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


GLenum ToGLBlendModeEnum( AlphaBlend::Mode mode )
{
	return g_ToGLBlendModeEnum[mode];
}


void CGLGraphicsDevice::SetSourceBlendMode( AlphaBlend::Mode src_blend_mode )
{
	m_SourceBlend = ToGLBlendModeEnum( src_blend_mode );
	glBlendFunc( m_SourceBlend, m_DestBlend );
}


inline void CGLGraphicsDevice::SeDestBlendMode( AlphaBlend::Mode dest_blend_mode )
{
	m_DestBlend = ToGLBlendModeEnum( dest_blend_mode );
	glBlendFunc( m_SourceBlend, m_DestBlend );
}



#endif // __GLGraphicsDevice_H__
