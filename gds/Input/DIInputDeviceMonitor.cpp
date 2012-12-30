#include "DIInputDeviceMonitor.hpp"
#include "Input/DirectInputGamepad.hpp"
#include "Support/Log/DefaultLog.hpp"
#include <boost/thread/xtime.hpp>


namespace amorphous
{

using namespace std;


static LPDIRECTINPUTDEVICE8 g_pDITempJoystickDevice = NULL;

// draft
void OnInputDevicePlugged();


// FIXME: Multiple CDIInputDeviceManagementRequest objects are created for the same gamepad device
// - Happens especially with small sleep time in CDIInputDeviceMonitor::run()
// - Second request is created when the first request is being processed and the gamepad device is being initialized.

BOOL CALLBACK EnumGameControllersCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext )
{
//	HRESULT hr;

//	DIInputDeviceMonitor().DeviceInstanceHolder().push_back(  );

	DIInputDeviceMonitor().AddDIDeviceInstance( *pdidInstance );

    return DIENUM_CONTINUE;
}


CSingleton<CDIInputDeviceMonitor> CDIInputDeviceMonitor::m_obj;


void CDIInputDeviceMonitor::ResetEnumStatus()
{
//	map<GUID,CDIInputDeviceContainer,less_for_guid>::iterator itr;

//	for( itr = m_mapGUIDtoDIDeviceInstance.begin();
//		 itr != m_mapGUIDtoDIDeviceInstance.end();
	for( vector<CDIInputDeviceContainer>::iterator itr = m_vecDIDeviceInstanceContainer.begin();
		itr != m_vecDIDeviceInstanceContainer.end();
		 itr++ )
	{
//		itr->second.SetAttached( false );
		itr->SetAttached( false );
	}
}


bool CDIInputDeviceMonitor::CreateDevice( CDIInputDeviceContainer& container )
{
	container.m_pGamepad.reset( new CDirectInputGamepad() );

	Result::Name res = container.m_pGamepad->InitDevice( container.m_DeviceInstance );

	if( res != Result::SUCCESS )//|| !container.m_pGamepad->IsValid() )
	{
		string log_text = fmt_string( "Failed to initialize the input device (instance: %s, product: %s).",
			container.m_DeviceInstance.tszInstanceName,
			container.m_DeviceInstance.tszProductName );

		LOG_PRINT_WARNING( log_text );
		container.m_pGamepad.reset();

		if( m_pCallback )
			m_pCallback->OnInputDeviceInitFailed();

		return false;
	}
	else
	{
		if( m_pCallback )
			m_pCallback->OnInputDeviceInitialized();

		return true;
	}
}


int CDIInputDeviceMonitor::GetContainerIndex( const GUID& guid )
{
	for( size_t i=0; i<m_vecDIDeviceInstanceContainer.size(); i++ )
	{
		if( m_vecDIDeviceInstanceContainer[i].m_DeviceInstance.guidInstance == guid )
			return (int)i;
	}

	return -1;
}


bool CDIInputDeviceMonitor::AlreadyRequested( const GUID& guid )
{
	tbb::concurrent_queue<CDIInputDeviceManagementRequest>::iterator itr;
	for( itr = m_queDIDeviceRequest.unsafe_begin();
		 itr != m_queDIDeviceRequest.unsafe_end();
		 itr++ )
	{
		if( itr->m_DeviceInstance.guidInstance == guid )
			return true;
	}

	return false;
}

// Enumerate all the input devices
// - Try to create input device objects for all the attached device.
// - The input device monitor thread keeps calling this at a regular interval to detect input devices plugged at runtime.
void CDIInputDeviceMonitor::CheckDevices()
{
	tbb::mutex::scoped_lock(m_DeviceContainerMutex);

//	LOG_PRINT( " Checking input devices..." );
	static uint s_CallCount = 0;
	s_CallCount++;

	HRESULT hr;

	m_vecDIDeviceInstanceHolder.resize( 0 );

	hr = DIRECTINPUT.GetDirectInputObject()->EnumDevices( DI8DEVCLASS_GAMECTRL, 
		                                                  EnumGameControllersCallback,
														  NULL, DIEDFL_ATTACHEDONLY );

	// m_vecDIDeviceInstanceHolder now contains enumerated device

	// Mark all devices as 'not attached'
	ResetEnumStatus();

	for( size_t i=0; i<m_vecDIDeviceInstanceHolder.size(); i++ )
	{
		DIDEVICEINSTANCE &di = m_vecDIDeviceInstanceHolder[i];

		int container_index = GetContainerIndex( di.guidInstance );
//		map<GUID,CDIInputDeviceContainer,less_for_guid>::iterator itr = m_mapGUIDtoDIDeviceInstance.find( di.guidInstance );

//		if( itr == m_mapGUIDtoDIDeviceInstance.end() )
		if( container_index < 0 )
		{
			if( !AlreadyRequested( di.guidInstance ) )
			{
				// A new game controller has been enumerated.
				// e.g. A gamepad is plugged at runtime

				CDIInputDeviceManagementRequest req(CDIInputDeviceManagementRequest::CREATE_DEVICE);
				req.m_DeviceInstance = di;

				m_queDIDeviceRequest.push( req );

				if( m_pCallback )
					m_pCallback->OnInputDeviceDetected();

				LOG_PRINT( fmt_string( " Added an input device creation request for '%s' (%d)", string(di.tszProductName).c_str(), s_CallCount ) );
			}
		}
		else
		{
			// Mark this device as attached
//			itr->second.SetAttached( true );
			m_vecDIDeviceInstanceContainer[container_index].SetAttached( true );
		}
	}
/*
	map<GUID,CDIInputDeviceContainer,less_for_guid>::iterator itr;

	for( itr = m_mapGUIDtoDIDeviceInstance.begin();
		 itr != m_mapGUIDtoDIDeviceInstance.end();
		 itr++ )
	{
		if( !itr->second.GetAttached() );
		{
			CDIInputDeviceManagementRequest req(CDIInputDeviceManagementRequest::RELEASE_DEVICE);
			req.m_DeviceInstance = di;

			m_queDIDeviceRequest.push( req c);
		}
	}
*/

	m_vecDIDeviceInstanceHolder.resize( 0 );
}


void CDIInputDeviceMonitor::run()
{
	boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC);

	while( !m_ExitThread )
	{
		CheckDevices();

		xt.sec += 2; // 1 [sec]
		boost::thread::sleep(xt);
	}
}


// create input device object for gamepad plugged to the computer
void CDIInputDeviceMonitor::ProcessInputDeviceManagementRequest()
{
	if( m_queDIDeviceRequest.empty() )
		return;

	ProcessRequest();
}


void CDIInputDeviceMonitor::ProcessRequest()
{
	tbb::mutex::scoped_lock(m_DeviceContainerMutex);

	size_t num_requests = m_queDIDeviceRequest.unsafe_size();

	CDIInputDeviceManagementRequest req;

	bool popped = m_queDIDeviceRequest.try_pop( req );

	switch( req.m_Type )
	{
	case CDIInputDeviceManagementRequest::CREATE_DEVICE:
	{
		CDIInputDeviceContainer container;
		container.m_DeviceInstance = req.m_DeviceInstance;

		bool created = CreateDevice( container );

		if( created )
		{
			// Commented out - 11:01 AM 9/5/2009
			// Duplicate requests may be added to queue if a device enumerated after the req popped
			// and before the device is created and stored in container
			// Moved to the beginning of the function
//			tbb::mutex::scoped_lock(m_DeviceContainerMutex);

//			container.SetAttached( true );
//			m_mapGUIDtoDIDeviceInstance[container.m_DeviceInstance.guidInstance] = container;
			m_vecDIDeviceInstanceContainer.push_back( container );

			OnInputDevicePlugged();
		}
		break;
	}

	case CDIInputDeviceManagementRequest::RELEASE_DEVICE:
		break;

	default:
		break;
	}
}


void CDIInputDeviceMonitor::AcquireInputDevices()
{
	tbb::mutex::scoped_lock( m_DeviceContainerMutex );

//	map<GUID,CDIInputDeviceContainer,less_for_guid>::iterator itr;

//	for( itr = m_mapGUIDtoDIDeviceInstance.begin();
//		 itr != m_mapGUIDtoDIDeviceInstance.end();
	for( vector<CDIInputDeviceContainer>::iterator itr = m_vecDIDeviceInstanceContainer.begin();
		 itr != m_vecDIDeviceInstanceContainer.end();
		 itr++ )
	{
/*		shared_ptr<CDirectInputGamepad> pGamepad = itr->second.m_pGamepad;

		if( pGamepad )
			pGamepad->Acquire();*/

		if( itr->m_pGamepad )
			itr->m_pGamepad->Acquire();
	}
}


} // namespace amorphous
