#ifndef  __amorphous_KeyboardInputDevice_HPP__
#define  __amorphous_KeyboardInputDevice_HPP__


#include "InputDevice.hpp"


namespace amorphous
{


class KeyboardInputDevice : public InputDevice
{
public:

	KeyboardInputDevice() {}

	virtual ~KeyboardInputDevice() {}

	InputDevice::InputDeviceType GetInputDeviceType() const { return TYPE_KEYBOARD; }

	virtual Result::Name Init() { return Result::SUCCESS; }

	virtual void Acquire() {}

	virtual Result::Name SendBufferedInputToInputHandlers() { return Result::SUCCESS; }

};


} // amorphous


#endif		/*  __amorphous_KeyboardInputDevice_HPP__  */
