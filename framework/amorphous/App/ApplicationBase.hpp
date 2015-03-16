#ifndef  __ApplicationBase_HPP__
#define  __ApplicationBase_HPP__


#include <string>
#include <vector>
#include "amorphous/Input/fwd.hpp"


#ifdef _DEBUG
#pragma comment ( lib, "BasicFramework_d.lib" )
#else
#pragma comment ( lib, "BasicFramework.lib" )
#endif


namespace amorphous
{

class ApplicationBase;


// =============================== function externs ===============================
extern void MainLoop( ApplicationBase *pApp );
extern ApplicationBase *CreateApplicationInstance();

// =============================== variable externs ===============================
//extern ApplicationBase *g_pAppBase;

/**
 * base class for 3d game app
 *
 */
class ApplicationBase
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

	static ApplicationBase *ms_pInstance;

public:

	ApplicationBase()
		:
	m_UseDefaultMouse(true),
	m_UseDefaultKeyboard(true),
	m_IsAppExitRequested(false)
	{}

	virtual ~ApplicationBase() {}

//	virtual void Release() {}

	void RequestAppExit() { m_IsAppExitRequested = true; }

	bool IsAppExitRequested() const { return m_IsAppExitRequested; }

	/// Implemented by GameApplicationBase
	virtual void AcquireInputDevices() {}

//	static void SetDefaultSleepTime( int sleep_time_in_ms ) { ms_DefaultSleepTimeMS = sleep_time_in_ms; }

	virtual void Run() = 0;

	bool UseDefaultKeyboard() const { return m_UseDefaultKeyboard; }

	bool UseDefaultMouse() const { return m_UseDefaultMouse; }

	static void SetInstance( ApplicationBase *pApp ) { ms_pInstance = pApp; }

	static ApplicationBase *GetInstance() { return ms_pInstance; }

	static void ReleaseInstance()
	{
		if( ms_pInstance )
		{
			delete ms_pInstance;
			ms_pInstance = nullptr;
		}
	}

	static std::vector<std::string> ms_CommandLineArguments;

	friend void MainLoop( ApplicationBase *pApp );
	friend ApplicationBase *CreateApplicationInstance();
};

} // namespace amorphous



#endif		/*  __ApplicationBase_HPP__  */
