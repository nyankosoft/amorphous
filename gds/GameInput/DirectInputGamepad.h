#ifndef  __DIRECTINPUTGAMEPAD_H__
#define  __DIRECTINPUTGAMEPAD_H__


#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include "../base.h"

#pragma comment( lib, "dinput8.lib" )


/**
 * TODO:
 * - customizable threshold for analog to digital conversion
 *
 */
class CDirectInputGamepad// : public CInputDevice
{
	LPDIRECTINPUTDEVICE8 m_pDIJoystick;

	enum param
	{
		DIJOYSTICK_BUFFER_SIZE = 32, ///< size of the buffer to hold input data from mouse (DirectInput)
	};

	/// NOTE: update s_GICodeForBinarizedAnalogInput in DirectInputGamepad.cpp when these enums are modified
	enum axis
	{
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
		ROTATION_X,
		ROTATION_Y,
		ROTATION_Z,
		NUM_ANALOG_CONTROLS
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
	float m_afAxisPosition[NUM_ANALOG_CONTROLS];

	bool m_bPrevHold[NUM_ANALOG_CONTROLS];

	/// holds POV(point-of-view) indicator values
	/// - initialized to -1 in ctor
	int m_aPOV[NUM_POV_INPUTS];
	int m_aPrevPOV[NUM_POV_INPUTS];

//	float m_fAnalogToDigitalThreshold[NUM_ANALOG_CONTROLS];

	bool m_bSendExtraDigitalInputFromAnalogInput;

	bool m_bSendExtraDigitalInputFromPOVInput;

private:

	void SendAnalogInputAsDigitalInput();

	void SendPOVInputAsDigitalInput();

	HRESULT InitDIGamepad( HWND hWnd );

public:

	CDirectInputGamepad();

	~CDirectInputGamepad();

	Result::Name Init();

	void Release();

	HRESULT Acquire();

	void Unacquire() { m_pDIJoystick->Unacquire(); }

//	float GetSensitivity( int iMouseAxis ) { return m_afMouseSensitivity[iMouseAxis]; }
//	void SetSensitivity( int iMouseAxis, float fSensitivity ) { m_afMouseSensitivity[iMouseAxis] = fSensitivity; }

	HRESULT UpdateInput();

};

#endif		/*  __DIRECTINPUTGAMEPAD_H__  */
