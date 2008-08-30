#ifndef  __APPLICATIONBASE_H__
#define  __APPLICATIONBASE_H__


#include <string>
#include "GameInput/fwd.h"

class CGlobalInputHandler;
class CGameTaskManager;
class CApplicationBase;
class CLogOutput_ScrolledTextBuffer;

// input devices
// accessed from window message procedure

extern CDirectInputMouse *g_pDIMouse;		// referenced from CGameTask classes
//extern CDIKeyboard *g_pDIKeyboard;
//extern CDirectInputGamepad *g_pDIGamepad;


extern CApplicationBase *g_pAppBase;

/**
 * base class for 3d game app
 *
 */
class CApplicationBase
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

private:

	void Execute();

	void InitDebugItems();

	void ReleaseDebugItems();

public:

	CApplicationBase();
	virtual ~CApplicationBase();

	virtual bool Init();

	/// returns true if succeeds
	virtual bool InitTaskManager() = 0;

	void Release();

	void AcquireInputDevices();

	CGameTaskManager *GetTaskManager() { return m_pTaskManager; }

	static void SetDefaultSleepTime( int sleep_time_in_ms ) { ms_DefaultSleepTimeMS = sleep_time_in_ms; }

	virtual const std::string GetApplicationTitle() { return "Some Application"; }

	void Run();
};


#endif		/*  __APPLICATIONBASE_H__  */
