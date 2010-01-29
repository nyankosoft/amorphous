#ifndef  __ApplicationBase_HPP__
#define  __ApplicationBase_HPP__


#include <string>
#include "Input/fwd.hpp"

class CApplicationBase;


// =============================== function externs ===============================
extern void MainLoop( CApplicationBase *pApp );

// =============================== variable externs ===============================
//extern CApplicationBase *g_pAppBase;

/**
 * base class for 3d game app
 *
 */
class CApplicationBase
{
protected:

//	static int ms_DefaultSleepTimeMS;

protected:

	// Override this to customize the text that appears on the title bar of the applicaiton window.
	virtual const std::string GetApplicationTitle() { return "Some Application"; }

private:

	/// Called by the system in ::MainLoop()
	virtual void UpdateFrame() {}

public:

	CApplicationBase() {}

	virtual ~CApplicationBase() {}

//	virtual void Release() {}

	/// Implemented by CGameApplicationBase
	virtual void AcquireInputDevices() {}

//	static void SetDefaultSleepTime( int sleep_time_in_ms ) { ms_DefaultSleepTimeMS = sleep_time_in_ms; }

	virtual void Run() = 0;

	friend void MainLoop( CApplicationBase *pApp );
};


#endif		/*  __ApplicationBase_HPP__  */
