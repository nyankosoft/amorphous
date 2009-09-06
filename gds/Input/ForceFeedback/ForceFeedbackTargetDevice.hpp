#ifndef __ForceFeedbackTargetDevice_HPP__
#define __ForceFeedbackTargetDevice_HPP__

#include "../../base.hpp"
#include "fwd.hpp"
#include "Input/fwd.hpp"
#include "Input/InputDevice.hpp"


class CForceFeedbackTargetDevice
{
public:
	int m_MinAxes;
	int m_MaxAxes;
	CInputDevice::InputDeviceType m_Type;
	int m_Group;

public:
	CForceFeedbackTargetDevice()
		:
	m_MinAxes(0),
	m_MaxAxes(0xFFFF),
	m_Type(CInputDevice::TYPE_GAMEPAD),
	m_Group(0)
	{}
};



#endif /* __ForceFeedbackTargetDevice_HPP__ */
