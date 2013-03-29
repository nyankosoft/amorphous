#ifndef __DIRECT3D9_H__
#define __DIRECT3D9_H__


#include "../GraphicsDevice.hpp"
#include <d3dx9.h>


namespace amorphous
{

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

	std::vector<AdapterMode> m_vecAdapterMode;

	D3DZBUFFERTYPE m_CurrentDepthBufferType;

	DWORD m_ClearColor;

	float m_fClearDepth;

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
	void GetAdapterModesForDefaultAdapter( std::vector<AdapterMode>& dest_buffer );

	bool IsCurrentDisplayMode( const DisplayMode& display_mode ) const;

	Result::Name SetTexture( int stage, const TextureHandle& texture );

	Result::Name SetTextureStageParams( uint stage, const TextureStage& params );

	Result::Name SetTextureTrasnformParams( uint stage, const CTextureTransformParams& params );

	Result::Name SetTextureCoordTrasnform( uint stage, const Matrix44& transform );

	bool GetRenderState( RenderStateType::Name type );

	Result::Name SetRenderState( RenderStateType::Name type, bool enable );

	void SetSourceBlendMode( AlphaBlend::Mode src_blend_mode );

	void SetDestBlendMode( AlphaBlend::Mode dest_blend_mode );

	void SetAlphaFunction( CompareFunc::Name alpha_func );

	void SetReferenceAlphaValue( float ref_alpha );

	Result::Name SetFogParams( const FogParams& fog_params );

	Result::Name SetCullingMode( CullingMode::Name cull_mode );

	Result::Name GetViewport( Viewport& viewport );

	Result::Name SetViewport( const Viewport& viewport );

	Result::Name SetClearColor( const SFloatRGBAColor& color );

	Result::Name SetClearDepth( float depth );

	Result::Name Clear( U32 buffer_mask );

	Result::Name BeginScene();

	Result::Name EndScene();

	Result::Name Present();

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

} // namespace amorphous



#endif // __DIRECT3D9_H__
