#ifndef __amorphous_InputDeviceInitializer_DirectInput_HPP__
#define __amorphous_InputDeviceInitializer_DirectInput_HPP__


#include "DirectInputMouse.hpp"
#include "DIKeyboard.hpp"


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


} // namespace amorphous


#endif /* __amorphous_InputDeviceInitializer_DirectInput_HPP__ */
