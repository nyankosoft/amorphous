#ifndef  __MAINMENUEVENTHANDLER_H__
#define  __MAINMENUEVENTHANDLER_H__

#include "GUI/GM_EventHandlerBase.hpp"
#include "Sound/SoundHandle.hpp"


namespace amorphous
{


/**
 * event handler for basic menu operations
 *
 */
class CMainMenuBaseEventHandler : public CGM_EventHandlerBase
{
	CSoundHandle m_FocusShiftSound;
	CSoundHandle m_ButtonPressSound;

public:
	CMainMenuBaseEventHandler();
	virtual ~CMainMenuBaseEventHandler();
	void HandleEvent( CGM_Event &event );
};


/**
 * event handler for root menu
 *
 */
class CMainMenuEventHandler : public CMainMenuBaseEventHandler
{
public:
//	CMainMenuEventHandler() {}
//	~CMainMenuEventHandler() {}
	virtual void HandleEvent( CGM_Event &event );
};



/**
 * event handler for resolution settings
 *
 */
class CResolutionChangeEventHandler : public CMainMenuBaseEventHandler
{
	int m_SelectedScreenWidth;
	int m_SelectedScreenHeight;
	bool m_bFulscreenModeSelected;

public:

	CResolutionChangeEventHandler();
	~CResolutionChangeEventHandler() {}
	virtual void HandleEvent( CGM_Event &event );
};


class CPhysSimVisEventHandler : public CMainMenuBaseEventHandler
{
public:
	CPhysSimVisEventHandler();
	~CPhysSimVisEventHandler() {}
	virtual void HandleEvent( CGM_Event &event );
};

} // namespace amorphous



#endif /* __MAINMENUEVENTHANDLER_H__ */
