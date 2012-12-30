#ifndef  __DIRECTINPUTGAMEPAD_H__
#define  __DIRECTINPUTGAMEPAD_H__


#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include "../base.hpp"

#include "InputDevice.hpp"


namespace amorphous
{


#pragma comment( lib, "dinput8.lib" )


/// direct input force feedback params
class CDIFFParams
{
public:
	LPDIRECTINPUTDEVICE8 m_pDeviceCopy;
	uint m_NumFFAxes;
};


/**
 * TODO:
 * - customizable threshold for analog to digital conversion
 *
 */
class CDirectInputGamepad : public CInputDevice
{
public:

	/// NOTE: update s_GICodeForBinarizedAnalogInput in DirectInputGamepad.cpp when these enums are modified
	enum Axis
	{
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
		ROTATION_X,
		ROTATION_Y,
		ROTATION_Z,
		NUM_ANALOG_CONTROLS
	};

private:

	LPDIRECTINPUTDEVICE8 m_pDIJoystick;

	DIJOYSTATE2 m_InputState;

	boost::shared_ptr<CDIFFParams> m_pFFParams;

	enum param
	{
		DIJOYSTICK_BUFFER_SIZE = 32,    ///< size of the buffer to hold input data from mouse (DirectInput)
		VALUE_NOT_FOUND = -1000000,     ///< used in ReadBufferedData() to mean that input data for an axis was not found
	};

	enum pov
	{
		POV_0,
		POV_1,
		POV_2,
		POV_3,
		NUM_POV_INPUTS
	};

	/// holds absolute position of the analog stick
	/// TODO: change value range from [-1000,1000] to [-1,1]
	int m_aAxisPosition[NUM_ANALOG_CONTROLS];

	bool m_bPrevHold[NUM_ANALOG_CONTROLS];

	/// holds POV(point-of-view) indicator values
	/// - initialized to -1 in ctor
	int m_aPOV[NUM_POV_INPUTS];
	int m_aPrevPOV[NUM_POV_INPUTS];

//	float m_fAnalogToDigitalThreshold[NUM_ANALOG_CONTROLS];

	bool m_bSendExtraDigitalInputFromAnalogInput;

	bool m_bSendExtraDigitalInputFromPOVInput;

	/// Used to scale the analog input to values other than 1.0 (default: 1.0).
	/// e.g. To change the value range of AXIS_X to [0,2] instead of [0,1],
	/// set m_afAnalogInputScale[AXIS_X] to 2
	float m_afAnalogInputScale[NUM_ANALOG_CONTROLS];

private:

	void SendAnalogInputAsDigitalInput();

	void SendPOVInputAsDigitalInput();

	HRESULT InitDIGamepad( HWND hWnd );

	HRESULT ReadBufferedData();

protected:

	virtual bool IsKeyPressed( int gi_code );

	virtual bool IsReleventInput( int gi_code ) { return IsGamepadInputCode( gi_code ); }

	virtual void RefreshKeyStates();

public:

	CDirectInputGamepad();

	~CDirectInputGamepad();

	CInputDevice::InputDeviceType GetInputDeviceType() const { return TYPE_GAMEPAD; }

	Result::Name Init();

	Result::Name InitDevice( const DIDEVICEINSTANCE& di );

	Result::Name CreateDevice( const DIDEVICEINSTANCE& di );

	void Release();

	HRESULT Acquire();

	void Unacquire() { m_pDIJoystick->Unacquire(); }

	Result::Name SendBufferedInputToInputHandlers();

	CForceFeedbackEffect CreateForceFeedbackEffect( const CForceFeedbackEffectDesc& desc );

	Result::Name InitForceFeedbackEffect( CDIForceFeedbackEffectImpl& impl );

	void SetAnalogInputScale( Axis analog_axis, float scale ) { m_afAnalogInputScale[analog_axis] = scale; }

	void GetStatus( std::vector<std::string>& buffer );
};
} // namespace amorphous



#endif		/*  __DIRECTINPUTGAMEPAD_H__  */
