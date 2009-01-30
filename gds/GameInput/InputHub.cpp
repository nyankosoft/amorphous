#include "InputHub.hpp"
#include "Support/Timer.hpp"


CInputHub CInputHub::ms_InputHub_Instance_;


CInputHub::CInputHub()
{
	// all the stacks have NULL at the bottom of the stack
//	int i, num = NUM_MAX_INPUT_HANDLERS;
//	for( i=0; i<num; i++ )
//		m_vecpInputHandler[i].push_back( NULL );

//	m_NumInputHandlers = 0;

//	m_pInputHandler = NULL;
}


CInputHub::~CInputHub()
{
//	ReleaseInputHandlers();
}

/*
void CInputHub::ReleaseInputHandlers()
{
	for( int i=0; i<NUM_MAX_INPUT_HANDLERS; i++ )
		SafeDelete( m_apInputHandler[i] );
}
*/


void CInputHub::SendAutoRepeat()
{
	int i, num_keys = m_PressedKeyList.size();
	for( i=0; i<num_keys; i++ )
	{
		unsigned long current_time = GlobalTimer().GetTimeMS();
		SInputData input_data;

		CInputState& pressed_key_state = m_aInputState[ m_PressedKeyList[i] ];
		if( pressed_key_state.m_NextAutoRepeatTimeMS <= current_time )
		{
			// Passed the scheduled time
			// - send an auto repeat key press event
			input_data.iGICode = m_PressedKeyList[i];
			input_data.iType = ITYPE_KEY_PRESSED;
			input_data.fParam1 = 1.0f;

			SendAutoRepeatInputToInputHandlers( input_data );

			// set the next auto repeat scheduled time
			unsigned long auto_repeat_interval_error = 50;
			if( current_time - pressed_key_state.m_NextAutoRepeatTimeMS < auto_repeat_interval_error )
			{
				// Let's assume that the app is running at a reasonable frame rate
				// - So, ...
				pressed_key_state.m_NextAutoRepeatTimeMS += AUTO_REPEAT_INTERVAL_MS;
			}
			else
			{
				// Probably the frame rate is too low.
				// - So, ...
				pressed_key_state.m_NextAutoRepeatTimeMS = current_time + AUTO_REPEAT_INTERVAL_MS;
			}
		}
	}
}
