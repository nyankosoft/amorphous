#ifndef __InputDeviceGroup_HPP__
#define __InputDeviceGroup_HPP__


#include "fwd.hpp"
#include "InputHandler.hpp"

/// auto repeat control requires the timer
#include "gds/Support/Timer.hpp"
#include "gds/Support/FixedVector.hpp"


namespace amorphous
{


class InputDeviceGroup
{
	std::vector<InputDevice *> m_vecpDevice;

	CInputState m_aInputState[NUM_GENERAL_INPUT_CODES];

	TCFixedVector<int,InputDeviceParam::NUM_MAX_SIMULTANEOUS_PRESSES> m_PressedKeyList;

public:

	InputDeviceGroup() {}

	~InputDeviceGroup() {}

	CInputState::Name GetInputState( int gi_code ) const { return m_aInputState[gi_code].m_State; }

	std::vector<InputDevice *>& InputDevice() { return m_vecpDevice; }

	friend class InputDevice;
	friend class InputDeviceHub;
};

} // namespace amorphous



#endif  /*  __InputDeviceGroup_HPP__  */
