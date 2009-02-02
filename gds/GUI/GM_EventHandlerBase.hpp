#ifndef  __GM_EVENTHANDLERBASE_H__
#define  __GM_EVENTHANDLERBASE_H__


#include "GM_Event.hpp"

class GM_Event;

class CGM_EventHandlerBase
{
public:

	CGM_EventHandlerBase() {}
	virtual ~CGM_EventHandlerBase() {}

	virtual void HandleEvent( CGM_Event &event ) = 0;
};


/* >>> ================================= template =================================

---------------------------- header ----------------------------

#ifndef  __UserEventHandler_H__
#define  __UserEventHandler_H__

#include "GUI/GM_EventHandlerBase.hpp"

/
 *
 *
 /
class CUserEventHandler : public CGM_EventHandlerBase
{

public:
	CUserEventHandler();
	virtual ~CUserEventHandler();

	void HandleEvent( CGM_Event &event );
};

#endif  // __UserEventHandler_H__

// check the following template code for
// - implementation
// - registration to a dialog


---------------------------- implementation ----------------------------


#include "UserEventHandler.h"
#include "UI.hpp"


CUserEventHandler::CUserEventHandler() {}
CUserEventHandler::~CUserEventHandler() {}

void CUserEventHandler::HandleEvent( CGM_Event &event )
{

	if( !event.pControl )
		return;

	switch( event.pControl->GetID() )
	{
	case ID_BUTTON1:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
		{
		}
		break;
	case ID_BUTTON2:
		if( event.Type == CGM_Event::FOCUS_SHIFTED )
		{
		}
		break;
	}
}


---------------------------- registering to a dialog ----------------------------


void RegisterDialogEventHandler( CGM_Dialog *pDialog )
{
	CGM_DialogEventHandlerSharedPtr pEventHandler( new CUserEventHandler );
	pDialog->SetEventHandler( pEventHandler );
}


 <<< ================================= template ================================= */


#endif		/*  __GM_EVENTHANDLERBASE_H__  */
