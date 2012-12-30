#ifndef  __MouseInputDevice_HPP__
#define  __MouseInputDevice_HPP__


#include "InputDevice.hpp"


namespace amorphous
{


/// - Send mouse positions in reference screen coordinates
/// - When the screen resolution is changed, the mouse device object needs to be notified
///   by MouseInputDevice::UpdateScreenSize()
/// - Internally, MouseInputDevice stores the cursor position in non-scaled coordinates
class MouseInputDevice : public CInputDevice
{
	enum eMouseAction
	{
		MOUSE_BUTTON_L = 0,
		MOUSE_BUTTON_R,
		MOUSE_BUTTON_M,
		MOUSE_AXIS_X,
		MOUSE_AXIS_Y,
		MOUSE_AXIS_Z

	};

public:

	MouseInputDevice() {}

	virtual ~MouseInputDevice() {}

	CInputDevice::InputDeviceType GetInputDeviceType() const { return TYPE_MOUSE; }

	virtual void AcquireMouse() {}

//	void UnacquireMouse() { m_pDIMouse->Unacquire(); }

	/// updates the rect area for the mouse cursor
	virtual void UpdateScreenSize( int width, int height ) {}

/*	float GetSensitivity( int iMouseAxis ) { return m_afMouseSensitivity[iMouseAxis]; }
	void SetSensitivity( int iMouseAxis, float fSensitivity ) { m_afMouseSensitivity[iMouseAxis] = fSensitivity; }
	bool InvertMouse();
	void SetInvertMouse( bool bEnableInvertMouse );
*/
	/// returns the current mouse cursor position in the current screen resolution
	virtual void GetCurrentPosition( int& x, int& y ) const {}

	virtual int GetCurrentPositionX() const { return 0; }
	virtual int GetCurrentPositionY() const { return 0; }
};

} // amorphous



#endif		/*  __MouseInputDevice_HPP__  */
