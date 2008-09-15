#include "DIKeyboard.h"

#include "../base.h"
#include "DirectInput.h"
#include "InputHub.h"


#define DIKEYBOARD_SAMPLE_BUFFER_SIZE 16  // arbitrary number of buffer elements


CDIKeyboard::CDIKeyboard()
: m_pKeyboard(NULL)
{
	InitKeyCodeMap();
}


CDIKeyboard::~CDIKeyboard()
{
	Release();
}


/**
 * initializes the table which maps DTK_ codes to GIC_ codes
 */
void CDIKeyboard::InitKeyCodeMap()
{
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

	m_aiGICodeForDIKCode[DIK_NUMPAD0] = GIC_NUMPAD0;
	m_aiGICodeForDIKCode[DIK_NUMPAD1] = GIC_NUMPAD1;
	m_aiGICodeForDIKCode[DIK_NUMPAD2] = GIC_NUMPAD2;
	m_aiGICodeForDIKCode[DIK_NUMPAD3] = GIC_NUMPAD3;
	m_aiGICodeForDIKCode[DIK_NUMPAD4] = GIC_NUMPAD4;
	m_aiGICodeForDIKCode[DIK_NUMPAD5] = GIC_NUMPAD5;
	m_aiGICodeForDIKCode[DIK_NUMPAD6] = GIC_NUMPAD6;
	m_aiGICodeForDIKCode[DIK_NUMPAD7] = GIC_NUMPAD7;
	m_aiGICodeForDIKCode[DIK_NUMPAD8] = GIC_NUMPAD8;
	m_aiGICodeForDIKCode[DIK_NUMPAD9] = GIC_NUMPAD9;
	m_aiGICodeForDIKCode[DIK_NUMPADENTER] = GIC_NUMPAD_ENTER;
	
	m_aiGICodeForDIKCode[DIK_MULTIPLY] = GIC_MULTIPLY;
	m_aiGICodeForDIKCode[DIK_DIVIDE]   = GIC_DIVIDE;
/*	m_aiGICodeForDIKCode[DIK_]    = '';
	m_aiGICodeForDIKCode[DIK_]    = '';
	m_aiGICodeForDIKCode[DIK_]    = '';*/

}


//============================================================================================
// Init()
//============================================================================================
HRESULT CDIKeyboard::Init( HWND hWnd )
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
        dipdw.dwData            = DIKEYBOARD_SAMPLE_BUFFER_SIZE; // Arbitary buffer size

        if( FAILED( hr = m_pKeyboard->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) )
            return hr;
    }

    // Acquire the newly created device
    hr = m_pKeyboard->Acquire();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: GetKeyState()
// Desc: Read the input device's state in immediate mode and display it.
//-----------------------------------------------------------------------------
//HRESULT CDIKeyboard::ReadImmediateData(bool& bKeyIsPressed, int iKeyIndex)
HRESULT CDIKeyboard::GetKeyState( BYTE *pacKeyboardStateBuffer )
{
    HRESULT hr;
//    BYTE    diks[256];   // DirectInput keyboard state buffer 

    if( NULL == m_pKeyboard ) 
        return S_OK;
    
    // Get the input's device state, and put the state in dims
//    ZeroMemory( diks, sizeof(diks) );
//    hr = m_pKeyboard->GetDeviceState( sizeof(diks), diks );
    hr = m_pKeyboard->GetDeviceState( sizeof(pacKeyboardStateBuffer), pacKeyboardStateBuffer );

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
        return S_OK; 
    }
    
/*	if( diks[iKeyIndex] & 0x80 )
		bKeyIsPressed = true;
	else
		bKeyIsPressed = false;*/
		
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: ReadBufferedData()
// Desc: Read the input device's state when in buffered mode and display it.
//-----------------------------------------------------------------------------
HRESULT CDIKeyboard::ReadBufferedData()
{
    DIDEVICEOBJECTDATA didod[ DIKEYBOARD_SAMPLE_BUFFER_SIZE ];  // Receives buffered data 
    DWORD              dwElements;
    DWORD              i;
    HRESULT            hr;

    if( NULL == m_pKeyboard ) 
        return S_OK;
    
    dwElements = DIKEYBOARD_SAMPLE_BUFFER_SIZE;
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

		// send input to input hub
		INPUTHUB.UpdateInput(input);
    }

    return S_OK;
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



/*

//-----------------------------------------------------------------------------
// Name: MainDlgProc()
// Desc: Handles dialog messages
//-----------------------------------------------------------------------------
INT_PTR CALLBACK MainDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )

        case WM_ACTIVATE:
            if( WA_INACTIVE != wParam && m_pKeyboard )
            {
                // Make sure the device is acquired, if we are gaining focus.
                m_pKeyboard->Acquire();
            }
            break;
        
        case WM_TIMER:
            // Update the input device every timer message
            {
                BOOL bImmediate = ( IsDlgButtonChecked( hDlg, IDC_IMMEDIATE  ) == BST_CHECKED );

                if( bImmediate )
                {
                    if( FAILED( ReadImmediateData( hDlg ) ) )
                    {
                        KillTimer( hDlg, 0 );    
                        MessageBox( NULL, _T("Error reading input state. ")
                                          _T("The sample will now exit."), 
                                          _T("Keyboard"), MB_ICONERROR | MB_OK );
                        EndDialog( hDlg, TRUE ); 
                    }
                }

*/