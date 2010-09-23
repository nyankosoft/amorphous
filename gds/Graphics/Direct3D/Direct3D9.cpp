#include "Direct3D9.hpp"

#include "Graphics/Direct3D/D3DSurfaceFormat.hpp"
#include "Graphics/FogParams.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Macro.h"


using namespace std;


const char *hr_d3d_error_to_string(HRESULT hr)
{
	switch(hr)
	{
	case D3DERR_DEVICELOST:          return "D3DERR_DEVICELOST";
	case D3DERR_DRIVERINTERNALERROR: return "D3DERR_DRIVERINTERNALERROR";
	case D3DERR_INVALIDCALL:         return "D3DERR_INVALIDCALL";
	case D3DERR_NOTAVAILABLE:        return "D3DERR_NOTAVAILABLE";
	case D3DERR_OUTOFVIDEOMEMORY:    return "D3DERR_OUTOFVIDEOMEMORY";
	case E_OUTOFMEMORY:              return "E_OUTOFMEMORY";
	case D3DXERR_INVALIDDATA:        return "D3DXERR_INVALIDDATA";
//	case : return "";
//	case : return "";
	default: return "Unknown";
	}

	return "Unknown";
}


static const char *GetDeviceTypeString( D3DDEVTYPE dev_type )
{
	switch(dev_type)
	{
	case D3DDEVTYPE_HAL:     return "D3DDEVTYPE_HAL";
	case D3DDEVTYPE_NULLREF: return "D3DDEVTYPE_NULLREF";
	case D3DDEVTYPE_REF:     return "D3DDEVTYPE_REF";
	case D3DDEVTYPE_SW:      return "D3DDEVTYPE_SW";
	default: return "invalid";
	}
}

static const char *GetVertexProcessingTypeString( DWORD behavior_flags )
{
	if(behavior_flags & D3DCREATE_PUREDEVICE)                     return "Pure Device";
	else if(behavior_flags & D3DCREATE_HARDWARE_VERTEXPROCESSING) return "Hardware";
	else if(behavior_flags & D3DCREATE_SOFTWARE_VERTEXPROCESSING) return "Software";
	else if(behavior_flags & D3DCREATE_MIXED_VERTEXPROCESSING)    return "Mixed";
	else return "unkonwn";
}



//========================================================================
// CDirect3D9
//========================================================================

// definition of singleton instance
CDirect3D9 CDirect3D9::ms_CDirect3D9_;


CDirect3D9::CDirect3D9()
:
m_pD3D( NULL ),
m_pD3DDevice( NULL ),
m_DeviceType(D3DDEVTYPE_HAL),
m_BehaviorFlags(0)
{
	m_CurrentDepthBufferType = D3DZB_TRUE;
}


bool CDirect3D9::InitD3D( HWND hWnd, int iWindowWidth, int iWindowHeight, int screen_mode )
{
	if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		LOG_PRINT_ERROR( "Direct3DCreate9() failed." );
		return false;
	}

	// create D3D device

	D3DDISPLAYMODE d3ddm;
	if( FAILED( m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
	{
		return false;
	}

	// save the surface format of the display mode
	m_AdapterFormat = d3ddm.Format;


	D3DPRESENT_PARAMETERS D3DPresentParam; 

	ZeroMemory( &D3DPresentParam, sizeof( D3DPresentParam ) );
	D3DPresentParam.BackBufferHeight				= iWindowHeight;
	D3DPresentParam.BackBufferWidth					= iWindowWidth;
	D3DPresentParam.BackBufferFormat				= d3ddm.Format;
//	D3DPresentParam.BackBufferFormat				= D3DFMT_A8R8G8B8;
	D3DPresentParam.BackBufferCount					= 1;
	D3DPresentParam.MultiSampleType					= D3DMULTISAMPLE_NONE;
//	D3DPresentParam.SwapEffect						= D3DSWAPEFFECT_COPY;	//original
	D3DPresentParam.SwapEffect						= D3DSWAPEFFECT_DISCARD;
	D3DPresentParam.hDeviceWindow					= hWnd;
	D3DPresentParam.Windowed						= ( screen_mode == WINDOWED ? TRUE : FALSE );
	D3DPresentParam.EnableAutoDepthStencil			= TRUE;
//	D3DPresentParam.AutoDepthStencilFormat			= D3DFMT_D16;
	D3DPresentParam.AutoDepthStencilFormat			= D3DFMT_D24S8;
	D3DPresentParam.Flags							= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	D3DPresentParam.FullScreen_RefreshRateInHz		= D3DPRESENT_RATE_DEFAULT;
//	D3DPresentParam.PresentationInterval			= D3DPRESENT_INTERVAL_DEFAULT;
	D3DPresentParam.PresentationInterval			= D3DPRESENT_INTERVAL_IMMEDIATE;

	// save as current present parameters
	m_CurrentPresentParameters = D3DPresentParam;

	// create D3D device

	bool res;
	res = CreateD3DDevice( D3DPresentParam, hWnd );
	if( !res )
		return false;

	// set up default render states
	SetDefaultRenderStates();

	// set up default camera matrix
	D3DXMATRIXA16 matView;
	D3DXVECTOR3 vEye( 0, 0, 3 );
	D3DXVECTOR3 vAt( 0, 0.7f, 0 );
	D3DXVECTOR3 vUp( 0, 1, 0 );
	D3DXMatrixLookAtLH( &matView, &vEye, &vAt, &vUp);

	m_pD3DDevice->SetTransform( D3DTS_VIEW,  &matView );

    // set up default projection matrix
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 640.0f / 480.0f, 0.5f, 500.0f );
    m_pD3DDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	m_State = CGraphicsDevice::STATE_INITIALIZED;

	return true;
}


bool CDirect3D9::CreateD3DDevice( D3DPRESENT_PARAMETERS& present_params, HWND hWnd )
{
	SAFE_RELEASE( m_pD3DDevice );

	const int num_params_sets_to_try = 3;

	D3DDEVTYPE device_types[] =
	{
		D3DDEVTYPE_HAL, // most desirable
		D3DDEVTYPE_HAL,
		D3DDEVTYPE_REF  // least desirable
	};

	DWORD behavior_flags[] =
	{
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, // most desirable
		D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED  // least desirable
	};

	HRESULT hr;

	// Start with the most desirable settings
	for( int i=0; i<num_params_sets_to_try; i++ )
	{
		hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT,
		device_types[i],
		hWnd,
		behavior_flags[i],
		&present_params,
		&m_pD3DDevice );
		
		if( SUCCEEDED(hr) )
		{
			// save params
			m_DeviceType    = device_types[i];
			m_BehaviorFlags = behavior_flags[i];
			m_CurrentPresentParameters = present_params;

			LOG_PRINT( fmt_string( "Created a D3D device. device type: %s / vertex processing: %s",
			GetDeviceTypeString(m_DeviceType),
			GetVertexProcessingTypeString(m_BehaviorFlags) ) );

			// update adapter modes
			EnumAdapterModesForDefaultAdapter();

			return true;
		}
		else
		{
			LOG_PRINT_WARNING( fmt_string( "CreateDevice() failed with the next params: device type = %s, behavior flags = %s. Error: %s",
			GetDeviceTypeString(m_DeviceType),
			GetVertexProcessingTypeString(m_BehaviorFlags),
			hr_d3d_error_to_string(hr) ) );
		}
	}

	// LOG_PRINT_WARNING( " - Hardware vertex processing is not available." );

	return false;
}


bool CDirect3D9::ResetD3DDevice( HWND hWnd, int iWindowWidth, int iWindowHeight, bool bFullScreen )
{
	D3DPRESENT_PARAMETERS present_param = m_CurrentPresentParameters;

	// modify the current presentation parameters
	present_param.BackBufferHeight			= iWindowHeight;
	present_param.BackBufferWidth			= iWindowWidth;
	present_param.BackBufferCount			= 1;
	present_param.Windowed					= !bFullScreen;
	present_param.hDeviceWindow				= hWnd;
//	present_param.AutoDepthStencilFormat	= D3DFMT_D16;
	present_param.AutoDepthStencilFormat	= D3DFMT_D24S8;

//	if( !bFullScreen )
//		present_param.BackBufferFormat			= D3DFMT_UNKNOWN;

	// reset D3D device
//	HRESULT hr = m_pD3DDevice->Reset(&present_param);

	return CreateD3DDevice( present_param, hWnd );
}


void CDirect3D9::SetDefaultRenderStates()
{
	// some default render states
	m_pD3DDevice->SetRenderState( D3DRS_LIGHTING,         FALSE );
	m_pD3DDevice->SetRenderState( D3DRS_DITHERENABLE,     TRUE );
	m_pD3DDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
	m_pD3DDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_NONE );
	m_pD3DDevice->SetRenderState( D3DRS_AMBIENT,          0x33333333 );
	m_pD3DDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
	m_pD3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
}


void CDirect3D9::Release()
{
	if( m_pD3DDevice ) m_pD3DDevice->Release();
	if( m_pD3D ) m_pD3D->Release();
}


void CDirect3D9::GetAdapterModesForDefaultAdapter( std::vector<CAdapterMode>& dest_buffer )
{
	if( !m_pD3D || !m_pD3DDevice )
		LOG_PRINT_ERROR( "Not initialized yet." );

	if( m_vecAdapterMode.size() == 0 )
		LOG_PRINT_WARNING( "No adapeter modes are in m_vecAdapterMode." );

	dest_buffer = m_vecAdapterMode;
}


bool CDirect3D9::IsCurrentDisplayMode( const CDisplayMode& display_mode ) const
{
	const D3DPRESENT_PARAMETERS& present_params = m_CurrentPresentParameters;
	if( display_mode.Width  == present_params.BackBufferWidth
	 && display_mode.Height == present_params.BackBufferHeight
	 && display_mode.Format == FromD3DSurfaceFormat( present_params.BackBufferFormat ) )
	{
		// What about refresh rates?
		return true;
	}
	else
	{
		return false;
	}
}


void CDirect3D9::EnumAdapterModesForDefaultAdapter()
{
	const D3DFORMAT allowable_formats[] =
	{
		D3DFMT_A1R5G5B5,
//		D3DFMT_A2R10G10B10,
		D3DFMT_A8R8G8B8,
		D3DFMT_R5G6B5,
		D3DFMT_X1R5G5B5,
		D3DFMT_X8R8G8B8
	};

	for(int i=0; i<numof(allowable_formats); i++)
	{
		m_vecAdapterMode.push_back( CAdapterMode(FromD3DSurfaceFormat(allowable_formats[i])) );
		CAdapterMode& adapter_mode = m_vecAdapterMode.back();

		uint num_adapter_modes = m_pD3D->GetAdapterModeCount( D3DADAPTER_DEFAULT, allowable_formats[i] );

		for(uint j=0;j<num_adapter_modes;j++)
		{
			D3DDISPLAYMODE mode;
			m_pD3D->EnumAdapterModes( D3DADAPTER_DEFAULT, allowable_formats[i], j, &mode );

			// add to the list
			adapter_mode.vecDisplayMode.push_back( CDisplayMode(
			mode.Width,
			mode.Height,
			mode.RefreshRate,
			FromD3DSurfaceFormat(mode.Format) )
			);

			// check if this is the current display mode
			if( IsCurrentDisplayMode( adapter_mode.vecDisplayMode.back() ) )
				adapter_mode.vecDisplayMode.back().Current = true;
				
		}
	}
}


Result::Name CDirect3D9::SetTexture( int stage, const CTextureHandle& texture )
{
	if( !m_pD3DDevice )
		return Result::UNKNOWN_ERROR;

	HRESULT hr = m_pD3DDevice->SetTexture( stage, texture.GetTexture() );

	return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


static inline D3DCULL ToD3DCullMode( CullingMode::Name cull_mode )
{
	switch( cull_mode )
	{
	case CullingMode::CLOCKWISE:        return D3DCULL_CW;
	case CullingMode::COUNTERCLOCKWISE: return D3DCULL_CCW;
//	case CullingMode::NONE:             return D3DCULL_NONE;
	default: return D3DCULL_NONE;
	}
}


static inline bool ToD3DRenderStateType( RenderStateType::Name type, D3DRENDERSTATETYPE& dest )
{
	switch(type)
	{
	case RenderStateType::ALPHA_BLEND:               dest = D3DRS_ALPHABLENDENABLE; return true;
	case RenderStateType::ALPHA_TEST:                dest = D3DRS_ALPHATESTENABLE;  return true;
	case RenderStateType::LIGHTING:                  dest = D3DRS_LIGHTING;		    return true;
	case RenderStateType::FOG:                       dest = D3DRS_FOGENABLE;	    return true;
	case RenderStateType::WRITING_INTO_DEPTH_BUFFER: dest = D3DRS_ZWRITEENABLE;	    return true;
	default:
		return false;
	}
}


Result::Name CDirect3D9::SetRenderState( RenderStateType::Name type, bool enable )
{
	if( !m_pD3DDevice )
		return Result::UNKNOWN_ERROR;

	D3DRENDERSTATETYPE d3d_rst;
	HRESULT hr = S_OK;
	bool res = ToD3DRenderStateType( type, d3d_rst );
	if( res )
	{
		hr = m_pD3DDevice->SetRenderState( d3d_rst, enable ? TRUE : FALSE );

		if( SUCCEEDED(hr) )
			return Result::SUCCESS;
		else
			return Result::UNKNOWN_ERROR;
	}

	switch(type)
	{
	case RenderStateType::DEPTH_TEST:
		if( enable )
			m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, m_CurrentDepthBufferType );
		else
			m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
		break;

	case RenderStateType::FACE_CULLING:
		if( enable )
			m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, ToD3DCullMode(m_CullMode) );
		else
			m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		break;

	default:
		return Result::INVALID_ARGS;
	}

	return Result::UNKNOWN_ERROR;
}


void CDirect3D9::SetSourceBlendMode( AlphaBlend::Mode src_blend_mode )
{
	if( m_pD3DDevice )
		m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, g_dwD3DBlendMode[src_blend_mode] );
}


void CDirect3D9::SetDestBlendMode( AlphaBlend::Mode dest_blend_mode )
{
	if( m_pD3DDevice )
		m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, g_dwD3DBlendMode[dest_blend_mode] );
}


static inline D3DFOGMODE ToD3DFogMode( FogMode::Name fog_mode )
{
	switch( fog_mode )
	{
	case FogMode::LINEAR: return D3DFOG_LINEAR;
	case FogMode::EXP:    return D3DFOG_EXP;
	case FogMode::EXP2:   return D3DFOG_EXP2;
	default: return D3DFOG_LINEAR;
	}
}


Result::Name CDirect3D9::SetFogParams( const CFogParams& fog_params )
{
	HRESULT hr = S_OK;

	hr = m_pD3DDevice->SetRenderState( D3DRS_FOGTABLEMODE, ToD3DFogMode(fog_params.Mode) );

	D3DCOLOR argb32 = fog_params.Color.GetARGB32();
	hr = m_pD3DDevice->SetRenderState( D3DRS_FOGCOLOR,   argb32 );

	float start = (float)fog_params.Start; 
	hr = m_pD3DDevice->SetRenderState( D3DRS_FOGSTART,   *(DWORD *)&start );

	float end = (float)fog_params.End;
	hr = m_pD3DDevice->SetRenderState( D3DRS_FOGEND,     *(DWORD *)&end );

	float density = (float)fog_params.Density;
	hr = m_pD3DDevice->SetRenderState( D3DRS_FOGDENSITY, *(DWORD *)&density );

	// Always use pixel fog and do not change params
//	hr = m_pD3DDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_NONE );
//	hr = m_pD3DDevice->SetRenderState( D3DRS_RANGEFOGENABLE, FALSE );

	return Result::SUCCESS;
}


Result::Name CDirect3D9::SetCullingMode( CullingMode::Name cull_mode )
{
	m_CullMode = cull_mode;

	HRESULT hr = S_OK;

	DWORD current_d3d_cull_mode = D3DCULL_NONE;
	hr = m_pD3DDevice->GetRenderState( D3DRS_CULLMODE, &current_d3d_cull_mode );
	D3DCULL dest_d3d_cull_mode = ToD3DCullMode(cull_mode);
	if( current_d3d_cull_mode != dest_d3d_cull_mode )
	{
		hr = m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, dest_d3d_cull_mode );
	}

	return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


Result::Name CDirect3D9::GetViewport( CViewport& viewport )
{
	D3DVIEWPORT9 vp;
	HRESULT hr = m_pD3DDevice->GetViewport( &vp );
	viewport.UpperLeftX = (uint)vp.X;
	viewport.UpperLeftY = (uint)vp.Y;
	viewport.Width      = (uint)vp.Width;
	viewport.Height     = (uint)vp.Height;
	viewport.MinDepth   = vp.MinZ;
	viewport.MaxDepth   = vp.MaxZ;

	return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


Result::Name CDirect3D9::SetViewport( const CViewport& viewport )
{
	D3DVIEWPORT9 vp;
	vp.X      = (DWORD)viewport.UpperLeftX;
	vp.Y      = (DWORD)viewport.UpperLeftY;
	vp.Width  = (DWORD)viewport.Width;
	vp.Height = (DWORD)viewport.Height;
	vp.MinZ   = viewport.MinDepth;
	vp.MaxZ   = viewport.MaxDepth;
	HRESULT hr = m_pD3DDevice->SetViewport( &vp );

	return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


Result::Name CDirect3D9::SetClearColor( const SFloatRGBAColor& color )
{
	m_ClearColor = color.GetARGB32();
	return Result::SUCCESS;
}


Result::Name CDirect3D9::SetClearDepth( float depth )
{
	m_fClearDepth = depth;
	return Result::SUCCESS;
}


Result::Name CDirect3D9::Clear( U32 buffer_mask )
{
	DWORD mask = 0;
	if( buffer_mask & BufferMask::COLOR )   mask |= D3DCLEAR_TARGET;
	if( buffer_mask & BufferMask::DEPTH )   mask |= D3DCLEAR_ZBUFFER;
	if( buffer_mask & BufferMask::STENCIL ) mask |= D3DCLEAR_STENCIL;

	HRESULT hr = m_pD3DDevice->Clear(
		0,
		NULL,
		mask,
		m_ClearColor,
		m_fClearDepth,
		0 );

	return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}
