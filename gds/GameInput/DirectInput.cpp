#include "DirectInput.h"

// definition of the singleton instance
CDirectInput CDirectInput::ms_SingletonInstance_;

CDirectInput::CDirectInput()
: m_pDI(NULL)
{
}


#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

CDirectInput::~CDirectInput()
{
	if( m_pDI )
	{
		m_pDI->Release();
		m_pDI = NULL;
	}
}

HRESULT CDirectInput::InitDirectInput()
{
	HRESULT hr;

    // Create a DInput object
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&m_pDI, NULL ) ) )
        return hr;

	return S_OK;
}

LPDIRECTINPUT8 CDirectInput::GetDirectInputObject()
{
	if( !m_pDI )
		InitDirectInput();

	return m_pDI;
}
