#include "InputDevice.h"
#include "Support/Profile.h"


CInputDevice::CInputDevice()
{
	InputDeviceHub().RegisterInputHandler( this );
}


CInputDevice::~CInputDevice()
{
	InputDeviceHub().UnregisterInputHandler( this );
}


void CInputDevice::CheckPressedKeys()
{
	// execute key states update routine of each input device
	RefreshKeyStates();

	// check each key currently marked as pressed
	TCFixedVector<int,CInputHub::NUM_MAX_SIMULTANEOUS_PRESSES>& pressed_key = InputHub().m_PressedKeyList;
	int i, num_pressed_keys = pressed_key.size();
	for( i=0; i<num_pressed_keys; /* Do not increment i here */)
	{
		if( !IsReleventInput( pressed_key[i] ) )
		{
			i++;
			continue;
		}

		if( !IsKeyPressed( pressed_key[i] ) )
		{
			// The key is marked as pressed but not actually pressed right now
			// - Probably the release event has been missed
			// - Send the release event
			SInputData input;
			input.iType = ITYPE_KEY_RELEASED;
			input.fParam1 = 0.0f;
			InputHub().UpdateInput( input );
			// UpdateInputState( input );

			pressed_key.erase_at( i );
			
			// NOTE:
			// pressed_key was modified
			// since the released key is removed from the list
			// refresh the num
			num_pressed_keys = pressed_key.size();
		}
		else
			i++;
	}

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


void CInputDeviceHub::SendInputToInputHandlers()
{
	PROFILE_FUNCTION();

	for( size_t i=0; i<m_vecpInputDevice.size(); i++ )
	{
		m_vecpInputDevice[i]->SendBufferedInputToInputHandlers();
	}

	// Send key press input data (auto repeat)
	InputHub().SendAutoRepeat();

	{
		PROFILE_SCOPE( "input devices - CheckPressedKeys()" );

		// Check if the keys currently marked as pressed are actually being pressed
		for( size_t i=0; i<m_vecpInputDevice.size(); i++ )
		{
			m_vecpInputDevice[i]->CheckPressedKeys();
		}
	}
}
