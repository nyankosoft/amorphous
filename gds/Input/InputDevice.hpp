#ifndef __InputDevice_H__
#define __InputDevice_H__


#include <boost/shared_ptr.hpp>
#include "fwd.hpp"
#include "InputHub.hpp"
#include "InputDeviceGroup.hpp"

/// auto repeat control requires the timer
#include "Support/Timer.hpp"
#include "Support/FixedVector.hpp"


class CInputDevice
{
	enum Param
	{
		FIRST_AUTO_REPEAT_INTERVAL_MS = 300,
//		NUM_MAX_SIMULTANEOUS_PRESSES  = 4,
	};

	void SetGroup( CInputDeviceGroup *pGroup );

protected:

	CInputDeviceGroup *m_pGroup;

protected:

	virtual bool IsKeyPressed( int gi_code ) { return false; }

	virtual bool IsReleventInput( int gi_code ) { return false; }

	virtual void RefreshKeyStates() {}

public:

	enum InputDeviceType
	{
		TYPE_GAMEPAD,
		TYPE_KEYBOARD,
		TYPE_MOUSE,
		NUM_INPUT_DEVICE_TYPES
	};

public:

	CInputDevice();

	virtual ~CInputDevice();

	virtual CInputDevice::InputDeviceType GetInputDeviceType() const = 0;

	TCFixedVector<int,CInputDeviceParam::NUM_MAX_SIMULTANEOUS_PRESSES>& PressedKeyList() { return m_pGroup->m_PressedKeyList; }

	CInputState& InputState( int gi_code ) { return m_pGroup->m_aInputState[gi_code]; }

	void UpdateInputState( const SInputData& input_data );

	virtual Result::Name Init() { return Result::SUCCESS; }

	virtual Result::Name SendBufferedInputToInputHandlers() = 0;

	void CheckPressedKeys();

	friend class CInputDeviceHub;
};



/// Used as a singleton class
class CInputDeviceHub
{
	std::vector<CInputDevice *> m_vecpInputDevice;

	std::vector< boost::shared_ptr<CInputDeviceGroup> > m_vecpGroup;

public:

	CInputDeviceHub();

	void RegisterInputDevice( CInputDevice *pDevice );

	void UnregisterInputDevice( CInputDevice *pDevice );

	void RegisterInputDeviceToGroup( CInputDevice *pDevice );

	void UnregisterInputDeviceFromGroup( CInputDevice *pDevice );

	void SendInputToInputHandlers();

	void SendAutoRepeat();

	void SendAutoRepeat( CInputDeviceGroup& group );

	boost::shared_ptr<CInputDeviceGroup> GetInputDeviceGroup( int i ) { return m_vecpGroup[i]; }
};


inline CInputDeviceHub& InputDeviceHub()
{
	static CInputDeviceHub s_instance;
	return s_instance;
}



#endif  /*  __InputDevice_H__  */
