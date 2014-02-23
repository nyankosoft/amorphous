#ifndef __DebugInfo_InputDevice_HPP__
#define __DebugInfo_InputDevice_HPP__


#include "amorphous/Support/DebugInfo.hpp"
#include "InputDevice.hpp"


namespace amorphous
{


class DebugInfo_InputDevice : public DebugInfo
{
public:

	DebugInfo_InputDevice() {}

	void UpdateDebugInfoText()
	{
		GetInputDeviceHub().GetInputDeviceStatus( m_MultiLineTextBuffer );
	}
};


} // namespace amorphous


#endif /* __DebugInfo_InputDevice_HPP__ */
