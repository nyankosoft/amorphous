#include "InputDevice.hpp"
#include "InputHub.hpp"
#include "ForceFeedback/ForceFeedbackEffect.hpp"
#include "Support/StringAux.hpp"
#include "Support/Profile.hpp"

using namespace std;
using namespace boost;


//=============================================================================
// CInputDevice
//=============================================================================

CInputDevice::CInputDevice()
:
m_pGroup(NULL)
{
	InputDeviceHub().RegisterInputDevice( this );

//	InputDeviceHub().RegisterInputDeviceToGroup( this ); // pure virtual function call
}


CInputDevice::~CInputDevice()
{
	InputDeviceHub().UnregisterInputDevice( this );

	InputDeviceHub().UnregisterInputDeviceFromGroup( this );
}


void CInputDevice::UpdateInputState( const SInputData& input_data )
{
	if( !m_pGroup )
		return;

	TCFixedVector<int,CInputDeviceParam::NUM_MAX_SIMULTANEOUS_PRESSES>& pressed_key_list = PressedKeyList();

	// access the input state holder in InputHub()
	CInputState& key = InputState( input_data.iGICode );

	if( input_data.iType == ITYPE_KEY_PRESSED )
	{
		key.m_State = CInputState::PRESSED;

		// schedule the first auto repeat event
		key.m_NextAutoRepeatTimeMS = GlobalTimer().GetTimeMS() + FIRST_AUTO_REPEAT_INTERVAL_MS;

		if( pressed_key_list.size() < CInputDeviceParam::NUM_MAX_SIMULTANEOUS_PRESSES )
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


void CInputDevice::CheckPressedKeys()
{
	if( !m_pGroup )
		return;

	// execute key states update routine of each input device
	RefreshKeyStates();

	// check each key currently marked as pressed
	TCFixedVector<int,CInputDeviceParam::NUM_MAX_SIMULTANEOUS_PRESSES>& pressed_key = PressedKeyList();
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


void CInputDevice::SetGroup( CInputDeviceGroup *pGroup )
{
	m_pGroup = pGroup;
}


CForceFeedbackEffect CInputDevice::CreateForceFeedbackEffect( const CForceFeedbackEffectDesc& desc )
{
	return CForceFeedbackEffect();
}


void CInputDevice::SetImplToForceFeedbackEffect( boost::shared_ptr<CForceFeedbackEffectImpl> pImpl, CForceFeedbackEffect& ffe )
{
	ffe.m_pImpl = pImpl;
}


//=============================================================================
// CInputDeviceHub
//=============================================================================

CInputDeviceHub::CInputDeviceHub()
{
	m_vecpGroup.resize( 1 );
	m_vecpGroup.back().reset( new CInputDeviceGroup() );
}


void CInputDeviceHub::RegisterInputDevice( CInputDevice *pDevice )
{
	boost::mutex::scoped_lock(m_Mutex);

	m_vecpInputDevice.push_back( pDevice );
}


void CInputDeviceHub::RegisterInputDeviceToGroup( CInputDevice *pDevice )
{
	switch( pDevice->GetInputDeviceType() )
	{
	case CInputDevice::TYPE_GAMEPAD:
	{
		// one gamepad per group
		size_t i = 0;
		for( i=0; i<m_vecpGroup.size(); i++ )
		{
			size_t j = 0;
			for( j=0; j<m_vecpGroup[i]->m_vecpDevice.size(); j++ )
			{
				if( m_vecpGroup[i]->m_vecpDevice[j]->GetInputDeviceType() == CInputDevice::TYPE_GAMEPAD )
					break;
			}

			if( j == m_vecpGroup[i]->m_vecpDevice.size() )
			{
				// no gamepad is in this group
				m_vecpGroup[i]->m_vecpDevice.push_back( pDevice );
				pDevice->SetGroup( m_vecpGroup[i].get() );
				break;
			}
		}

		if( m_vecpGroup.size() == i )	
		{
			boost::shared_ptr<CInputDeviceGroup> pGroup( new CInputDeviceGroup() );
			m_vecpGroup.push_back( pGroup );
			m_vecpGroup.back()->m_vecpDevice.push_back( pDevice );
			pDevice->SetGroup( m_vecpGroup.back().get() );
		}
	}
		break;
	case CInputDevice::TYPE_KEYBOARD:
		m_vecpGroup[0]->m_vecpDevice.push_back( pDevice );
		pDevice->SetGroup( m_vecpGroup[0].get() );
		break;
	case CInputDevice::TYPE_MOUSE:
		m_vecpGroup[0]->m_vecpDevice.push_back( pDevice );
		pDevice->SetGroup( m_vecpGroup[0].get() );
		break;
	default:
		break;
	}
}


void CInputDeviceHub::UnregisterInputDeviceFromGroup( CInputDevice *pDevice )
{
}


void CInputDeviceHub::UnregisterInputDevice( CInputDevice *pDevice )
{
	boost::mutex::scoped_lock(m_Mutex);

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
	SendAutoRepeat();

	{
		PROFILE_SCOPE( "input devices - CheckPressedKeys()" );

		// Check if the keys currently marked as pressed are actually being pressed
		for( size_t i=0; i<m_vecpInputDevice.size(); i++ )
		{
			m_vecpInputDevice[i]->CheckPressedKeys();
		}
	}
}


void CInputDeviceHub::SendAutoRepeat()
{
	for( size_t i=0; i<m_vecpGroup.size(); i++ )
	{
		SendAutoRepeat( *(m_vecpGroup[i].get()) );
	}
}


void CInputDeviceHub::SendAutoRepeat( CInputDeviceGroup& group )
{
	int i, num_keys = group.m_PressedKeyList.size();
	for( i=0; i<num_keys; i++ )
	{
		unsigned long current_time = GlobalTimer().GetTimeMS();
		SInputData input_data;

		CInputState& pressed_key_state = group.m_aInputState[ group.m_PressedKeyList[i] ];
		if( pressed_key_state.m_NextAutoRepeatTimeMS <= current_time )
		{
			// Passed the scheduled time
			// - send an auto repeat key press event
			input_data.iGICode = group.m_PressedKeyList[i];
			input_data.iType = ITYPE_KEY_PRESSED;
			input_data.fParam1 = 1.0f;

			InputHub().SendAutoRepeatInputToInputHandlers( input_data );

			// set the next auto repeat scheduled time
			unsigned long auto_repeat_interval_error = 50;
			if( current_time - pressed_key_state.m_NextAutoRepeatTimeMS < auto_repeat_interval_error )
			{
				// Let's assume that the app is running at a reasonable frame rate
				// - So, ...
				pressed_key_state.m_NextAutoRepeatTimeMS += CInputHub::AUTO_REPEAT_INTERVAL_MS;
			}
			else
			{
				// Probably the frame rate is too low.
				// - So, ...
				pressed_key_state.m_NextAutoRepeatTimeMS = current_time + CInputHub::AUTO_REPEAT_INTERVAL_MS;
			}
		}
	}
}


void CInputDeviceHub::GetInputDeviceStatus( std::vector<std::string>& dest_text_buffer )
{
	boost::mutex::scoped_lock(m_Mutex);

	dest_text_buffer.resize( 2 );
	dest_text_buffer[0] = fmt_string( "Input Device(s) (%d found)\n", (int)m_vecpInputDevice.size() );
	dest_text_buffer[1] = "----------------------------------------\n";

	vector<string> buffer;
	for( size_t i=0; i<m_vecpInputDevice.size(); i++ )
	{
		buffer.resize( 0 );

		m_vecpInputDevice[i]->GetStatus( buffer );

		dest_text_buffer.insert( dest_text_buffer.end(), buffer.begin(), buffer.end() );
	}
}
