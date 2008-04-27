#include "Direct3D9.h"
#include <d3dx9.h>

#include "Support/Log/DefaultLog.h"

CDirect3D9 CDirect3D9::ms_CDirect3D9_;


CDirect3D9::CDirect3D9()
: m_pD3D( NULL ), m_pD3DDevice( NULL )
{
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
	if(behavior_flags & D3DCREATE_HARDWARE_VERTEXPROCESSING)      return "HW";
	else if(behavior_flags & D3DCREATE_SOFTWARE_VERTEXPROCESSING) return "SW";
	else if(behavior_flags & D3DCREATE_MIXED_VERTEXPROCESSING)    return "mixed";
	else return "unkonwn";
}


bool CDirect3D9::InitD3D( HWND hWnd, int iWindowWidth, int iWindowHeight, int screen_mode )
{
	if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return false;

	// create D3D device

	D3DDISPLAYMODE d3ddm;
	if( FAILED( m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) ){
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

	HRESULT hr;

	m_DeviceType    = D3DDEVTYPE_HAL;
	m_BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;// | D3DCREATE_MULTITHREADED;

	hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, m_DeviceType, hWnd, m_BehaviorFlags,
					  &D3DPresentParam, &m_pD3DDevice );

	if( FAILED(hr) )
	{
		LOG_PRINT_WARNING( " - Hardware vertex processing is not available." );

		m_BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;// | D3DCREATE_MULTITHREADED;

		hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, m_DeviceType, hWnd, m_BehaviorFlags,
						  &D3DPresentParam, &m_pD3DDevice );

		if( FAILED(hr) )
		{
			// try the reference rasterizer
			m_DeviceType = D3DDEVTYPE_REF;
			hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, m_DeviceType, hWnd, m_BehaviorFlags,
						  &D3DPresentParam, &m_pD3DDevice );

			if( FAILED(hr) )
                return false;
		}
	}

	g_Log.Print( "CDirect3D9::InitD3D() - created D3D device. device type: %s / vertex processing: %s",
		GetDeviceTypeString(m_DeviceType),
		GetVertexProcessingTypeString(m_BehaviorFlags) );


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

	return true;
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

	SAFE_RELEASE( m_pD3DDevice );

	if( FAILED( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
					  D3DCREATE_HARDWARE_VERTEXPROCESSING,
					  &present_param, &m_pD3DDevice ) ) )
	{
//		MessageBox(NULL, "Hardware vertex processing is not available.", "Bad News", MB_ICONWARNING|MB_OK);
		if( FAILED( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
						  D3DCREATE_SOFTWARE_VERTEXPROCESSING,
						  &present_param, &m_pD3DDevice ) ) )
		{
			return false;
		}
	}
	
/*	if( hr == D3DERR_DEVICELOST )
		return false;
	else if( hr == D3DERR_DRIVERINTERNALERROR )
		return false;
	else if( hr == D3DERR_INVALIDCALL )
		return false;
	else if( hr == D3DERR_OUTOFVIDEOMEMORY )
		return false;
	else if( hr == E_OUTOFMEMORY )
		return false;

	if( FAILED(hr) )
		return false;*/

	m_CurrentPresentParameters = present_param;

	SetDefaultRenderStates();

	return true;
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
