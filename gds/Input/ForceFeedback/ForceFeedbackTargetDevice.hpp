#ifndef __ForceFeedbackTargetDevice_HPP__
#define __ForceFeedbackTargetDevice_HPP__

#include "../../base.hpp"
#include "fwd.hpp"
#include "Input/fwd.hpp"
#include "Input/InputDevice.hpp"


namespace amorphous
{


class CForceFeedbackTargetDevice
{
public:
	int m_MinAxes;
	int m_MaxAxes;
	InputDevice::InputDeviceType m_Type;
	int m_Group;

public:
	CForceFeedbackTargetDevice()
		:
	m_MinAxes(0),
	m_MaxAxes(0xFFFF),
	m_Type(InputDevice::TYPE_GAMEPAD),
	m_Group(0)
	{}
};


} // amorphous



#endif /* __ForceFeedbackTargetDevice_HPP__ */
