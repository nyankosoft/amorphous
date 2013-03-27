#include "DirectInputMouse.hpp"

#include "Support/Profile.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Input/DirectInput.hpp"
#include "Input/InputHub.hpp"

#include "App/GameWindowManager_Win32.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"


namespace amorphous
{


/*
class InputDeviceFactory
{
public:

	CMouseInputDevice *CreateMouse( int type )
	{
		return new DirectInputMouse();
	}

	CKeyboardInputDevice *CreateKeyboard( int type )
	{
		return new DIKeyboard();
	}
};
*/

DirectInputMouse::DirectInputMouse()
:
m_pDIMouse(NULL)
{
	m_fPrevMove_X = 0;
	m_fPrevMove_Y = 0;

	m_iPosX = m_iPosY = 0;

	GetInputDeviceHub().RegisterInputDeviceToGroup( this );
}


DirectInputMouse::~DirectInputMouse()
{
	Release();
}


HRESULT DirectInputMouse::InitDIMouse( HWND hWnd )
{
	HRESULT hr;

	bool bExclusive = true;
	bool bForeground = true;
	DWORD dwCoopFlags = 0;

	if( bExclusive )
        dwCoopFlags = DISCL_EXCLUSIVE;
    else
        dwCoopFlags = DISCL_NONEXCLUSIVE;

    if( bForeground )
        dwCoopFlags |= DISCL_FOREGROUND;
    else
        dwCoopFlags |= DISCL_BACKGROUND;

    
    // Obtain an interface to the system mouse device.
//    if( FAILED( hr = m_pDI->CreateDevice( GUID_SysMouse, &m_pDIMouse, NULL ) ) )
    if( FAILED( hr = DIRECTINPUT.GetDirectInputObject()->CreateDevice( GUID_SysMouse, &m_pDIMouse, NULL ) ) )
        return hr;
    
    // Set the data format to "mouse format" - a predefined data format 
    //
    // A data format specifies which controls on a device we
    // are interested in, and how they should be reported.
    //
    // This tells DirectInput that we will be passing a
    // DIMOUSESTATE2 structure to IDirectInputDevice::GetDeviceState.
    if( FAILED( hr = m_pDIMouse->SetDataFormat( &c_dfDIMouse2 ) ) )
        return hr;
    
    // Set the cooperativity level to let DirectInput know how
    // this device should interact with the system and with other
    // DirectInput applications.
    hr = m_pDIMouse->SetCooperativeLevel( hWnd, dwCoopFlags );
    if( hr == DIERR_UNSUPPORTED && !bForeground && bExclusive )
    {
//      SetCooperativeLevel() returned DIERR_UNSUPPORTED. For security reasons, background exclusive mouse
//		access is not allowed.
		Release();
		LOG_PRINT_ERROR( " SetCooperativeLevel() failed." );
        return S_OK;
    }

    if( FAILED(hr) )
        return hr;

    // IMPORTANT STEP TO USE BUFFERED DEVICE DATA!
    //
    // DirectInput uses unbuffered I/O (buffer size = 0) by default.
    // If you want to read buffered data, you need to set a nonzero
    // buffer size.
    //
    // Set the buffer size to SAMPLE_BUFFER_SIZE (defined above) elements.
    //
    // The buffer size is a DWORD property associated with the device.
    DIPROPDWORD dipdw;
    dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj        = 0;
    dipdw.diph.dwHow        = DIPH_DEVICE;
    dipdw.dwData            = DIMOUSE_BUFFER_SIZE; // Arbitary buffer size
    if( FAILED( hr = m_pDIMouse->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) )
        return hr;

    // Acquire the newly created device
    //m_pDIMouse->Acquire();

	// get client rect size
	RECT rect;
	LONG cw, ch;
	GetClientRect(hWnd, &rect);		// get client area size
	cw = rect.right - rect.left;	// width of the non-client area?
	ch = rect.bottom - rect.top;	// height of the non-client area?
	m_ScreenWidth = cw;
	m_ScreenHeight = ch;

	return Result::SUCCESS;
}


Result::Name DirectInputMouse::Init()
{
	HRESULT hr = InitDIMouse( GetGameWindowManager_Win32().GetWindowHandle() );

	if( SUCCEEDED(hr) )
		return Result::SUCCESS;
	else
		return Result::UNKNOWN_ERROR;
}


/*
bool DirectInputMouse::InvertMouse()
{
	if( 0 < m_fInvertMouse_Y )
		return true;
	else
		return false;
}

void DirectInputMouse::SetInvertMouse( bool bEnableInvertMouse )
{
	if( bEnableInvertMouse )
		m_fInvertMouse_Y = 1.0f;	// turn on invert mouse
	else
		m_fInvertMouse_Y = -1.0f;	// turn off invert mouse
}*/


void DirectInputMouse::AcquireMouse()
{
	if( m_pDIMouse )
	{
		HRESULT hr = m_pDIMouse->Acquire();

		if(FAILED(hr))
			int err = 1;
//			PrintLog( "DirectInputMouse::AcquireMouse() - unable to acquire mouse" );
	}
}


void DirectInputMouse::Release()
{
    // Unacquire the device one last time just in case 
    // the app tried to exit while the device is still acquired.
    if( m_pDIMouse ) 
        m_pDIMouse->Unacquire();
    
    // Release any DirectInput objects.
    SAFE_RELEASE( m_pDIMouse );
}


/// Read input data from buffer and send them to the input handlers
HRESULT DirectInputMouse::UpdateInput()
{
	PROFILE_FUNCTION();

    DIDEVICEOBJECTDATA didod[ DIMOUSE_BUFFER_SIZE ];  // Receives buffered data 
    DWORD              dwElements;
    DWORD              i;
    HRESULT            hr;

    if( NULL == m_pDIMouse ) 
        return S_OK;
    
    dwElements = DIMOUSE_BUFFER_SIZE;
    hr = m_pDIMouse->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), didod, &dwElements, 0 );
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
        hr = m_pDIMouse->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = m_pDIMouse->Acquire();

        // Update the dialog text 
//        if( hr == DIERR_OTHERAPPHASPRIO || hr == DIERR_NOTACQUIRED ) 
//            ErrorMessageBox("mouse unacquired");

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return S_OK; 
    }

    if( FAILED(hr) )  
        return hr;

	int iMoveX = 0, iMoveY = 0;
	float fNewMove_X = 0, fNewMove_Y = 0; 
	InputData input;
//	const float factor = (float)REFERENCE_SCREEN_WIDTH / (float)m_ScreenWidth;
	const float scale = (float)m_ScreenWidth / (float)GraphicsComponent::REFERENCE_SCREEN_WIDTH;

	// m_iPosX & m_iPosY holds scaled positions (positions in reference screen coordinates)
//	int non_scaled_pos_x = (int)(m_iPosX / factor);
//	int non_scaled_pos_y = (int)(m_iPosY / factor);
	int pos_x = m_iPosX;
	int pos_y = m_iPosY;

    // Study each of the buffer elements and process them.
    for( i = 0; i < dwElements; i++ )
    {
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

		input.SetParamH16( (short)(pos_x / scale) );
		input.SetParamL16( (short)(pos_y / scale) );

		switch( didod[i].dwOfs )
        {
            case DIMOFS_BUTTON0:
				input.iGICode = GIC_MOUSE_BUTTON_L;	
				UpdateInputState( input );
				GetInputHub().UpdateInput(input);
				break;
            case DIMOFS_BUTTON1:
				input.iGICode = GIC_MOUSE_BUTTON_R;
				UpdateInputState( input );
				GetInputHub().UpdateInput(input);
				break;
            case DIMOFS_BUTTON2:
				input.iGICode = GIC_MOUSE_BUTTON_M;	
				UpdateInputState( input );
				GetInputHub().UpdateInput(input);
				break;

            case DIMOFS_X:
				iMoveX += (int)didod[i].dwData;
				break;
            case DIMOFS_Y:
				iMoveY += (int)didod[i].dwData;
				break;
            case DIMOFS_Z:
				if( 0 < (int)didod[i].dwData )
					input.iGICode = GIC_MOUSE_WHEEL_UP;
				else
					input.iGICode = GIC_MOUSE_WHEEL_DOWN;
				UpdateInputState( input );
				GetInputHub().UpdateInput(input);
				break;
        }			

    }

	pos_x += iMoveX;
	pos_y += iMoveY;

	// clamp the cursor position within the window
	if( pos_x < 0 ) pos_x = 0;
	if( pos_y < 0 ) pos_y = 0;
	int w = m_ScreenWidth;
	int h = m_ScreenHeight;
//	int w = GameWindowManager().GetScreenWidth();
//	int h = GameWindowManager().GetScreenHeight();
	if( w < pos_x ) pos_x = w;
	if( h < pos_y ) pos_y = h;

//	m_iPosX = (int)( pos_x * factor );
//	m_iPosY = (int)( pos_y * factor );
	m_iPosX = pos_x;
	m_iPosY = pos_y;

//	input.SetParamH16( (short)m_iPosX );
//	input.SetParamL16( (short)m_iPosY );
	input.SetParamH16( (short)(pos_x / scale) );
	input.SetParamL16( (short)(pos_y / scale) );

	// Send input data for mouse movements in the X and Y directions

	input.iType = ITYPE_VALUE_CHANGED;

//	m_fPrevMoveFractionX = (float)iMoveX * scale;
//	m_fPrevMoveFractionY = (float)iMoveY * scale;

	input.iGICode = GIC_MOUSE_AXIS_X;
//	input.fParam1 = (float)iMoveX * factor;	//	fNewMove_X
	input.fParam1 = (float)iMoveX / scale;
	GetInputHub().UpdateInput(input);

	input.iGICode = GIC_MOUSE_AXIS_Y;
//	input.fParam1 = (float)iMoveY * factor;	//	fNewMove_Y
	input.fParam1 = (float)iMoveY / scale;
	GetInputHub().UpdateInput(input);

	///	StateLog().Register( "non-scaled mouse move x", iMoveX );
	///	StateLog().Register( "non-scaled mouse move y", iMoveY );
	///	StateLog().Register( "scaled mouse pos x", m_iPosX );
	///	StateLog().Register( "scaled mouse pos y", m_iPosY );

    return S_OK;
}


Result::Name DirectInputMouse::SendBufferedInputToInputHandlers()
{
	HRESULT hr = UpdateInput();

	if( SUCCEEDED(hr) )
		return Result::SUCCESS;
	else
		return Result::UNKNOWN_ERROR;
}


} // namespace amorphous
