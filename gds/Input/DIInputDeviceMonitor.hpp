#ifndef  __DIInputDeviceMonitor_HPP__
#define  __DIInputDeviceMonitor_HPP__

#include "../base.hpp"

#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include "Input/fwd.hpp"
#include "Input/DirectInput.hpp"
#include "Support/thread_starter.hpp"
#include "Support/Singleton.hpp"
using namespace NS_KGL;


class CDIInputDeviceContainer
{
public:

	boost::shared_ptr<CDirectInputGamepad> m_pGamepad;

	DIDEVICEINSTANCE m_DeviceInstance;

	bool m_bAttached;

public:

	CDIInputDeviceContainer()
	{}

	CDIInputDeviceContainer( const DIDEVICEINSTANCE& di )
		:
	m_DeviceInstance(di)
	{}

	void SetAttached( bool attached ) { m_bAttached = attached; }

	bool GetAttached() const { return m_bAttached; }
};



class CDIInputDeviceMonitor : public thread_class
{
	struct less_for_guid : public std::binary_function<GUID, GUID, bool>
	{
		// No need to sort - just compare the pointer values
		bool operator()(const GUID& lhs, const GUID& rhs) const
		{
			// apply operator< to operands
			return ((&lhs) < (&rhs));
		}
	};

	std::map<GUID,CDIInputDeviceContainer,less_for_guid> m_mapGUIDtoDIDeviceInstance;

	std::vector<DIDEVICEINSTANCE> m_vecDIDeviceInstanceHolder;

	bool m_ExitThread;

private:

	bool CreateDevice( CDIInputDeviceContainer& container );

protected:

	/// singleton
	static CSingleton<CDIInputDeviceMonitor> m_obj;

public:
/*
	CDIInputDeviceMonitor()
		:
	m_ExitThread(false)
	{}
*/
	void CheckDevices();

//	void ThreadMain();

	static CDIInputDeviceMonitor* Get() { return m_obj.get(); }

//	std::vector<DIDEVICEINSTANCE>& DeviceInstanceHolder() { return m_vecDIDeviceInstanceHolder; }

	void AddDIDeviceInstance( const DIDEVICEINSTANCE& di ) { m_vecDIDeviceInstanceHolder.push_back( di ); }

	void ResetEnumStatus();

	void run();

	void ExitThread() { m_ExitThread = true; }

	void AcquireInputDevices();
};


inline CDIInputDeviceMonitor& DIInputDeviceMonitor()
{
	return (*CDIInputDeviceMonitor::Get());
}



#endif  /* __DIInputDeviceMonitor_HPP__ */

