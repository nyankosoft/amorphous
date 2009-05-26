#ifndef __GraphicsDevice_HPP__
#define __GraphicsDevice_HPP__

#include <vector>
#include "../base.hpp"
#include "AlphaBlend.hpp"
#include "TextureFormat.hpp"


class CDisplayMode
{
public:
	uint Width;
	uint Height;
	uint RefreshRate;
	TextureFormat::Format Format;

	// true if this is the current display mode.
	// - Invalidated after the graphics device settings are changed
	//   (e.g., resolution, fullscreen/window mode changes).
	// - Call CDirect3D9::GetAdapterModesForDefaultAdapter() to get updated info
	//   after chaging the settings.
	bool Current;

public:

	CDisplayMode( uint w=0, uint h=0, uint r=0, TextureFormat::Format fmt = TextureFormat::X8R8G8B8 )
		:
	Width(w),
	Height(h),
	RefreshRate(r),
	Format(fmt),
	Current(false)
	{}
};

class CAdapterMode
{
public:

	TextureFormat::Format Format;

	std::vector<CDisplayMode> vecDisplayMode;

public:

	CAdapterMode( TextureFormat::Format fmt = TextureFormat::X8R8G8B8 )
		:
	Format(fmt)
	{}
};


class DepthBufferType
{
public:
	enum Name
	{
		ZBUFFER,
		WBUFFER,
		NUM_DEPTH_BUFFER_TYPES
//		DISABLED,
//		NUM_DEPTH_BUFFER_STATES
	};
};


class ScreenMode
{
public:
	enum Name
	{
		WINDOWED,
		FULLSCREEN,
		NUM_MODES
	};
};


class RenderStateType
{
public:
	enum Name
	{
		DEPTH_TEST,
		ALPHA_BLEND,
		ALPHA_TEST,
		LIGHTING,
		FOG,
		FACE_CULLING,
		WRITING_INTO_DEPTH_BUFFER,
		NUM_RENDER_STATES
	};
};


class CFogDesc
{
public:
	float fStartDist;
	float fEndDist;
	float fDensity;
};


class CGraphicsDevice
{
protected:

//	std::vector<CAdapterMode> m_vecAdapterMode;

private:

//	void SetDefaultRenderStates();

	/// Retrieve possible adapter modes for the primary display adapter
	/// and store them to m_vecAdapterMode. Called in InitD3D().
//	void EnumAdapterModesForDefaultAdapter();


protected:

	//RenderSystem();	// singleton

public:

	virtual ~CGraphicsDevice() {}
	
	virtual bool Init( int iWindowWidth, int iWindowHeight, ScreenMode::Name screen_mode ) { return false; }

	//bool ResetD3DDevice( HWND hWnd, int iWindowWidth = 0, int iWindowHeight = 0, bool bFullScreen = false );

//	void Release();


	/// experimental: Could this be a platform-independent way to retrieve resolutions
//	void GetAdapterModesForDefaultAdapter( std::vector<CAdapterMode>& dest_buffer );

//	bool IsCurrentDisplayMode( const CDisplayMode& display_mode );
	
//	virtual void SetWorldTransform();
//	virtual void SetViewTransform();
//	virtual void SetProjectionTransform();

//	virtual void SetTexture( int stage, CTextureHandle& texture ) {};

	inline Result::Name Enable( RenderStateType::Name type ) { return SetRenderState( type, true ); }

	inline Result::Name Disable( RenderStateType::Name type ) { return SetRenderState( type, false ); }

	virtual Result::Name SetRenderState( RenderStateType::Name type, bool enable ) = 0;

	virtual void SetSourceBlendMode( AlphaBlend::Mode src_blend_mode ) = 0;

	virtual void SeDestBlendMode( AlphaBlend::Mode dest_blend_mode ) = 0;
};


#endif // __GraphicsDevice_HPP__
