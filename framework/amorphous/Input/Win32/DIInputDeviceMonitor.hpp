#ifndef  __DIInputDeviceMonitor_HPP__
#define  __DIInputDeviceMonitor_HPP__

#include "amorphous/base.hpp"

#include <vector>
#include <map>
#include <memory>
#include <tbb/concurrent_queue.h>
#include <tbb/mutex.h>
#include "amorphous/Input/fwd.hpp"
#include "amorphous/Input/Win32/DirectInput.hpp"
#include "amorphous/Support/thread_starter.hpp"
#include "amorphous/Support/singleton.hpp"


namespace amorphous
{


class InputDeviceStateCallback
{
public:

	virtual ~InputDeviceStateCallback() {}

	virtual void OnInputDeviceDetected() {}
	virtual void OnInputDeviceInitialized() {}
	virtual void OnInputDeviceInitFailed() {}
	virtual void OnInputDeviceUnplugged() {}
};


class CDIInputDeviceContainer
{
public:

	std::shared_ptr<DirectInputGamepad> m_pGamepad;

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


class CDIInputDeviceManagementRequest
{
public:
	enum Type
	{
		CREATE_DEVICE,
		RELEASE_DEVICE,
		INVALID,
		NUM_REQUEST_TYPES
	};

	Type m_Type;
	DIDEVICEINSTANCE m_DeviceInstance;

public:

	CDIInputDeviceManagementRequest( Type type = INVALID )
		:
	m_Type(type)
	{}
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

//	std::map<GUID,CDIInputDeviceContainer,less_for_guid> m_mapGUIDtoDIDeviceInstance;
	std::vector<CDIInputDeviceContainer> m_vecDIDeviceInstanceContainer;

	tbb::mutex m_DeviceContainerMutex;

	std::vector<DIDEVICEINSTANCE> m_vecDIDeviceInstanceHolder;

	tbb::concurrent_queue<CDIInputDeviceManagementRequest> m_queDIDeviceRequest;

	bool m_ExitThread;

	std::shared_ptr<InputDeviceStateCallback> m_pCallback;

private:

	bool CreateDevice( CDIInputDeviceContainer& container );

	int GetContainerIndex( const GUID& guid );

	bool AlreadyRequested( const GUID& guid );

	/// Called by the main thread
	void ProcessRequest();

protected:

	/// singleton
	static singleton<CDIInputDeviceMonitor> m_obj;

public:

	CDIInputDeviceMonitor()
		:
	m_ExitThread(false)
	{}

	void CheckDevices();

//	void ThreadMain();

	static CDIInputDeviceMonitor* Get() { return m_obj.get(); }

//	std::vector<DIDEVICEINSTANCE>& DeviceInstanceHolder() { return m_vecDIDeviceInstanceHolder; }

	void AddDIDeviceInstance( const DIDEVICEINSTANCE& di ) { m_vecDIDeviceInstanceHolder.push_back( di ); }

	void ResetEnumStatus();

	void run();

	void ExitThread() { m_ExitThread = true; }

	void ProcessInputDeviceManagementRequest();

	void AcquireInputDevices();

	void RegisterCallback( std::shared_ptr<InputDeviceStateCallback> pCallback ) { m_pCallback = pCallback; }

	void UnregisterCallback() { m_pCallback = std::shared_ptr<InputDeviceStateCallback>(); }
};


inline CDIInputDeviceMonitor& DIInputDeviceMonitor()
{
	return (*CDIInputDeviceMonitor::Get());
}


} // namespace amorphous



#endif  /* __DIInputDeviceMonitor_HPP__ */

