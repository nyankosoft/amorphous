#ifndef __DIRECT3D9_H__
#define __DIRECT3D9_H__

#include <d3d9.h>
#include <d3dx9.h>

//#pragma comment( lib, "d3dx9dt.lib" )
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )


#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }


#define DIRECT3D9 ( CDirect3D9::ms_CDirect3D9_ )
//#define DIRECT3D9 ( CDirect3D9::GetInstance() )


class CDirect3D9
{
private:

	LPDIRECT3D9			m_pD3D;

	LPDIRECT3DDEVICE9	m_pD3DDevice;

	D3DPRESENT_PARAMETERS m_CurrentPresentParameters; 

	D3DDEVTYPE m_DeviceType;

	DWORD m_BehaviorFlags;

	D3DFORMAT m_AdapterFormat; ///< the surface format of the display mode

private:

	void SetDefaultRenderStates();

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

	enum mode
	{
		FULLSCREEN,
		WINDOWED
	};

};


#endif // __DIRECT3D9_H__
