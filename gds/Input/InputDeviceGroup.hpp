#ifndef __InputDeviceGroup_HPP__
#define __InputDeviceGroup_HPP__


#include "fwd.hpp"
#include "InputHandler.hpp"

/// auto repeat control requires the timer
#include "Support/Timer.hpp"
#include "Support/FixedVector.hpp"


class CInputDeviceGroup
{
	std::vector<CInputDevice *> m_vecpDevice;

	CInputState m_aInputState[NUM_GENERAL_INPUT_CODES];

	TCFixedVector<int,CInputDeviceParam::NUM_MAX_SIMULTANEOUS_PRESSES> m_PressedKeyList;

public:

	CInputDeviceGroup() {}

	~CInputDeviceGroup() {}

	CInputState::Name GetInputState( int gi_code ) const { return m_aInputState[gi_code].m_State; }

	friend class CInputDevice;
	friend class CInputDeviceHub;
};


#endif  /*  __InputDeviceGroup_HPP__  */
