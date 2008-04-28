#ifndef  __FG_CmdMenuEventHandler_H__
#define  __FG_CmdMenuEventHandler_H__

#include "UI/ui_fwd.h"
#include "UI/GM_EventHandlerBase.h"


enum eCommandMenuUI
{
	UIID_CMD_DLG_CMD_ROOT = 10000,
	UIID_CMD_DLG_WPN_ROOT,
	UIID_CMD_DLG_MSC_ROOT,
	UIID_CMD_DLG_VIS_ROOT,
	UIID_CMD_DLG_SMT_ROOT,
	UIID_CMD_RBT_VIS_NORMAL,
	UIID_CMD_RBT_VIS_NIGHTVISION,
	UIID_CMD_RBT_VIS_THERMAL,
	NUM_COMMAND_MENU_UIIDS
};


class CBE_PlayerPseudoAircraft;


/**
 * event handler for command menu UI
 * - turning on/off night vision
 * - changing sub-monitor mode
 * - etc.
 */
class CFG_CmdMenuEventHandler : public CGM_EventHandlerBase
{
	/// borrowed reference
	CBE_PlayerPseudoAircraft *m_pPlayerAircraft;

public:

//	CFG_CmdMenuEventHandler();
	CFG_CmdMenuEventHandler( CBE_PlayerPseudoAircraft *pPlayerAircraft ) : m_pPlayerAircraft(pPlayerAircraft) {}
	virtual ~CFG_CmdMenuEventHandler();

	void HandleEvent( CGM_Event &event );
};

#endif  // __FG_CmdMenuEventHandler_H__
