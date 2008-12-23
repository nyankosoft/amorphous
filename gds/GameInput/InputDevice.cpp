#include "InputDevice.h"


CInputDevice::CInputDevice()
{
	InputDeviceHub().RegisterInputHandler( this );
}


CInputDevice::~CInputDevice()
{
	InputDeviceHub().UnregisterInputHandler( this );
}



void CInputDeviceHub::UnregisterInputHandler( CInputDevice *pDevice )
{
	for( size_t i=0; i<m_vecpInputDevice.size(); i++ )
	{
		if( m_vecpInputDevice[i] == pDevice )
		{
			m_vecpInputDevice.erase( m_vecpInputDevice.begin() + i );
		}
	}

}
