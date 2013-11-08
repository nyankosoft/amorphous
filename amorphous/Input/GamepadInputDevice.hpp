#ifndef  __amorphous_GamepadInputDevice_HPP__
#define  __amorphous_GamepadInputDevice_HPP__


#include "InputDevice.hpp"


namespace amorphous
{


class GamepadInputDevice : public InputDevice
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

public:

	GamepadInputDevice() {}

	virtual ~GamepadInputDevice() {}

	InputDevice::InputDeviceType GetInputDeviceType() const { return TYPE_GAMEPAD; }

	virtual Result::Name Init() { return Result::SUCCESS; }

	virtual Result::Name Acquire() { return Result::UNKNOWN_ERROR; }

	virtual Result::Name SendBufferedInputToInputHandlers() { return Result::SUCCESS; }

//	virtual CForceFeedbackEffect CreateForceFeedbackEffect( const CForceFeedbackEffectDesc& desc ) { return CForceFeedbackEffect(); }

	virtual Result::Name InitForceFeedbackEffect( CDIForceFeedbackEffectImpl& impl ) { return Result::UNKNOWN_ERROR; }

	virtual void SetAnalogInputScale( Axis analog_axis, float scale ) {}//{ m_afAnalogInputScale[analog_axis] = scale; }

	virtual void GetStatus( std::vector<std::string>& buffer ) {}
};


} // amorphous


#endif		/*  __amorphous_GamepadInputDevice_HPP__  */
