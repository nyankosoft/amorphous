
#include "GameTask_MainMenuFG.h"
#include "EventHandler_MainMenuFG.h"
#include "../FlightGameGlobalParams.h"
#include "UI/ui.h"

#include "Sound/GameSoundManager.h"

#include "Stage/PlayerInfo.h"
//#include "Stage/ScreenEffectManager.h"
#include "Stage/Stage.h"
#include "Support/memory_helpers.h"


/*
CMainMenuBaseEventHandler::CMainMenuBaseEventHandler()
{
	m_FocusShiftSound.SetSoundName( "focusshift" );
	m_ButtonPressSound.SetSoundName( "itemselect" );
}


CMainMenuBaseEventHandler::~CMainMenuBaseEventHandler()
{
}


void CMainMenuBaseEventHandler::HandleEvent( CGM_Event &event )
{
	if( event.Type == CGM_Event::FOCUS_SHIFTED )
	{
		GAMESOUNDMANAGER.Play( m_FocusShiftSound );
	}
	else if( event.Type == CGM_Event::BUTTON_CLICKED )
	{
		GAMESOUNDMANAGER.Play( m_ButtonPressSound );
	}
}*/



void CEventHandler_MainMenuFG::HandleEvent( CGM_Event &event )
{
	CEventHandler_Base::HandleEvent( event );

	if( !event.pControl )
		return;

	switch( event.pControl->GetID() )
	{
	case ID_TMM_DLG_ROOT:
		if( event.Type == CGM_Event::DIALOG_CLOSED )
			m_pTask->RequestTaskTransition( CGameTask::ID_PREVTASK );
		break;

	case ID_TMM_NEXT:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
		{
//			if( g_FlightGameParams.SkipBriefing )
//				m_pTask->RequestTaskTransition( CGameTaskFG::ID_AIRCRAFT_SELECT );
//			else
				m_pTask->RequestTaskTransition( CGameTaskFG::ID_BRIEFING_FG );
		}
		break;

	case ID_TMM_TITLE:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
			m_pTask->RequestTaskTransition( CGameTaskFG::ID_TITLE_FG );
		break;

	case ID_TMM_SHOP:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
			m_pTask->RequestTaskTransition( CGameTaskFG::ID_SHOP_FG );
		break;

	case ID_TMM_SAVE:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
			m_pTask->RequestTaskTransition( CGameTaskFG::ID_SAVE_FG );
		break;

/*	case ID_UI_LOAD:
		break;

	case ID_UI_EXIT:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
		{
			//========== destroy stage object before ending application ==========
			SafeDelete( g_pStage );
			//====================================================================

			PostMessage( GAMEWINDOWMANAGER.GetWindowHandle(), WM_QUIT, 0, 0 );
		}
		break;*/
	}
}
