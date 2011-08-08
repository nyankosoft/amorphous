#ifndef __DIRECT3D9_H__
#define __DIRECT3D9_H__


#include "../GraphicsDevice.hpp"
#include <d3dx9.h>

//#pragma comment( lib, "d3dx9dt.lib" )
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )


#define DIRECT3D9 ( CDirect3D9::ms_CDirect3D9_ )
//#define DIRECT3D9 ( CDirect3D9::GetInstance() )


class CDirect3D9 : public CGraphicsDevice
{
private:

	LPDIRECT3D9			m_pD3D;

	LPDIRECT3DDEVICE9	m_pD3DDevice;

	D3DPRESENT_PARAMETERS m_CurrentPresentParameters; 

	D3DDEVTYPE m_DeviceType;

	DWORD m_BehaviorFlags;

	D3DFORMAT m_AdapterFormat; ///< the surface format of the display mode

	std::vector<CAdapterMode> m_vecAdapterMode;

	D3DZBUFFERTYPE m_CurrentDepthBufferType;

	DWORD m_ClearColor;

	float m_fClearDepth;

	CullingMode::Name m_CullMode;

	std::vector<Plane> m_vecClipPlane;

private:

	void SetDefaultRenderStates();

	/// Retrieve possible adapter modes for the primary display adapter
	/// and store them to m_vecAdapterMode. Called in InitD3D().
	void EnumAdapterModesForDefaultAdapter();

	bool CreateD3DDevice( D3DPRESENT_PARAMETERS& present_params, HWND hWnd );

protected:

	CDirect3D9();	// singleton

public:

	static CDirect3D9 ms_CDirect3D9_; // singleton instance

//	inline static CDirect3D9& GetInstance() { return ms_CDirect3D9_; }

	LPDIRECT3D9 GetD3D() { return m_pD3D; }

	LPDIRECT3DDEVICE9 GetDevice() { return m_pD3DDevice; }
	
	bool InitD3D( HWND hWnd, int iWindowWidth = 0, int iWindowHeight = 0, int screen_mode = WINDOWED );

	bool ResetD3DDevice( HWND hWnd, int iWindowWidth = 0, int iWindowHeight = 0, bool bFullScreen = false );

	void Release();

	D3DDEVTYPE GetDeviceType() const { return m_DeviceType; }

	DWORD GetBehaviorFlags() const { return m_BehaviorFlags; }

	D3DFORMAT GetAdapterFormat() { return m_AdapterFormat; }

	/// experimental: Could this be a platform-independent way to retrieve resolutions?
	void GetAdapterModesForDefaultAdapter( std::vector<CAdapterMode>& dest_buffer );

	bool IsCurrentDisplayMode( const CDisplayMode& display_mode ) const;

	Result::Name SetTexture( int stage, const CTextureHandle& texture );

	Result::Name SetTextureStageParams( uint stage, const CTextureStage& params );

	Result::Name SetTextureTrasnformParams( uint stage, const CTextureTransformParams& params );

	Result::Name SetTextureCoordTrasnform( uint stage, const Matrix44& transform );

	Result::Name SetRenderState( RenderStateType::Name type, bool enable );

	void SetSourceBlendMode( AlphaBlend::Mode src_blend_mode );

	void SetDestBlendMode( AlphaBlend::Mode dest_blend_mode );

	Result::Name SetFogParams( const CFogParams& fog_params );

	Result::Name SetCullingMode( CullingMode::Name cull_mode );

	Result::Name GetViewport( CViewport& viewport );

	Result::Name SetViewport( const CViewport& viewport );

	Result::Name SetClearColor( const SFloatRGBAColor& color );

	Result::Name SetClearDepth( float depth );

	Result::Name Clear( U32 buffer_mask );

	Result::Name SetClipPlane( uint index, const Plane& clip_plane );

	Result::Name EnableClipPlane( uint index );

	Result::Name DisableClipPlane( uint index );

	Result::Name UpdateViewProjectionTransformsForClipPlane( uint index, const Matrix44& view_transform, const Matrix44& proj_transform );

	Result::Name SetScissorRect( const SRect& rect );

	enum mode
	{
		FULLSCREEN,
		WINDOWED
	};
};


#endif // __DIRECT3D9_H__
