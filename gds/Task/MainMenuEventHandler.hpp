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
class MainMenuBaseEventHandler : public CGM_EventHandlerBase
{
	SoundHandle m_FocusShiftSound;
	SoundHandle m_ButtonPressSound;

public:
	MainMenuBaseEventHandler();
	virtual ~MainMenuBaseEventHandler();
	void HandleEvent( CGM_Event &event );
};


/**
 * event handler for root menu
 *
 */
class MainMenuEventHandler : public MainMenuBaseEventHandler
{
public:
//	MainMenuEventHandler() {}
//	~MainMenuEventHandler() {}
	virtual void HandleEvent( CGM_Event &event );
};



/**
 * event handler for resolution settings
 *
 */
class CResolutionChangeEventHandler : public MainMenuBaseEventHandler
{
	int m_SelectedScreenWidth;
	int m_SelectedScreenHeight;
	bool m_bFulscreenModeSelected;

public:

	CResolutionChangeEventHandler();
	~CResolutionChangeEventHandler() {}
	virtual void HandleEvent( CGM_Event &event );
};


class CPhysSimVisEventHandler : public MainMenuBaseEventHandler
{
public:
	CPhysSimVisEventHandler();
	~CPhysSimVisEventHandler() {}
	virtual void HandleEvent( CGM_Event &event );
};

} // namespace amorphous



#endif /* __MAINMENUEVENTHANDLER_H__ */
