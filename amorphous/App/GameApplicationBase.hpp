#ifndef  __GameApplicationBase_HPP__
#define  __GameApplicationBase_HPP__


#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Input/fwd.hpp"
#include "amorphous/Task/fwd.hpp"
#include "ApplicationBase.hpp"


#ifdef _DEBUG
#pragma comment ( lib, "GameStageFramework_d.lib" )
#else
#pragma comment ( lib, "GameStageFramework.lib" )
#endif


namespace amorphous
{

class GlobalInputHandler;
class GameApplicationBase;
class LogOutput_ScrolledTextBuffer;


// input devices
// accessed from window message procedure


// =============================== function externs ===============================
extern void MainLoop( ApplicationBase *pApp );

// =============================== variable externs ===============================
extern GameApplicationBase *g_pGameAppBase;

/**
 * base class for 3d game app
 *
 */
class GameApplicationBase : public ApplicationBase
{
	boost::shared_ptr<MouseInputDevice> m_pMouse;
	KeyboardInputDevice *m_pDIKeyboard;
	DirectInputGamepad *m_pDIGamepad;

protected:

	/// handles debug-related input
	/// - turning on / off logs on the screen
	/// - taking screen shot (not debug input)
	GlobalInputHandler *m_pGlobalInputHandler;

	GameTaskManager *m_pTaskManager;

	LogOutput_ScrolledTextBuffer *m_pOnScreenLog;

	static int ms_DefaultSleepTimeMS;

protected:

	// Virtual functions implemented by the developer

	virtual const std::string GetApplicationTitle() { return "Some Application"; }

	/// \brief Override this virtual function and return a unique ID, such as a GUID,
	/// to limit the application to one instance on OS.
	/// If this virtual function is not overriden, the system allows multiple instances
	/// of the application on OS.
	virtual const char *GetUniqueID() const { return ""; }

	/// Implement either GetStartTaskName() or GetStartTaskID() to tell
	/// the system which game task you want to use as a start-up task

	virtual const std::string GetStartTaskName() const { return ""; }

	virtual int GetStartTaskID() const;

	virtual GameTaskFactoryBase *CreateGameTaskFactory() const;

	/// Called at the end of InitBase()
	virtual bool Init() { return true; }

private:

	void Execute();

	void InitDebugItems();

	void ReleaseDebugItems();

	bool CreateDefaultMouseCursor( GraphicsElementManager& mgr );

	void UpdateFrame();

public:

	GameApplicationBase();
	virtual ~GameApplicationBase();

	bool InitBase();

	/// returns true if succeeds
	bool InitTaskManager();

	void Release();

	void AcquireInputDevices();

	GameTaskManager *GetTaskManager() { return m_pTaskManager; }

	static void SetDefaultSleepTime( int sleep_time_in_ms ) { ms_DefaultSleepTimeMS = sleep_time_in_ms; }

	void Run();

	friend void MainLoop( ApplicationBase *pApp );
};

} // namespace amorphous



#endif		/*  __GameApplicationBase_HPP__  */
