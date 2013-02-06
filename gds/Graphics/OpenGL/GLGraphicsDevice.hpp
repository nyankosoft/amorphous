#ifndef __GLGraphicsDevice_H__
#define __GLGraphicsDevice_H__


#include "gds/Graphics/GraphicsDevice.hpp"
#include <gl/gl.h>			// Header File For The OpenGL32 Library
#include <gl/glu.h>			// Header File For The GLu32 Library


namespace amorphous
{


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

//	std::vector<AdapterMode> m_vecAdapterMode;

	GLenum m_SourceBlend;
	GLenum m_DestBlend;

	GLenum m_AlphaFunc;

	float m_fReferenceAlphaValue;

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

	void GetAdapterModesForDefaultAdapter( std::vector<AdapterMode>& dest_buffer );

	bool IsCurrentDisplayMode( const DisplayMode& display_mode ) const;

	Result::Name SetTexture( int stage, const TextureHandle& texture );

	Result::Name SetTextureStageParams( uint stage, const TextureStage& params );

	Result::Name SetTextureTrasnformParams( uint stage, const CTextureTransformParams& params );

	Result::Name SetTextureCoordTrasnform( uint stage, const Matrix44& transform );

	bool GetRenderState( RenderStateType::Name type );

	Result::Name SetRenderState( RenderStateType::Name type, bool enable );

	inline void SetSourceBlendMode( AlphaBlend::Mode src_blend_mode );

	inline void SetDestBlendMode( AlphaBlend::Mode dest_blend_mode );

	void SetAlphaFunction( CompareFunc::Name alpha_func );

	void SetReferenceAlphaValue( float ref_alpha );

	Result::Name SetFogParams( const FogParams& fog_params );

	Result::Name SetCullingMode( CullingMode::Name cull_mode );

	Result::Name GetViewport( CViewport& viewport );

	Result::Name SetViewport( const CViewport& viewport );

	Result::Name SetClearColor( const SFloatRGBAColor& color );

	Result::Name SetClearDepth( float depth );

	Result::Name Clear( U32 buffer_mask );

	Result::Name SetClipPlane( uint index, const Plane& clip_plane );

	Result::Name EnableClipPlane( uint index );

	Result::Name DisableClipPlane( uint index );

	Result::Name UpdateViewProjectionTransformsForClipPlane( uint index, const Matrix44& view_transform, const Matrix44& proj_transform ) { return Result::SUCCESS; }

	Result::Name SetScissorRect( const SRect& rect );
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


} // namespace amorphous



#endif // __GLGraphicsDevice_H__
