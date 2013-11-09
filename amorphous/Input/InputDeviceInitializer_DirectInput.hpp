#ifndef __amorphous_InputDeviceInitializer_DirectInput_HPP__
#define __amorphous_InputDeviceInitializer_DirectInput_HPP__


#include "DirectInputMouse.hpp"
#include "DIKeyboard.hpp"
#include "DirectInputGamepad.hpp"


namespace amorphous
{


inline MouseInputDevice *CreateMouseInputDevice()
{
	return new DirectInputMouse;
}


inline KeyboardInputDevice *CreateKeyboardInputDevice()
{
	return new DIKeyboard;
}


inline GamepadInputDevice *CreateGamepadInputDevice()
{
	return new DirectInputGamepad;
}


} // namespace amorphous


#endif /* __amorphous_InputDeviceInitializer_DirectInput_HPP__ */
