#ifndef  __ApplicationBase_HPP__
#define  __ApplicationBase_HPP__


#include <string>
#include <vector>
#include "Input/fwd.hpp"

class CApplicationBase;


// =============================== function externs ===============================
extern void MainLoop( CApplicationBase *pApp );
extern CApplicationBase *CreateApplicationInstance();

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

	bool m_UseDefaultMouse;

	bool m_UseDefaultKeyboard;

	bool m_IsAppExitRequested;

protected:

	// Override this to customize the text that appears on the title bar of the applicaiton window.
	virtual const std::string GetApplicationTitle() { return "Some Application"; }

private:

	/// Called by the system in ::MainLoop()
	virtual void UpdateFrame() {}

	static CApplicationBase *ms_pInstance;

public:

	CApplicationBase()
		:
	m_UseDefaultMouse(true),
	m_UseDefaultKeyboard(true),
	m_IsAppExitRequested(false)
	{}

	virtual ~CApplicationBase() {}

//	virtual void Release() {}

	void RequestAppExit() { m_IsAppExitRequested = true; }

	bool IsAppExitRequested() const { return m_IsAppExitRequested; }

	/// Implemented by CGameApplicationBase
	virtual void AcquireInputDevices() {}

//	static void SetDefaultSleepTime( int sleep_time_in_ms ) { ms_DefaultSleepTimeMS = sleep_time_in_ms; }

	virtual void Run() = 0;

	bool UseDefaultKeyboard() const { return m_UseDefaultKeyboard; }

	bool UseDefaultMouse() const { return m_UseDefaultMouse; }

	static void SetInstance( CApplicationBase *pApp ) { ms_pInstance = pApp; }

	static CApplicationBase *GetInstance() { return ms_pInstance; }

	static void ReleaseInstance()
	{
		if( ms_pInstance )
		{
			delete ms_pInstance;
			ms_pInstance = NULL;
		}
	}

	static std::vector<std::string> ms_CommandLineArguments;

	friend void MainLoop( CApplicationBase *pApp );
	friend CApplicationBase *CreateApplicationInstance();
};


#endif		/*  __ApplicationBase_HPP__  */
