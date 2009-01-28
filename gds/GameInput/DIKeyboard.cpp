#include "DIKeyboard.h"

#include "../base.h"
#include "DirectInput.h"
#include "InputHub.h"
#include "App/GameWindowManager_Win32.h"


#define DIKEYBOARD_BUFFER_SIZE 32  // arbitrary number of buffer elements


CDIKeyboard::CDIKeyboard()
: m_pKeyboard(NULL)
{
	InitKeyCodeMap();
}


CDIKeyboard::~CDIKeyboard()
{
	Release();
}


static int gs_aGICodeToDIKCode[0xFF];


/**
 Initializes the mapping tables
 - maps from DTK_ codes to GIC_ codes
 - maps from GIC_ codes to DIK_ codes
 */
void CDIKeyboard::InitKeyCodeMap()
{
	// Mappings from DIK codes to GI codes

	for( int i=0; i<NUM_SUPPORTED_DIK_CODES; i++ )
		m_aiGICodeForDIKCode[i] = GIC_INVALID;

	m_aiGICodeForDIKCode[DIK_F1]    = GIC_F1;
	m_aiGICodeForDIKCode[DIK_F2]    = GIC_F2;
	m_aiGICodeForDIKCode[DIK_F3]    = GIC_F3;
	m_aiGICodeForDIKCode[DIK_F4]    = GIC_F4;
	m_aiGICodeForDIKCode[DIK_F5]    = GIC_F5;
	m_aiGICodeForDIKCode[DIK_F6]    = GIC_F6;
	m_aiGICodeForDIKCode[DIK_F7]    = GIC_F7;
	m_aiGICodeForDIKCode[DIK_F8]    = GIC_F8;
	m_aiGICodeForDIKCode[DIK_F9]    = GIC_F9;
	m_aiGICodeForDIKCode[DIK_F10]   = GIC_F10;
	m_aiGICodeForDIKCode[DIK_F11]   = GIC_F11;
	m_aiGICodeForDIKCode[DIK_F12]   = GIC_F12;

	m_aiGICodeForDIKCode[DIK_RETURN]   = GIC_ENTER;
	m_aiGICodeForDIKCode[DIK_SPACE]    = GIC_SPACE;
	m_aiGICodeForDIKCode[DIK_LSHIFT]   = GIC_LSHIFT;
	m_aiGICodeForDIKCode[DIK_RSHIFT]   = GIC_RSHIFT;
	m_aiGICodeForDIKCode[DIK_RCONTROL] = GIC_RCONTROL,
	m_aiGICodeForDIKCode[DIK_LCONTROL] = GIC_LCONTROL;
	m_aiGICodeForDIKCode[DIK_RALT]     = GIC_RALT,
	m_aiGICodeForDIKCode[DIK_LALT]     = GIC_LALT;
	m_aiGICodeForDIKCode[DIK_TAB]      = GIC_TAB;
	m_aiGICodeForDIKCode[DIK_BACK]     = GIC_BACK;
	m_aiGICodeForDIKCode[DIK_ESCAPE]   = GIC_ESC;
	m_aiGICodeForDIKCode[DIK_UP]       = GIC_UP;
	m_aiGICodeForDIKCode[DIK_DOWN]     = GIC_DOWN;
	m_aiGICodeForDIKCode[DIK_RIGHT]    = GIC_RIGHT;
	m_aiGICodeForDIKCode[DIK_LEFT]     = GIC_LEFT;
	m_aiGICodeForDIKCode[DIK_NOCONVERT] = GIC_NOCONVERT;

	m_aiGICodeForDIKCode[DIK_A]   = 'A';
	m_aiGICodeForDIKCode[DIK_B]   = 'B';
	m_aiGICodeForDIKCode[DIK_C]   = 'C';
	m_aiGICodeForDIKCode[DIK_D]   = 'D';
	m_aiGICodeForDIKCode[DIK_E]   = 'E';
	m_aiGICodeForDIKCode[DIK_F]   = 'F';
	m_aiGICodeForDIKCode[DIK_G]   = 'G';
	m_aiGICodeForDIKCode[DIK_H]   = 'H';
	m_aiGICodeForDIKCode[DIK_I]   = 'I';
	m_aiGICodeForDIKCode[DIK_J]   = 'J';
	m_aiGICodeForDIKCode[DIK_K]   = 'K';
	m_aiGICodeForDIKCode[DIK_L]   = 'L';
	m_aiGICodeForDIKCode[DIK_M]   = 'M';
	m_aiGICodeForDIKCode[DIK_N]   = 'N';
	m_aiGICodeForDIKCode[DIK_O]   = 'O';
	m_aiGICodeForDIKCode[DIK_P]   = 'P';
	m_aiGICodeForDIKCode[DIK_Q]   = 'Q';
	m_aiGICodeForDIKCode[DIK_R]   = 'R';
	m_aiGICodeForDIKCode[DIK_S]   = 'S';
	m_aiGICodeForDIKCode[DIK_T]   = 'T';
	m_aiGICodeForDIKCode[DIK_U]   = 'U';
	m_aiGICodeForDIKCode[DIK_V]   = 'V';
	m_aiGICodeForDIKCode[DIK_W]   = 'W';
	m_aiGICodeForDIKCode[DIK_Y]   = 'Y';
	m_aiGICodeForDIKCode[DIK_Z]   = 'Z';
	m_aiGICodeForDIKCode[DIK_0]   = '0';
	m_aiGICodeForDIKCode[DIK_1]   = '1';
	m_aiGICodeForDIKCode[DIK_2]   = '2';
	m_aiGICodeForDIKCode[DIK_3]   = '3';
	m_aiGICodeForDIKCode[DIK_4]   = '4';
	m_aiGICodeForDIKCode[DIK_5]   = '5';
	m_aiGICodeForDIKCode[DIK_6]   = '6';
	m_aiGICodeForDIKCode[DIK_7]   = '7';
	m_aiGICodeForDIKCode[DIK_8]   = '8';
	m_aiGICodeForDIKCode[DIK_9]   = '9';

	m_aiGICodeForDIKCode[DIK_NUMPAD0]     = GIC_NUMPAD0;
	m_aiGICodeForDIKCode[DIK_NUMPAD1]     = GIC_NUMPAD1;
	m_aiGICodeForDIKCode[DIK_NUMPAD2]     = GIC_NUMPAD2;
	m_aiGICodeForDIKCode[DIK_NUMPAD3]     = GIC_NUMPAD3;
	m_aiGICodeForDIKCode[DIK_NUMPAD4]     = GIC_NUMPAD4;
	m_aiGICodeForDIKCode[DIK_NUMPAD5]     = GIC_NUMPAD5;
	m_aiGICodeForDIKCode[DIK_NUMPAD6]     = GIC_NUMPAD6;
	m_aiGICodeForDIKCode[DIK_NUMPAD7]     = GIC_NUMPAD7;
	m_aiGICodeForDIKCode[DIK_NUMPAD8]     = GIC_NUMPAD8;
	m_aiGICodeForDIKCode[DIK_NUMPAD9]     = GIC_NUMPAD9;
	m_aiGICodeForDIKCode[DIK_NUMPADENTER] = GIC_NUMPAD_ENTER;
	
	m_aiGICodeForDIKCode[DIK_MULTIPLY]    = GIC_MULTIPLY;
	m_aiGICodeForDIKCode[DIK_DIVIDE]      = GIC_DIVIDE;
	m_aiGICodeForDIKCode[DIK_HOME]        = GIC_HOME;
	m_aiGICodeForDIKCode[DIK_END]         = GIC_END;
	m_aiGICodeForDIKCode[DIK_PGUP]        = GIC_PAGE_UP;
	m_aiGICodeForDIKCode[DIK_PGDN]        = GIC_PAGE_DOWN;


	// Mappings from GI codes to DIK codes

	memset( gs_aGICodeToDIKCode, 0, sizeof(gs_aGICodeToDIKCode) );

	gs_aGICodeToDIKCode[GIC_F1]        = DIK_F1;
	gs_aGICodeToDIKCode[GIC_F2]        = DIK_F2;
	gs_aGICodeToDIKCode[GIC_F3]        = DIK_F3;
	gs_aGICodeToDIKCode[GIC_F4]        = DIK_F4;
	gs_aGICodeToDIKCode[GIC_F5]        = DIK_F5;
	gs_aGICodeToDIKCode[GIC_F6]        = DIK_F6;
	gs_aGICodeToDIKCode[GIC_F7]        = DIK_F7;
	gs_aGICodeToDIKCode[GIC_F8]        = DIK_F8;
	gs_aGICodeToDIKCode[GIC_F9]        = DIK_F9;
	gs_aGICodeToDIKCode[GIC_F10]       = DIK_F10;
	gs_aGICodeToDIKCode[GIC_F11]       = DIK_F11;
	gs_aGICodeToDIKCode[GIC_F12]       = DIK_F12;

	gs_aGICodeToDIKCode[GIC_ENTER]     = DIK_RETURN;
	gs_aGICodeToDIKCode[GIC_SPACE]     = DIK_SPACE;
	gs_aGICodeToDIKCode[GIC_LSHIFT]    = DIK_LSHIFT;
	gs_aGICodeToDIKCode[GIC_RSHIFT]    = DIK_RSHIFT;
	gs_aGICodeToDIKCode[GIC_RCONTROL]  = DIK_RCONTROL,
	gs_aGICodeToDIKCode[GIC_LCONTROL]  = DIK_LCONTROL;
	gs_aGICodeToDIKCode[GIC_RALT]      = DIK_RALT,
	gs_aGICodeToDIKCode[GIC_LALT]      = DIK_LALT;
	gs_aGICodeToDIKCode[GIC_TAB]       = DIK_TAB;
	gs_aGICodeToDIKCode[GIC_BACK]      = DIK_BACK;
	gs_aGICodeToDIKCode[GIC_ESC]       = DIK_ESCAPE;
	gs_aGICodeToDIKCode[GIC_UP]        = DIK_UP;
	gs_aGICodeToDIKCode[GIC_DOWN]      = DIK_DOWN;
	gs_aGICodeToDIKCode[GIC_RIGHT]     = DIK_RIGHT;
	gs_aGICodeToDIKCode[GIC_LEFT]      = DIK_LEFT;
	gs_aGICodeToDIKCode[GIC_NOCONVERT] = DIK_NOCONVERT;

	gs_aGICodeToDIKCode['A']   = DIK_A;
	gs_aGICodeToDIKCode['B']   = DIK_B;
	gs_aGICodeToDIKCode['C']   = DIK_C;
	gs_aGICodeToDIKCode['D']   = DIK_D;
	gs_aGICodeToDIKCode['E']   = DIK_E;
	gs_aGICodeToDIKCode['F']   = DIK_F;
	gs_aGICodeToDIKCode['G']   = DIK_G;
	gs_aGICodeToDIKCode['H']   = DIK_H;
	gs_aGICodeToDIKCode['I']   = DIK_I;
	gs_aGICodeToDIKCode['J']   = DIK_J;
	gs_aGICodeToDIKCode['K']   = DIK_K;
	gs_aGICodeToDIKCode['L']   = DIK_L;
	gs_aGICodeToDIKCode['M']   = DIK_M;
	gs_aGICodeToDIKCode['N']   = DIK_N;
	gs_aGICodeToDIKCode['O']   = DIK_O;
	gs_aGICodeToDIKCode['P']   = DIK_P;
	gs_aGICodeToDIKCode['Q']   = DIK_Q;
	gs_aGICodeToDIKCode['R']   = DIK_R;
	gs_aGICodeToDIKCode['S']   = DIK_S;
	gs_aGICodeToDIKCode['T']   = DIK_T;
	gs_aGICodeToDIKCode['U']   = DIK_U;
	gs_aGICodeToDIKCode['V']   = DIK_V;
	gs_aGICodeToDIKCode['W']   = DIK_W;
	gs_aGICodeToDIKCode['Y']   = DIK_Y;
	gs_aGICodeToDIKCode['Z']   = DIK_Z;
	gs_aGICodeToDIKCode['0']   = DIK_0;
	gs_aGICodeToDIKCode['1']   = DIK_1;
	gs_aGICodeToDIKCode['2']   = DIK_2;
	gs_aGICodeToDIKCode['3']   = DIK_3;
	gs_aGICodeToDIKCode['4']   = DIK_4;
	gs_aGICodeToDIKCode['5']   = DIK_5;
	gs_aGICodeToDIKCode['6']   = DIK_6;
	gs_aGICodeToDIKCode['7']   = DIK_7;
	gs_aGICodeToDIKCode['8']   = DIK_8;
	gs_aGICodeToDIKCode['9']   = DIK_9;

	gs_aGICodeToDIKCode[GIC_NUMPAD0]      = DIK_NUMPAD0;
	gs_aGICodeToDIKCode[GIC_NUMPAD1]      = DIK_NUMPAD1;
	gs_aGICodeToDIKCode[GIC_NUMPAD2]      = DIK_NUMPAD2;
	gs_aGICodeToDIKCode[GIC_NUMPAD3]      = DIK_NUMPAD3;
	gs_aGICodeToDIKCode[GIC_NUMPAD4]      = DIK_NUMPAD4;
	gs_aGICodeToDIKCode[GIC_NUMPAD5]      = DIK_NUMPAD5;
	gs_aGICodeToDIKCode[GIC_NUMPAD6]      = DIK_NUMPAD6;
	gs_aGICodeToDIKCode[GIC_NUMPAD7]      = DIK_NUMPAD7;
	gs_aGICodeToDIKCode[GIC_NUMPAD8]      = DIK_NUMPAD8;
	gs_aGICodeToDIKCode[GIC_NUMPAD9]      = DIK_NUMPAD9;
	gs_aGICodeToDIKCode[GIC_NUMPAD_ENTER] = DIK_NUMPADENTER;
	
	gs_aGICodeToDIKCode[GIC_MULTIPLY]     = DIK_MULTIPLY;
	gs_aGICodeToDIKCode[GIC_DIVIDE]       = DIK_DIVIDE;
	gs_aGICodeToDIKCode[GIC_HOME]         = DIK_HOME;
	gs_aGICodeToDIKCode[GIC_END]          = DIK_END;
	gs_aGICodeToDIKCode[GIC_PAGE_UP]      = DIK_PGUP;
	gs_aGICodeToDIKCode[GIC_PAGE_DOWN]    = DIK_PGDN;

}


//============================================================================================
// Init()
//============================================================================================
HRESULT CDIKeyboard::InitDIKeyboard( HWND hWnd )
{

    HRESULT hr;
    BOOL    bExclusive;
    BOOL    bForeground;
    BOOL    bImmediate;
    BOOL    bDisableWindowsKey;
    DWORD   dwCoopFlags;

    // Cleanup any previous call first
    Release();

    // Detrimine where the buffer would like to be allocated 
    bExclusive         = true;
    bForeground        = true;
    bImmediate         = false;
    bDisableWindowsKey = true;

    if( bExclusive )
        dwCoopFlags = DISCL_EXCLUSIVE;
    else
        dwCoopFlags = DISCL_NONEXCLUSIVE;

    if( bForeground )
        dwCoopFlags |= DISCL_FOREGROUND;
    else
        dwCoopFlags |= DISCL_BACKGROUND;

    // Disabling the windows key is only allowed only if we are in foreground nonexclusive
    if( bDisableWindowsKey && !bExclusive && bForeground )
        dwCoopFlags |= DISCL_NOWINKEY;

    
    // Obtain an interface to the system keyboard device.
	hr = DIRECTINPUT.GetDirectInputObject()->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, NULL );
    if( FAILED(hr) )
        return hr;
    
    // Set the data format to "keyboard format" - a predefined data format 
    //
    // A data format specifies which controls on a device we
    // are interested in, and how they should be reported.
    //
    // This tells DirectInput that we will be passing an array
    // of 256 bytes to IDirectInputDevice::GetDeviceState.
    if( FAILED( hr = m_pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
        return hr;
    
    // Set the cooperativity level to let DirectInput know how
    // this device should interact with the system and with other
    // DirectInput applications.
	// For security reasons, background exclusive keyboard access is not allowed

	hr = m_pKeyboard->SetCooperativeLevel( hWnd, dwCoopFlags );

    if( FAILED(hr) )
        return hr;

    if( !bImmediate )
    {
        // IMPORTANT STEP TO USE BUFFERED DEVICE DATA!
        //
        // DirectInput uses unbuffered I/O (buffer size = 0) by default.
        // If you want to read buffered data, you need to set a nonzero
        // buffer size.
        //
        // Set the buffer size to DINPUT_BUFFERSIZE (defined above) elements.
        //
        // The buffer size is a DWORD property associated with the device.
        DIPROPDWORD dipdw;

        dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj        = 0;
        dipdw.diph.dwHow        = DIPH_DEVICE;
        dipdw.dwData            = DIKEYBOARD_BUFFER_SIZE; // Arbitary buffer size

        if( FAILED( hr = m_pKeyboard->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) )
            return hr;
    }

    // Acquire the newly created device
    hr = m_pKeyboard->Acquire();

    return S_OK;
}


/// Read the input device's state in immediate mode and display it.
void CDIKeyboard::RefreshKeyStates()
{
	HRESULT hr;

	if( NULL == m_pKeyboard ) 
		return;// S_OK;
    
    // Get the input's device state, and put the state in dims
	memset( m_DIKs, 0, sizeof(m_DIKs) );
    hr = m_pKeyboard->GetDeviceState( sizeof(m_DIKs), m_DIKs );

    if( FAILED(hr) ) 
    {
        // DirectInput may be telling us that the input stream has been
        // interrupted.  We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done.
        // We just re-acquire and try again.
        
        // If input is lost then acquire and keep trying 
        hr = m_pKeyboard->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = m_pKeyboard->Acquire();

        // Update the dialog text 
        if( hr == DIERR_OTHERAPPHASPRIO || hr == DIERR_NOTACQUIRED ) 
//            SetDlgItemText( hDlg, IDC_DATA, TEXT("Unacquired") );

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return;// S_OK; 
    }
    
/*	if( diks[iKeyIndex] & 0x80 )
		bKeyIsPressed = true;
	else
		bKeyIsPressed = false;*/

//	return S_OK;
}

bool CDIKeyboard::IsKeyPressed( int gi_code )
{
	if( InputHub().GetInputState( 'A' ) == CInputState::PRESSED )
	{
		int break_here = 1;
	}

//	if( !IsKeyboardInputCode( gi_code ) )
//		return false;

	int dik_code = gs_aGICodeToDIKCode[ gi_code ];
	if( m_DIKs[dik_code] & 0x80 )
	{
		return true;
	}
	else
	{
		return false;
	}
}


Result::Name CDIKeyboard::Init()
{
	HRESULT hr = InitDIKeyboard( GameWindowManager_Win32().GetWindowHandle() );

	if( SUCCEEDED(hr) )
		return Result::SUCCESS;
	else
		return Result::UNKNOWN_ERROR;
}


/**
 Read the input device's state from the buffer and send the input data to the input handlers through InputHub()
*/
HRESULT CDIKeyboard::ReadBufferedData()
{
    DIDEVICEOBJECTDATA didod[ DIKEYBOARD_BUFFER_SIZE ];  // Receives buffered data 
    DWORD              dwElements;
    DWORD              i;
    HRESULT            hr;

    if( NULL == m_pKeyboard ) 
        return S_OK;
    
    dwElements = DIKEYBOARD_BUFFER_SIZE;
    hr = m_pKeyboard->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), didod, &dwElements, 0 );

    if( hr != DI_OK ) 
    {
        // We got an error or we got DI_BUFFEROVERFLOW.
        //
        // Either way, it means that continuous contact with the
        // device has been lost, either due to an external
        // interruption, or because the buffer overflowed
        // and some events were lost.
        //
        // Consequently, if a button was pressed at the time
        // the buffer overflowed or the connection was broken,
        // the corresponding "up" message might have been lost.
        //
        // But since our simple sample doesn't actually have
        // any state associated with button up or down events,
        // there is no state to reset.  (In a real game, ignoring
        // the buffer overflow would result in the game thinking
        // a key was held down when in fact it isn't; it's just
        // that the "up" event got lost because the buffer
        // overflowed.)
        //
        // If we want to be cleverer, we could do a
        // GetDeviceState() and compare the current state
        // against the state we think the device is in,
        // and process all the states that are currently
        // different from our private state.
		if( hr == DI_BUFFEROVERFLOW )
//			UpdateDeviceState();

        hr = m_pKeyboard->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = m_pKeyboard->Acquire();

        // Update the dialog text 
        if( hr == DIERR_OTHERAPPHASPRIO || 
            hr == DIERR_NOTACQUIRED ) 
//            ErrorMessage( "Unacquired - Direct Input Keyboad" );

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return S_OK; 
    }

	if( dwElements == 0 )
		return S_OK;	// no input in buffer

    // Study each of the buffer elements and process them.
	SInputData input;
    for( i = 0; i < dwElements; i++ ) 
    {
		if( 255 < didod[i].dwOfs )
			continue;

		input.iGICode= m_aiGICodeForDIKCode[ didod[i].dwOfs ];

		if( input.iGICode == GIC_INVALID )
			continue;	// no GIC for this keyboard device constant

		if( didod[i].dwData & 0x80 )
		{
			input.iType = ITYPE_KEY_PRESSED;
			input.fParam1 = 1.0f;
		}
		else
		{
			input.iType = ITYPE_KEY_RELEASED;
			input.fParam1 = 0.0f;
		}

		// send a pressed/released event to input hub
		InputHub().UpdateInput(input);

		UpdateInputState( input );

/*
		// update the input state
		CInputState& key = InputState( input.iGICode );
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			key.m_State = ;

			// schedule the time for the first auto repeat event
			key.m_NextAutoRepeatTimeMS = GlobalTimer().GetTimeMS() + ???;
		}
		else
			;
*/
    }

    return S_OK;
}


Result::Name CDIKeyboard::SendBufferedInputToInputHandlers()
{
	 HRESULT hr = ReadBufferedData();
	 if( SUCCEEDED(hr) )
		 return Result::SUCCESS;
	 else
		 return Result::UNKNOWN_ERROR;
}


//-----------------------------------------------------------------------------
// Name: Acquire()
// Desc: Acquire DirectInput keyboard.
//-----------------------------------------------------------------------------
void CDIKeyboard::Acquire()
{
	if( m_pKeyboard )
		m_pKeyboard->Acquire();
}


//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Initialize the DirectInput variables.
//-----------------------------------------------------------------------------
void CDIKeyboard::Release()
{
    // Unacquire the device one last time just in case 
    // the app tried to exit while the device is still acquired.
    if( m_pKeyboard ) 
        m_pKeyboard->Unacquire();
    
    // Release any DirectInput objects.
    SAFE_RELEASE( m_pKeyboard );
}
