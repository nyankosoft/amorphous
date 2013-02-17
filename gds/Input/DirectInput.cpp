#include "DirectInput.hpp"


namespace amorphous
{

// definition of the singleton instance
DirectInput DirectInput::ms_SingletonInstance_;

DirectInput::DirectInput()
: m_pDI(NULL)
{
}


#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

DirectInput::~DirectInput()
{
	if( m_pDI )
	{
		m_pDI->Release();
		m_pDI = NULL;
	}
}

HRESULT DirectInput::InitDirectInput()
{
	HRESULT hr;

    // Create a DInput object
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&m_pDI, NULL ) ) )
        return hr;

	return S_OK;
}

LPDIRECTINPUT8 DirectInput::GetDirectInputObject()
{
	if( !m_pDI )
		InitDirectInput();

	return m_pDI;
}


} // namespace amorphous
