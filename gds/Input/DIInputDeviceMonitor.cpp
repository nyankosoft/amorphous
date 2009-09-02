#include "DIInputDeviceMonitor.hpp"
#include "Input/DirectInputGamepad.hpp"
#include "Support/Log/DefaultLog.hpp"
#include <boost/thread/xtime.hpp>

using namespace std;
using namespace boost;


static LPDIRECTINPUTDEVICE8 g_pDITempJoystickDevice = NULL;


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
	map<GUID,CDIInputDeviceContainer,less_for_guid>::iterator itr;

	for( itr = m_mapGUIDtoDIDeviceInstance.begin();
		 itr != m_mapGUIDtoDIDeviceInstance.end();
		 itr++ )
	{
		itr->second.SetAttached( false );
	}
}


bool CDIInputDeviceMonitor::CreateDevice( CDIInputDeviceContainer& container )
{
	container.m_pGamepad
		= shared_ptr<CDirectInputGamepad>( new CDirectInputGamepad() );

	Result::Name res = container.m_pGamepad->InitDevice( container.m_DeviceInstance );

	if( res != Result::SUCCESS )//|| !container.m_pGamepad->IsValid() )
	{
		string log_text = fmt_string( "Failed to initialize the input device (instance: %s, product: %s).",
			container.m_DeviceInstance.tszInstanceName,
			container.m_DeviceInstance.tszProductName );

		LOG_PRINT_WARNING( log_text );
		container.m_pGamepad.reset();
		return false;
	}
	else
		return true;
}


// Enumerate all the input device
// - Try to create input device objects for all the attached device
void CDIInputDeviceMonitor::CheckDevices()
{
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

		map<GUID,CDIInputDeviceContainer,less_for_guid>::iterator itr = m_mapGUIDtoDIDeviceInstance.find( di.guidInstance );

		if( itr == m_mapGUIDtoDIDeviceInstance.end() )
		{
			// A new game controller has been enumerated.

			CDIInputDeviceContainer container;
			container.m_DeviceInstance = di;

			bool created = CreateDevice( container );

			if( created )
			{
				container.SetAttached( true );
				m_mapGUIDtoDIDeviceInstance[di.guidInstance] = container;
			}
		}
		else
		{
			// Mark this device as attached
			itr->second.SetAttached( true );
		}
	}

	m_vecDIDeviceInstanceHolder.resize( 0 );
}


void CDIInputDeviceMonitor::run()
{
	boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC);
	xt.sec += 1; // 1 [sec]

	while( !m_ExitThread )
	{
		CheckDevices();

		boost::thread::sleep(xt);
	}
}


void CDIInputDeviceMonitor::AcquireInputDevices()
{
	map<GUID,CDIInputDeviceContainer,less_for_guid>::iterator itr;

	for( itr = m_mapGUIDtoDIDeviceInstance.begin();
		 itr != m_mapGUIDtoDIDeviceInstance.end();
		 itr++ )
	{
		shared_ptr<CDirectInputGamepad> pGamepad = itr->second.m_pGamepad;

		if( pGamepad )
			pGamepad->Acquire();
	}
}
