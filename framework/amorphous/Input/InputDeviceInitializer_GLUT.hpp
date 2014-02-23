#ifndef __amorphous_InputDeviceInitializer_GLUT_HPP__
#define __amorphous_InputDeviceInitializer_GLUT_HPP__


#include "GLUTMouse.hpp"
#include "GLUTKeyboard.hpp"


namespace amorphous
{


inline MouseInputDevice *CreateMouseInputDevice()
{
	return new GLUTMouse;
}


inline KeyboardInputDevice *CreateKeyboardInputDevice()
{
	return new GLUTKeyboard;
}


} // namespace amorphous


#endif /* __amorphous_InputDeviceInitializer_GLUT_HPP__ */
