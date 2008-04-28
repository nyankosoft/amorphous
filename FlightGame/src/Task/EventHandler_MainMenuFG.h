#ifndef  __EventHandler_MainMenuFG_H__
#define  __EventHandler_MainMenuFG_H__

#include "UI/GM_EventHandlerBase.h"
#include "Sound/SoundHandle.h"


/**
 * event handler for basic menu operations
 *
*/
class CEventHandler_Base : public CGM_EventHandlerBase
{
	CSoundHandle m_FocusShiftSound;
	CSoundHandle m_ButtonPressSound;

public:
	CEventHandler_Base() {}
	virtual ~CEventHandler_Base() {}

	void HandleEvent( CGM_Event &event ) {}
};


class CGameTask_MainMenuFG;

/**
 * event handler for main menu in FlightGame
 *
 */
class CEventHandler_MainMenuFG : public CEventHandler_Base
{
	/// task that processes events (borrowed reference)
	CGameTask_MainMenuFG *m_pTask;

public:

	CEventHandler_MainMenuFG( CGameTask_MainMenuFG *pTask ) : m_pTask(pTask) {}
	virtual ~CEventHandler_MainMenuFG() {}

	virtual void HandleEvent( CGM_Event &event );
};


#endif /* __EventHandler_MainMenuFG_H__ */
