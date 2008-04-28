
#include "FG_CmdMenuEventHandler.h"
#include "Stage/BE_PlayerPseudoAircraft.h"
#include "UI/ui.h"



/*
class CEventHandler_PlayerCmdMenuFG : public CGM_EventHandlerBase
{
public:
	CEventHandler_PlayerCmdMenuFG() {}
	virtual ~CEventHandler_PlayerCmdMenuFG() {}

	void HandleEvent( CGM_Event &event );
};


void CEventHandler_PlayerCmdMenuFG::HandleEvent( CGM_Event &event )
{
}
*/
/*
class CPlayerCmdMenuStateHolder
{
	bool m_RootDialogClosed;

public:

	CPlayerCmdMenuStateHolder()
		:
	m_RootDialogClosed(true)
	{}
};
*/




//CFG_CmdMenuEventHandler::CFG_CmdMenuEventHandler() {}
CFG_CmdMenuEventHandler::~CFG_CmdMenuEventHandler() {}

void CFG_CmdMenuEventHandler::HandleEvent( CGM_Event &event )
{
	if( !event.pControl )
		return;

	switch( event.pControl->GetID() )
	{
	case UIID_CMD_RBT_VIS_NORMAL:
		if( event.Type == CGM_Event::RADIOBUTTON_CHECKED )
		{
			m_pPlayerAircraft->SetVisionMode( CPlayerVisionMode::Normal );
		}
		break;

	case UIID_CMD_RBT_VIS_NIGHTVISION:
		if( event.Type == CGM_Event::RADIOBUTTON_CHECKED )
		{
			m_pPlayerAircraft->SetVisionMode( CPlayerVisionMode::NightVision );
		}
		break;

	case UIID_CMD_RBT_VIS_THERMAL:
		if( event.Type == CGM_Event::RADIOBUTTON_CHECKED )
		{
			m_pPlayerAircraft->SetVisionMode( CPlayerVisionMode::Thermal );
		}
		break;

	default:
		break;
	}
}

