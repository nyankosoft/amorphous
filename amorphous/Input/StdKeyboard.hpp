#ifndef  __STDKEYBOARD_H__
#define  __STDKEYBOARD_H__


#include "InputDevice.hpp"


namespace amorphous
{


/**
 Handles input from keyboard in Windows platform
 - takes keyup & keydown notification as virtual keycode
   and reports it as general input code

*/
class Win32StdKeyboard : public InputDevice
{
	int m_aiGICodeForVKCode[NUM_GENERAL_INPUT_CODES];

public:
	Win32StdKeyboard();
	~Win32StdKeyboard() {}

	InputDevice::InputDeviceType GetInputDeviceType() const { return TYPE_KEYBOARD; }

	Result::Name SendBufferedInputToInputHandlers();

	void NotifyKeyDown( int iVK_Code );
	void NotifyKeyUp( int iVK_Code );

};


inline Win32StdKeyboard& StdWin32Keyboard()
{
	static Win32StdKeyboard s_kbd;

	return s_kbd;
}


/*
---------------------------- template ----------------------------
 - add the following code to Windows message handler
 
	switch( msg )
	{
	// ... some other message handlings ...

	case WM_KEYDOWN:
		StdWin32Keyboard().NotifyKeyDown( (int)wParam );
		break;

	case WM_KEYUP:
		StdWin32Keyboard().NotifyKeyUp( (int)wParam );
		break;

	// ... some more message handlings ...

	default:
		break;
	}

*/

} // namespace amorphous



#endif		/*  __STDKEYBOARD_H__  */