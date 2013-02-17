#include "DirectInputGamepad.hpp"
#include "DirectInput.hpp"
#include "InputHub.hpp"
#include "ForceFeedback/DIForceFeedbackEffectImpl.hpp"
#include "App/GameWindowManager_Win32.hpp"

#include "Support/StringAux.hpp"
//#include "Support/Log/StateLog.hpp"
#include "Support/Log/DefaultLog.hpp"

#include "../base.hpp"

#include <math.h>


namespace amorphous
{

using namespace std;
using namespace boost;



//-----------------------------------------------------------------------------
// Function-prototypes
//-----------------------------------------------------------------------------
BOOL CALLBACK    EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
BOOL CALLBACK    EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
BOOL CALLBACK    EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );

// used to temporarily hold pointer to joystick device
static LPDIRECTINPUTDEVICE8 g_pDITempJoystickDevice = NULL;
//static vector<DIDEVICEOBJECTINSTANCE> g_vecDITempJoystickDeviceInstance;
static boost::mutex gs_GamepadDeviceCreationMutex;
static DirectInputGamepad *sg_pDIGamepad = NULL;


DirectInputGamepad::DirectInputGamepad()
:
m_pDIJoystick(NULL)
{
	LOG_PRINT( " - Creating an instance..." );

	int i;
	for( i=0; i<NUM_ANALOG_CONTROLS; i++ )
        m_aAxisPosition[i] = 0;

	for( i=0; i<NUM_ANALOG_CONTROLS; i++ )
        m_bPrevHold[i] = false;

	for( i=0; i<NUM_POV_INPUTS; i++ )
	{
		m_aPrevPOV[i] = -1;
		m_aPOV[i] = -1;
	}

	m_bSendExtraDigitalInputFromAnalogInput = false;

	m_bSendExtraDigitalInputFromPOVInput = true;

	for( i=0; i<NUM_ANALOG_CONTROLS; i++ )
        m_afAnalogInputScale[i] = 1.0f;

	GetInputDeviceHub().RegisterInputDeviceToGroup( this );
}


DirectInputGamepad::~DirectInputGamepad()
{
	Release();
}


void DirectInputGamepad::Release()
{
    // Unacquire the device one last time just in case
    // the app tried to exit while the device is still acquired.
    if( m_pDIJoystick ) 
        m_pDIJoystick->Unacquire();
    
    // Release the DirectInput object of the joystick.
    SAFE_RELEASE( m_pDIJoystick );
}


HRESULT DirectInputGamepad::InitDIGamepad( HWND hWnd )
{
	if( !m_pDIJoystick )
		return E_FAIL;
	
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

	HRESULT hr;

    // Set the data format to "simple joystick" - a predefined data format 
    //
    // A data format specifies which controls on a device we are interested in,
    // and how they should be reported. This tells DInput that we will be
    // passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
    if( FAILED( hr = m_pDIJoystick->SetDataFormat( &c_dfDIJoystick2 ) ) )
        return hr;

	LOG_PRINT( " A Gamepad data format has been set" );

    // Set the cooperativity level to let DirectInput know how
    // this device should interact with the system and with other
    // DirectInput applications.
    hr = m_pDIJoystick->SetCooperativeLevel( hWnd, dwCoopFlags );

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

	g_pDITempJoystickDevice = m_pDIJoystick;

    // Enumerate the joystick objects. The callback function enabled user
    // interface elements for objects that are found, and sets the min/max
    // values property for discovered axes.
	hr = m_pDIJoystick->EnumObjects( EnumObjectsCallback, (VOID*)hWnd, DIDFT_ALL );
	g_pDITempJoystickDevice = NULL;
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
    dipdw.dwData            = DIJOYSTICK_BUFFER_SIZE; // Arbitary buffer size
    if( FAILED( hr = m_pDIJoystick->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) )
        return hr;

	// set to the absolute axis mode
    dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj        = 0;
    dipdw.diph.dwHow        = DIPH_DEVICE;
    dipdw.dwData            = DIPROPAXISMODE_ABS; // Arbitary buffer size
    if( FAILED( hr = m_pDIJoystick->SetProperty( DIPROP_AXISMODE, &dipdw.diph ) ) )
        return hr;

	// force feedback related init (draft)

	m_pFFParams.reset( new CDIFFParams );

	DWORD dwNumForceFeedbackAxis = 0;

	// Enumerate and count the axes of the joystick 
	if( FAILED( hr = m_pDIJoystick->EnumObjects( EnumAxesCallback,
												 ( VOID* )&dwNumForceFeedbackAxis, DIDFT_AXIS ) ) )
	{
		//
	}

	// This simple sample only supports one or two axis joysticks
	if( 2 < dwNumForceFeedbackAxis )
		dwNumForceFeedbackAxis = 2;

	m_pFFParams->m_NumFFAxes = dwNumForceFeedbackAxis;
	m_pFFParams->m_pDeviceCopy = m_pDIJoystick;

    // Acquire the newly created device
    //m_pDIJoystick->Acquire();

	return S_OK;
}


Result::Name DirectInputGamepad::Init()
{
	boost::mutex::scoped_lock( gs_GamepadDeviceCreationMutex );

	sg_pDIGamepad = this;

	LOG_PRINT( " - initializing gamepad..." );

	HRESULT hr;

//	g_pDITempJoystickDevice = NULL;

	LOG_PRINT( " Enumerating input device objects..." );

    // Look for a simple joystick we can use for this sample program.
	hr = DIRECTINPUT.GetDirectInputObject()->EnumDevices( DI8DEVCLASS_GAMECTRL, 
		                                                  EnumJoysticksCallback,
														  NULL, DIEDFL_ATTACHEDONLY );

	if( FAILED(hr) )
		return Result::UNKNOWN_ERROR;

	if( !m_pDIJoystick )
	{
		LOG_PRINT_ERROR( "Failed to create game controller device" );
		return Result::UNKNOWN_ERROR;
	}

	sg_pDIGamepad = NULL;

	hr = InitDIGamepad( GameWindowManager_Win32().GetWindowHandle() );

	if( SUCCEEDED(hr) )
		return Result::SUCCESS;
	else
		return Result::UNKNOWN_ERROR;
}


Result::Name DirectInputGamepad::InitDevice( const DIDEVICEINSTANCE& di )
{
	m_InstanceName = di.tszInstanceName;
	m_ProductName  = di.tszProductName;

	Result::Name res = CreateDevice( di );

	if( res != Result::SUCCESS )
		return res;

	HRESULT hr = InitDIGamepad( GameWindowManager_Win32().GetWindowHandle() );

	if( SUCCEEDED(hr) )
		return Result::SUCCESS;
	else
		return Result::UNKNOWN_ERROR;
}



Result::Name DirectInputGamepad::CreateDevice( const DIDEVICEINSTANCE& di )
{
	HRESULT hr;
	hr = DIRECTINPUT.GetDirectInputObject()->CreateDevice( di.guidInstance, &m_pDIJoystick, NULL );

	if( FAILED(hr) )
	{
		LOG_PRINT_ERROR( " IDirectInput8::CreateDevice() failed." );
		return Result::UNKNOWN_ERROR;
	}
    
    // Make sure we got a joystick
    if( m_pDIJoystick )
	{
		return Result::SUCCESS;
	}
	else
    {
		LOG_PRINT_ERROR( " IDirectInput8::CreateDevice() didn't fail, but the device was not created." );
//		MessageBox( NULL, "Joystick not found.", "DirectInputGamepad::Init()", MB_ICONERROR | MB_OK );
		return Result::UNKNOWN_ERROR;
    }
}

BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                VOID* pContext )
{
    DWORD* pdwNumForceFeedbackAxis = (DWORD*)pContext;

    if( ( pdidoi->dwFlags & DIDOI_FFACTUATOR ) != 0 )
        ( *pdwNumForceFeedbackAxis )++;

    return DIENUM_CONTINUE;
}



//-----------------------------------------------------------------------------
// Name: EnumJoysticksCallback()
// Desc: Called once for each enumerated joystick. If we find one, create a
//       device interface on it so we can play with it.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext )
{
	// Obtain an interface to the enumerated joystick.
//	hr = DIRECTINPUT.GetDirectInputObject()->CreateDevice( pdidInstance->guidInstance, &g_pDITempJoystickDevice, NULL );

	if( !sg_pDIGamepad )
		return DIENUM_STOP;

	Result::Name res = sg_pDIGamepad->CreateDevice( *pdidInstance );

	if( res == Result::SUCCESS ) 
	{
		// Stop enumeration. Note: we're just taking the first joystick we get. You
		// could store all the enumerated joysticks and let the user pick.
		return DIENUM_STOP;
	}
	else
	{
		// If it failed, then we can't use this joystick. (Maybe the user unplugged
		// it while we were in the middle of enumerating it.)
        return DIENUM_CONTINUE;
	}
}



//-----------------------------------------------------------------------------
// Name: EnumObjectsCallback()
// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a 
//       joystick. This function enables user interface elements for objects
//       that are found to exist, and scales axes min/max values.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                   VOID* pContext )
{
    HWND hDlg = (HWND)pContext;

    static int nSliderCount = 0;  // Number of returned slider controls
    static int nPOVCount = 0;     // Number of returned POV controls

    // For axes that are returned, set the DIPROP_RANGE property for the
    // enumerated axis in order to scale min/max values.
    if( pdidoi->dwType & DIDFT_AXIS )
    {
        DIPROPRANGE diprg; 
        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diprg.diph.dwHow        = DIPH_BYID; 
        diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
        diprg.lMin              = -1000; 
        diprg.lMax              = +1000; 
    
        // Set the range for the axis
        if( FAILED( g_pDITempJoystickDevice->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
            return DIENUM_STOP;
         
    }

/*
    // Set the UI to reflect what objects the joystick supports
    if (pdidoi->guidType == GUID_XAxis)
    {
        EnableWindow( GetDlgItem( hDlg, IDC_X_AXIS ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDC_X_AXIS_TEXT ), TRUE );
    }
    if (pdidoi->guidType == GUID_YAxis)
    {
        EnableWindow( GetDlgItem( hDlg, IDC_Y_AXIS ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDC_Y_AXIS_TEXT ), TRUE );
    }
    if (pdidoi->guidType == GUID_ZAxis)
    {
        EnableWindow( GetDlgItem( hDlg, IDC_Z_AXIS ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDC_Z_AXIS_TEXT ), TRUE );
    }
    if (pdidoi->guidType == GUID_RxAxis)
    {
        EnableWindow( GetDlgItem( hDlg, IDC_X_ROT ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDC_X_ROT_TEXT ), TRUE );
    }
    if (pdidoi->guidType == GUID_RyAxis)
    {
        EnableWindow( GetDlgItem( hDlg, IDC_Y_ROT ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDC_Y_ROT_TEXT ), TRUE );
    }
    if (pdidoi->guidType == GUID_RzAxis)
    {
        EnableWindow( GetDlgItem( hDlg, IDC_Z_ROT ), TRUE );
        EnableWindow( GetDlgItem( hDlg, IDC_Z_ROT_TEXT ), TRUE );
    }
    if (pdidoi->guidType == GUID_Slider)
    {
        switch( nSliderCount++ )
        {
            case 0 :
                EnableWindow( GetDlgItem( hDlg, IDC_SLIDER0 ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDC_SLIDER0_TEXT ), TRUE );
                break;

            case 1 :
                EnableWindow( GetDlgItem( hDlg, IDC_SLIDER1 ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDC_SLIDER1_TEXT ), TRUE );
                break;
        }
    }
    if (pdidoi->guidType == GUID_POV)
    {
        switch( nPOVCount++ )
        {
            case 0 :
                EnableWindow( GetDlgItem( hDlg, IDC_POV0 ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDC_POV0_TEXT ), TRUE );
                break;

            case 1 :
                EnableWindow( GetDlgItem( hDlg, IDC_POV1 ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDC_POV1_TEXT ), TRUE );
                break;

            case 2 :
                EnableWindow( GetDlgItem( hDlg, IDC_POV2 ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDC_POV2_TEXT ), TRUE );
                break;

            case 3 :
                EnableWindow( GetDlgItem( hDlg, IDC_POV3 ), TRUE );
                EnableWindow( GetDlgItem( hDlg, IDC_POV3_TEXT ), TRUE );
                break;
        }
    }
*/
    return DIENUM_CONTINUE;
}


HRESULT DirectInputGamepad::Acquire()
{
	if( m_pDIJoystick )
		return m_pDIJoystick->Acquire();
	else
		return E_FAIL;
}


/// Read input data from buffer
HRESULT DirectInputGamepad::ReadBufferedData()
{
	m_pDIJoystick->Poll();

    DIDEVICEOBJECTDATA didod[ DIJOYSTICK_BUFFER_SIZE ];  // Receives buffered data 
    DWORD              dwElements;
    DWORD              i;
    HRESULT            hr;
	int current_analog_values[NUM_ANALOG_CONTROLS];

	for( int i=0; i<NUM_ANALOG_CONTROLS; i++ )
		current_analog_values[i] = VALUE_NOT_FOUND;

    if( NULL == m_pDIJoystick ) 
        return S_OK;
    
    dwElements = DIJOYSTICK_BUFFER_SIZE;
    hr = m_pDIJoystick->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), didod, &dwElements, 0 );
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
        hr = m_pDIJoystick->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = m_pDIJoystick->Acquire();

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

	InputData input;

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

		if( DIJOFS_BUTTON0 <= didod[i].dwOfs && didod[i].dwOfs <= DIJOFS_BUTTON11 )
		{
			input.iGICode = GIC_GPD_BUTTON_00 + ( didod[i].dwOfs - DIJOFS_BUTTON0 );
			GetInputHub().UpdateInput(input);

			UpdateInputState( input );
/*
			// access the input state holder in InputHub()()
			CInputState& button = InputState( input.iGICode );

			if( input.iType == ITYPE_KEY_PRESSED )
			{
				button.m_State = CInputState::PRESSED;

				// schedule the first auto repeat event
				button.m_NextAutoRepeatTimeMS = GlobalTimer().GetTimeMS() + GPD_FIRST_AUTO_REPEAT_INTERVAL_MS;
			}
			else
			{
				button.m_State = CInputState::RELEASED;
			}
*/
//			StateLog.Update(14, "gpd.button: " + to_string(input.iGICode - GIC_GPD_BUTTON_00) );
		}
		else
		{
			switch( didod[i].dwOfs )
			{
				// original input data is signed value, but is represented
				// as unsigned integer (DWORD)
				// therefore, didod[i].dwData must be casted to signed integer
				// before being casted to other types, such as float

				case DIJOFS_X:
					current_analog_values[AXIS_X] = (int)didod[i].dwData;
					break;
				case DIJOFS_Y:
					current_analog_values[AXIS_Y] = (int)didod[i].dwData;
					break;
				case DIJOFS_Z:
					current_analog_values[AXIS_Z] = (int)didod[i].dwData;
					break;
				case DIJOFS_RX:
					current_analog_values[ROTATION_X] = (int)didod[i].dwData;
					break;
				case DIJOFS_RY:
					current_analog_values[ROTATION_Y] = (int)didod[i].dwData;
					break;
				case DIJOFS_RZ:
					current_analog_values[ROTATION_Z] = (int)didod[i].dwData;
					break;
				case DIJOFS_POV(0):
					m_aPOV[POV_0] = ((int)didod[i].dwData);
					break;
/*				case DIJOFS_POV(1):
					m_aPOV[POV_1] = ((int)didod[i].dwData);
					break;
				case DIJOFS_POV(2):
					m_aPOV[POV_2] = ((int)didod[i].dwData);
					break;
				case DIJOFS_POV(3):
					m_aPOV[POV_3] = ((int)didod[i].dwData);
					break;*/
				default:
					break;
		    }
		}
    }

	input.iType = ITYPE_VALUE_CHANGED;

	static const int gi_codes[NUM_ANALOG_CONTROLS] = {
		GIC_GPD_AXIS_X,
		GIC_GPD_AXIS_Y,
		GIC_GPD_AXIS_Z,
		GIC_GPD_ROTATION_X,
		GIC_GPD_ROTATION_Y,
		GIC_GPD_ROTATION_Z,
	};

	for( int i=0; i<NUM_ANALOG_CONTROLS; i++ )
	{
		if( current_analog_values[i] == VALUE_NOT_FOUND
		 || current_analog_values[i] == m_aAxisPosition[i] )
		{
			continue;
		}

		m_aAxisPosition[i] = current_analog_values[i]; // update the value

		// analog input value has changed
		input.iGICode = gi_codes[i];
		input.fParam1 = (float)m_aAxisPosition[i] * 0.001f; // floating point value scaled to [0,1]
		input.fParam1 *= m_afAnalogInputScale[i]; // scale to a user defined value if necessary
//		input.iParam = m_aAxisPosition[i]; // non-scaled original value (signed integer)

		GetInputHub().UpdateInput(input);
	}

	if( m_bSendExtraDigitalInputFromAnalogInput )
        SendAnalogInputAsDigitalInput();

	if( m_bSendExtraDigitalInputFromPOVInput )
		SendPOVInputAsDigitalInput();
/*
	StateLog.Update( 8, "gpd.axis-x: " + to_string(m_aAxisPosition[AXIS_X]) );
	StateLog.Update( 9, "gpd.axis-y: " + to_string(m_aAxisPosition[AXIS_Y]) );
	StateLog.Update(10, "gpd.axis-z: " + to_string(m_aAxisPosition[AXIS_Z]) );
	StateLog.Update(11, "gpd.rotation-x: " + to_string(m_aAxisPosition[ROTATION_X]) );
	StateLog.Update(12, "gpd.rotation-y: " + to_string(m_aAxisPosition[ROTATION_Y]) );
	StateLog.Update(13, "gpd.rotation-z: " + to_string(m_aAxisPosition[ROTATION_Z]) );
*/
	return S_OK;
}


bool DirectInputGamepad::IsKeyPressed( int gi_code )
{
	DWORD& pov0 = m_InputState.rgdwPOV[0];
	switch( gi_code )
	{
	case GIC_GPD_BUTTON_00: return 0 < (m_InputState.rgbButtons[ 0] & 0x80);
	case GIC_GPD_BUTTON_01: return 0 < (m_InputState.rgbButtons[ 1] & 0x80);
	case GIC_GPD_BUTTON_02: return 0 < (m_InputState.rgbButtons[ 2] & 0x80);
	case GIC_GPD_BUTTON_03: return 0 < (m_InputState.rgbButtons[ 3] & 0x80);
	case GIC_GPD_BUTTON_04: return 0 < (m_InputState.rgbButtons[ 4] & 0x80);
	case GIC_GPD_BUTTON_05: return 0 < (m_InputState.rgbButtons[ 5] & 0x80);
	case GIC_GPD_BUTTON_06: return 0 < (m_InputState.rgbButtons[ 6] & 0x80);
	case GIC_GPD_BUTTON_07: return 0 < (m_InputState.rgbButtons[ 7] & 0x80);
	case GIC_GPD_BUTTON_08: return 0 < (m_InputState.rgbButtons[ 8] & 0x80);
	case GIC_GPD_BUTTON_09: return 0 < (m_InputState.rgbButtons[ 9] & 0x80);
	case GIC_GPD_BUTTON_10: return 0 < (m_InputState.rgbButtons[10] & 0x80);
	case GIC_GPD_BUTTON_11: return 0 < (m_InputState.rgbButtons[11] & 0x80);

	case GIC_GPD_UP:
		return ( 0 <= pov0 && pov0 <= 4500 ) || ( 31500 <= pov0 && pov0 < 36000 );
	case GIC_GPD_RIGHT:
		return ( m_InputState.rgdwPOV[0] == 9000 );
	case GIC_GPD_DOWN:
		return ( m_InputState.rgdwPOV[0] == 18000 );
	case GIC_GPD_LEFT:
		return ( m_InputState.rgdwPOV[0] == 27000 );
	default:
		return false;
	}

	return false;
}


void DirectInputGamepad::RefreshKeyStates()
{
    if( NULL == m_pDIJoystick ) 
        return;// S_OK;
    
    // Get the input's device state, and put the state in dims
	HRESULT hr = m_pDIJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &m_InputState );

	if( m_InputState.rgbButtons[0] & 0x80
	 || m_InputState.rgbButtons[1] & 0x80
	 || m_InputState.rgbButtons[2] & 0x80
	 || m_InputState.rgbButtons[3] & 0x80
	 || m_InputState.rgbButtons[4] & 0x80
	 || m_InputState.rgbButtons[5] & 0x80
	 || m_InputState.rgbButtons[6] & 0x80
	 || m_InputState.rgbButtons[7] & 0x80
	 || m_InputState.rgbButtons[8] & 0x80
	 || m_InputState.rgbButtons[9] & 0x80 )
	{
		int break_here = 1;
	}

    return;// S_OK;
}


Result::Name DirectInputGamepad::SendBufferedInputToInputHandlers()
{
	HRESULT hr = ReadBufferedData();

	if( SUCCEEDED(hr) )
		return Result::SUCCESS;
	else
		return Result::UNKNOWN_ERROR;
}


void DirectInputGamepad::SendAnalogInputAsDigitalInput()
{
	static const int s_GICodeForBinarizedAnalogInput[] =
	{
		GIC_GPD_AXIS_X_D,     ///< AXIS_X,
		GIC_GPD_AXIS_Y_D,     ///< AXIS_Y,
		GIC_GPD_AXIS_Z_D,     ///< AXIS_Z,
		GIC_GPD_ROTATION_X_D, ///< ROTATION_X,
		GIC_GPD_ROTATION_Y_D, ///< ROTATION_Y,
		GIC_GPD_ROTATION_Z_D, ///< ROTATION_Z,
	};

	InputData input;
	for( int i=0; i<NUM_ANALOG_CONTROLS; i++ )
	{
		const int& dirkey_threshold = 700;

		bool prev_hold = m_bPrevHold[i];
		bool hold = dirkey_threshold < abs(m_aAxisPosition[i]) ? true : false;

		m_bPrevHold[i] = hold;

		input.iGICode = s_GICodeForBinarizedAnalogInput[i];

		// send pressed/released event

		if( !prev_hold && hold )
		{
			input.iType = ITYPE_KEY_PRESSED;
			input.fParam1 = 1.0f;
			GetInputHub().UpdateInput(input);
		}
		else if( prev_hold && !hold )
		{
			input.iType = ITYPE_KEY_RELEASED;
			input.fParam1 = 0.0f;
			GetInputHub().UpdateInput(input);
		}

		// update input state
		UpdateInputState( input );
	}
}


static inline int GetGICodeFromPOV( int pov )
{
	switch( pov )
	{
	case 0:     return GIC_GPD_UP;
	case 9000:  return GIC_GPD_RIGHT;
	case 18000: return GIC_GPD_DOWN;
	case 27000: return GIC_GPD_LEFT;
	default:    return GIC_GPD_DOWN;
	}
}


void DirectInputGamepad::SendPOVInputAsDigitalInput()
{
	InputData input;
	for( int i=0; i<1/*NUM_POV_INPUTS*/; i++ )
	{
		if( m_aPrevPOV[i] == -1 && m_aPOV[i] != -1 )
		{
			input.iGICode = GetGICodeFromPOV( m_aPOV[i] );
			input.iType = ITYPE_KEY_PRESSED;
			input.fParam1 = 1.0f;
			GetInputHub().UpdateInput(input);

			// update input state
			UpdateInputState( input );
		}
		else if( m_aPrevPOV[i] != -1 && m_aPOV[i] == -1 )
		{
			input.iGICode = GetGICodeFromPOV( m_aPrevPOV[i] );
			input.iType = ITYPE_KEY_RELEASED;
			input.fParam1 = 0.0f;
			GetInputHub().UpdateInput(input);

			// update input state
			UpdateInputState( input );
		}
	}

	// save the current pov indicator values
	memcpy( m_aPrevPOV, m_aPOV, sizeof(m_aPOV) );
}


CForceFeedbackEffect DirectInputGamepad::CreateForceFeedbackEffect( const CForceFeedbackEffectDesc& desc )
{
	CForceFeedbackEffect ffe;

	if( !m_pDIJoystick )
		return ffe;

	boost::shared_ptr<CDIForceFeedbackEffectImpl> pImpl( new CDIForceFeedbackEffectImpl() );

	Result::Name res = pImpl->Init( desc, m_pFFParams );
	if( res != Result::SUCCESS )
		return ffe;

	SetImplToForceFeedbackEffect( pImpl, ffe );

	return ffe;
}


Result::Name DirectInputGamepad::InitForceFeedbackEffect( CDIForceFeedbackEffectImpl& impl )
{
	Result::Name res = impl.Init( m_pFFParams );

	return res;
}


void DirectInputGamepad::GetStatus( std::vector<std::string>& buffer )
{
	buffer.reserve( NUM_ANALOG_CONTROLS + 1 );

	buffer.push_back( m_InstanceName + " (" +  m_ProductName + "):\n" );

	char line[64], val[16];
	for( int i=0; i<NUM_ANALOG_CONTROLS; i+=4 )
	{
		int start = i;
		int end = take_min( i+3, NUM_ANALOG_CONTROLS - 1 );
		sprintf( line, "axis[%02d:%02d]", start, end );
		for( int j=start; j<=end; j++ )
		{
			int pos = (int)m_aAxisPosition[j];
			sprintf( val, " %04d |", pos );
			strcat( line, val );
		}
		strcat( line, "\n" );
		buffer.push_back( line );
	}
}


} // namespace amorphous
