
#include "GameTaskFactoryFG.h"

#include "GameTask_TitleFG.h"
#include "GameTask_AircraftSelect.h"
#include "GameTask_InStageMenuFG.h"
#include "GameTask_OnMissionFailedFG.h"
#include "GameTask_MainMenuFG.h"
#include "GameTask_BriefingFG.h"
#include "GameTask_DebriefingFG.h"
#include "GameTask_ShopFG.h"
#include "GameTask_ControlCustomizerFG.h"
#include "GameTask_SaveFG.h"
#include "GameTask_LoadFG.h"
#include "GameTask_StageFG.h"
#include "GameTask_AsyncStageLoaderFG.h"
//#include "GameTask_.h"
//#include "GameTask_.h"

#include "Support/SafeDelete.h"
#include "Support/msgbox.h"

#include <stdlib.h>


CGameTask *CGameTaskFactoryFG::CreateTask( int iTaskID )
{
	if( CGameTask* pTask = CGameTaskFactoryBase::CreateTask( iTaskID ) )
		return pTask;

	switch( iTaskID )
	{
	case CGameTaskFG::ID_TITLE_FG:
		return new CGameTask_TitleFG;

	case CGameTaskFG::ID_AIRCRAFT_SELECT:
		return new CGameTask_AircraftSelect;

	case CGameTaskFG::ID_STAGE_FG:
		return new CGameTask_StageFG;

	case CGameTaskFG::ID_INSTAGEMENU_FG:
		return new CGameTask_InStageMenuFG;

	case CGameTaskFG::ID_ON_MISSIONFAILED_FG:
		return new CGameTask_OnMissionFailedFG;

	case CGameTaskFG::ID_MAINMENU_FG:
		return new CGameTask_MainMenuFG;

	case CGameTaskFG::ID_BRIEFING_FG:
		return new CGameTask_BriefingFG;

	case CGameTaskFG::ID_DEBRIEFING_FG:
		return new CGameTask_DebriefingFG;

	case CGameTaskFG::ID_SHOP_FG:
		return new CGameTask_ShopFG;

	case CGameTaskFG::ID_CONTROLCUSTOMIZER_FG:
		return new CGameTask_ControlCustomizerFG;

	case CGameTaskFG::ID_SAVE_FG:
		return new CGameTask_SaveFG;

	case CGameTaskFG::ID_LOAD_FG:
		return new CGameTask_LoadFG;

	case CGameTaskFG::ID_ASYNCSTAGELOADER_FG:
		return new CGameTask_AsyncStageLoaderFG;

	default:
		MsgBoxFmt( "CGameTaskFactoryFG::CreateTask() - invalid task id: %d", iTaskID );
		return NULL;
	}
}


CGameTaskFactoryFG::CGameTaskFactoryFG()
{
}


CGameTaskFactoryFG::~CGameTaskFactoryFG()
{
}
