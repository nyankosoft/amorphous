#ifndef  __GameApplicationBase_HPP__
#define  __GameApplicationBase_HPP__


#include "Input/fwd.hpp"
#include "Task/fwd.hpp"
#include "ApplicationBase.hpp"

class CGlobalInputHandler;
class CGameApplicationBase;
class CLogOutput_ScrolledTextBuffer;


// input devices
// accessed from window message procedure

extern CDirectInputMouse *g_pDIMouse;		// referenced from CGameTask classes
//extern CDIKeyboard *g_pDIKeyboard;
//extern CDirectInputGamepad *g_pDIGamepad;


// =============================== function externs ===============================
extern void MainLoop( CApplicationBase *pApp );

// =============================== variable externs ===============================
extern CGameApplicationBase *g_pGameAppBase;

/**
 * base class for 3d game app
 *
 */
class CGameApplicationBase : public CApplicationBase
{
	CDIKeyboard *m_pDIKeyboard;
	CDirectInputGamepad *m_pDIGamepad;

protected:

	/// handles debug-related input
	/// - turning on / off logs on the screen
	/// - taking screen shot (not debug input)
	CGlobalInputHandler *m_pGlobalInputHandler;

	CGameTaskManager *m_pTaskManager;

	CLogOutput_ScrolledTextBuffer *m_pOnScreenLog;

	static int ms_DefaultSleepTimeMS;

protected:

	// Virtual functions implemented by the developer

	virtual const std::string GetApplicationTitle() { return "Some Application"; }

	/// Implement either GetStartTaskName() or GetStartTaskID() to tell
	/// the system which game task you want to use as a start-up task

	virtual const std::string GetStartTaskName() const { return ""; }

	virtual int GetStartTaskID() const;

	virtual CGameTaskFactoryBase *CreateGameTaskFactory() const;

	/// Called at the end of InitBase()
	virtual bool Init() { return true; }

private:

	void Execute();

	void InitDebugItems();

	void ReleaseDebugItems();

	void UpdateFrame();

public:

	CGameApplicationBase();
	virtual ~CGameApplicationBase();

	bool InitBase();

	/// returns true if succeeds
	bool InitTaskManager();

	void Release();

	void AcquireInputDevices();

	CGameTaskManager *GetTaskManager() { return m_pTaskManager; }

	static void SetDefaultSleepTime( int sleep_time_in_ms ) { ms_DefaultSleepTimeMS = sleep_time_in_ms; }

	void Run();

	friend void MainLoop( CApplicationBase *pApp );
};


#endif		/*  __GameApplicationBase_HPP__  */
