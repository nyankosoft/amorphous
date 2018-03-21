#ifndef  __ApplicationBase_HPP__
#define  __ApplicationBase_HPP__


#include <string>
#include <vector>
#include "amorphous/Input/fwd.hpp"
#include "fwd.hpp"
#include "ApplicationCore.hpp"


#ifdef _DEBUG
#pragma comment ( lib, "BasicFramework_d.lib" )
#else
#pragma comment ( lib, "BasicFramework.lib" )
#endif


namespace amorphous
{

// =============================== function externs ===============================

// The users of the framework is required to implement this function in their source code.
extern ApplicationBase *CreateApplicationInstance();

/**
 * base class for 3d game app
 *
 * Users of this class need to do these:
 * - Define an application class derived from ApplicationBase.
 * - Define CreateApplicationInstance() which creates an instance
 *   of the defined application clcass
 * - Call ApplicationBase::StartApp() from their C++ source code,
 *   e.g. from main() function, to run the application
 * 
 * The ApplicationBase class has access to GameWindowManager and asks it to create a window for the application.
 * GameWindowManager creates a window based on the window system of the OS.
 * 
 * Requirements for derived (user-defined) classes (what they need to implement):
 * - Implement Run() to run the application. Note that GetGameWindowManager().MainLoop()
 *   needs to be called inside the implemented Run().
 */
class ApplicationBase : public ApplicationCore
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

	void OnKeyDown(unsigned int keycode);
	void OnKeyUp(unsigned int keycode);
	void OnButtonDown(unsigned int button);
	void OnButtonUp(unsigned int button);

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

	/**
	 * \brief Creates a used-defined application instance and enters the main loop.
	 */
	static void StartApp();

	static std::vector<std::string> ms_CommandLineArguments;

	friend ApplicationBase *CreateApplicationInstance();
};

} // namespace amorphous



#endif		/*  __ApplicationBase_HPP__  */
