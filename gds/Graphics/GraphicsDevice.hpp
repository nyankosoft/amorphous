#ifndef __GraphicsDevice_HPP__
#define __GraphicsDevice_HPP__

#include <vector>
#include <string>
#include "../base.hpp"
#include "AlphaBlend.hpp"
#include "TextureFormat.hpp"
#include "TextureHandle.hpp"

#include "Support/Singleton.hpp"
using namespace NS_KGL;


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


class PrimitiveType
{
public:
	enum Name
	{
		TRIANGLE_LIST,
		TRIANGLE_FAN,
		TRIANGLE_STRIP,
		INVALID,
		NUM_PRIMITIVE_TYPES
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

	virtual Result::Name SetTexture( int stage, const CTextureHandle& texture ) = 0;

	inline Result::Name Enable( RenderStateType::Name type ) { return SetRenderState( type, true ); }

	inline Result::Name Disable( RenderStateType::Name type ) { return SetRenderState( type, false ); }

	virtual Result::Name SetRenderState( RenderStateType::Name type, bool enable ) = 0;

	virtual void SetSourceBlendMode( AlphaBlend::Mode src_blend_mode ) = 0;

	virtual void SetDestBlendMode( AlphaBlend::Mode dest_blend_mode ) = 0;
};


class CGraphicsDeviceHolder
{
	CGraphicsDevice *m_pGraphicsDevice;

protected:

	/// singleton
	static CSingleton<CGraphicsDeviceHolder> m_obj;

public:

	CGraphicsDeviceHolder()
		:
	m_pGraphicsDevice(NULL)
	{}

	static CGraphicsDeviceHolder* Get() { return m_obj.get(); }

	Result::Name SelectGraphicsDevice( const std::string& library_name );

	CGraphicsDevice& GetDevice() { return *m_pGraphicsDevice; }
};


inline CGraphicsDevice& GraphicsDevice()
{
	return CGraphicsDeviceHolder::Get()->GetDevice();
}



#endif // __GraphicsDevice_HPP__
