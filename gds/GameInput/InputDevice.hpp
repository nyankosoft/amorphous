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

protected:

	virtual bool IsKeyPressed( int gi_code ) { return false; }

	virtual bool IsReleventInput( int gi_code ) { return false; }

	virtual void RefreshKeyStates() {}

public:

	CInputDevice();

	~CInputDevice();

	CInputState& InputState( int gi_code ) { return InputHub().m_aInputState[gi_code]; }

	inline void UpdateInputState( const SInputData& input_data );

	virtual Result::Name Init() { return Result::SUCCESS; }

	virtual Result::Name SendBufferedInputToInputHandlers() = 0;

	void CheckPressedKeys();
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
		{
			// Return if the key has already been registered as a 'pressed' key
			// because the same key cannot be pressed again before it is released
			// - This sould not happen, but does happen with gamepad. Why?
			for( int i=0; i<pressed_key_list.size(); i++ )
			{
				if( pressed_key_list[i] == input_data.iGICode )
					return;
			}

			// register this key as a 'pressed' key
			pressed_key_list.push_back( input_data.iGICode );
		}
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

	void SendInputToInputHandlers();
};


inline CInputDeviceHub& InputDeviceHub()
{
	static CInputDeviceHub s_instance;
	return s_instance;
}



#endif  /*  __InputDevice_H__  */
