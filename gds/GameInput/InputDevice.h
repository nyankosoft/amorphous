#ifndef __InputDevice_H__
#define __InputDevice_H__


#include "InputHub.h"

/// auto repeat control requires the timer
#include "Support/Timer.h"


class CInputDevice
{
	enum Param
	{
		FIRST_AUTO_REPEAT_INTERVAL_MS = 300,
	};

public:

	CInputDevice();

	~CInputDevice();

	CInputState& InputState( int gi_code ) { return InputHub().m_aInputState[gi_code]; }

	inline void UpdateInputState( const SInputData& input_data );

	virtual Result::Name Init() { return Result::SUCCESS; }

	virtual Result::Name SendBufferedInputToInputHandlers() = 0;
};


inline void CInputDevice::UpdateInputState( const SInputData& input_data )
{
	TCFixedVector<int,CInputHub::NUM_MAX_SIMULTANEOUS_PRESSES>& pressed_key_list = InputHub().m_PressedKeyList;

	// access the input state holder in InputHub()
	CInputState& key = InputState( input_data.iGICode );

	if( input_data.iType == ITYPE_KEY_PRESSED )
	{
		key.m_State = CInputState::PRESSED;

		// schedule the first auto repeat event
		key.m_NextAutoRepeatTimeMS = GlobalTimer().GetTimeMS() + FIRST_AUTO_REPEAT_INTERVAL_MS;

		if( pressed_key_list.size() < CInputHub::NUM_MAX_SIMULTANEOUS_PRESSES )
			pressed_key_list.push_back( input_data.iGICode );
	}
	else // ( input_data.iType == ITYPE_KEY_RELEASED )
	{
		key.m_State = CInputState::RELEASED;

		// clear the key from the list of pressed keys
		for( int i=0; i<pressed_key_list.size(); i++ )
		{
			if( pressed_key_list[i] == input_data.iGICode )
				pressed_key_list.erase_at( i );
		}
	}
}


/// Used as a singleton class
class CInputDeviceHub
{
	std::vector<CInputDevice *> m_vecpInputDevice;

public:

	void RegisterInputHandler( CInputDevice *pDevice ) { m_vecpInputDevice.push_back( pDevice ); }

	void UnregisterInputHandler( CInputDevice *pDevice );

	void SendInputToInputHandlers()
	{
		for( size_t i=0; i<m_vecpInputDevice.size(); i++ )
		{
			m_vecpInputDevice[i]->SendBufferedInputToInputHandlers();
		}

		// Send key press input data (auto repeat)
		// InputHub().SendAutoRepeat();
	}
};


inline CInputDeviceHub& InputDeviceHub()
{
	static CInputDeviceHub s_instance;
	return s_instance;
}



#endif  /*  __InputDevice_H__  */
