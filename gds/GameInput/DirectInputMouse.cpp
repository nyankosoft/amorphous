#include "../GameInput/DirectInputMouse.h"
#include "../GameInput/DirectInput.h"
#include "../GameInput/InputHub.h"

//#include "App/GameWindowManager_Win32.h"


#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }


CDirectInputMouse::CDirectInputMouse()
: m_pDIMouse(NULL)
{
	m_fPrevMove_X = 0;
	m_fPrevMove_Y = 0;

	m_iPosX = m_iPosY = 0;
}


CDirectInputMouse::~CDirectInputMouse()
{
	Release();
}


HRESULT CDirectInputMouse::Init( HWND hWnd )
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
		MessageBox( NULL, "SetCooperativeLevel() failed.", "Error", MB_OK );
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
	GetClientRect(hWnd, &rect);		// クライアント部分のサイズの取得
	cw = rect.right - rect.left;	// クライアント領域外の横幅を計算
	ch = rect.bottom - rect.top;	// クライアント領域外の縦幅を計算
	m_ScreenWidth = cw;
	m_ScreenHeight = ch;

	return S_OK;
}

/*
bool CDirectInputMouse::InvertMouse()
{
	if( 0 < m_fInvertMouse_Y )
		return true;
	else
		return false;
}

void CDirectInputMouse::SetInvertMouse( bool bEnableInvertMouse )
{
	if( bEnableInvertMouse )
		m_fInvertMouse_Y = 1.0f;	// turn on invert mouse
	else
		m_fInvertMouse_Y = -1.0f;	// turn off invert mouse
}*/


void CDirectInputMouse::AcquireMouse()
{
	if( m_pDIMouse )
	{
		HRESULT hr = m_pDIMouse->Acquire();

		if(FAILED(hr))
			int err = 1;
//			PrintLog( "CDirectInputMouse::AcquireMouse() - unable to acquire mouse" );
	}
}


void CDirectInputMouse::Release()
{
    // Unacquire the device one last time just in case 
    // the app tried to exit while the device is still acquired.
    if( m_pDIMouse ) 
        m_pDIMouse->Unacquire();
    
    // Release any DirectInput objects.
    SAFE_RELEASE( m_pDIMouse );
}

//read input data from buffer
HRESULT CDirectInputMouse::UpdateInput()
{

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
	SInputData input;

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

		input.SetParamH16( (short)m_iPosX );
		input.SetParamL16( (short)m_iPosY );

		switch( didod[i].dwOfs )
        {
            case DIMOFS_BUTTON0:
				input.iGICode = GIC_MOUSE_BUTTON_L;	
				INPUTHUB.UpdateInput(input);
				break;
            case DIMOFS_BUTTON1:
				input.iGICode = GIC_MOUSE_BUTTON_R;
				INPUTHUB.UpdateInput(input);
				break;
            case DIMOFS_BUTTON2:
				input.iGICode = GIC_MOUSE_BUTTON_M;	
				INPUTHUB.UpdateInput(input);
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
				INPUTHUB.UpdateInput(input);
				break;
        }			

    }

	m_iPosX += iMoveX;
	m_iPosY += iMoveY;

	// clamp the cursor position within the window
	if( m_iPosX < 0 ) m_iPosX = 0;
	if( m_iPosY < 0 ) m_iPosY = 0;
	int w = m_ScreenWidth;
	int h = m_ScreenHeight;
//	int w = GAMEWINDOWMANAGER.GetScreenWidth();
//	int h = GAMEWINDOWMANAGER.GetScreenHeight();
	if( w < m_iPosX ) m_iPosX = w;
	if( h < m_iPosY ) m_iPosY = h;

	input.SetParamH16( (short)m_iPosX );
	input.SetParamL16( (short)m_iPosY );

	input.iType = ITYPE_KEY_PRESSED;

	input.iGICode = GIC_MOUSE_AXIS_X;
	input.fParam1 = (float)iMoveX;	//	fNewMove_X
	INPUTHUB.UpdateInput(input);

	input.iGICode = GIC_MOUSE_AXIS_Y;
	input.fParam1 = (float)iMoveY;	//	fNewMove_Y
	INPUTHUB.UpdateInput(input);

///	g_PerformanceCheck.fMouseNewMoveX = fNewMove_X;
///	g_PerformanceCheck.fMouseNewMoveY = fNewMove_Y;

    return S_OK;
}