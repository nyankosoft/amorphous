#ifndef __amorphous_InputDeviceInitializer_Win32_HPP__
#define __amorphous_InputDeviceInitializer_Win32_HPP__


#include "StdKeyboard.hpp"
#include "StdMouseInput.hpp"


namespace amorphous
{


inline MouseInputDevice *CreateMouseInputDevice()
{
	return new Win32StdMouseInput;
}


inline KeyboardInputDevice *CreateKeyboardInputDevice()
{
	return new Win32StdKeyboard;
}


inline GamepadInputDevice *CreateGamepadInputDevice()
{
	return NULL;
}


} // namespace amorphous


#endif /* __amorphous_InputDeviceInitializer_Win32_HPP__ */
