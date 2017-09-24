#include "GameTask_MainMenu.hpp"
#include "amorphous/App/GameWindowManager.hpp"
#include "MainMenuEventHandler.hpp"
#include "amorphous/GUI.hpp"
#include "amorphous/Sound/SoundManager.hpp"
#include "GameTask_Stage.hpp"

#include "amorphous/Stage/PlayerInfo.hpp"
//#include "amorphous/Stage/ScreenEffectManager.hpp"
#include "amorphous/Stage/Stage.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;


MainMenuBaseEventHandler::MainMenuBaseEventHandler()
{
	m_FocusShiftSound.SetResourceName( "focusshift" );
	m_ButtonPressSound.SetResourceName( "itemselect" );
}


MainMenuBaseEventHandler::~MainMenuBaseEventHandler()
{
}


void MainMenuBaseEventHandler::HandleEvent( CGM_Event &event )
{
	if( event.Type == CGM_Event::FOCUS_SHIFTED )
	{
		GetSoundManager().Play( m_FocusShiftSound );
	}
	else if( event.Type == CGM_Event::BUTTON_CLICKED )
	{
		GetSoundManager().Play( m_ButtonPressSound );
	}
}


CPhysSimVisEventHandler::CPhysSimVisEventHandler()
{
}


void CPhysSimVisEventHandler::HandleEvent( CGM_Event &event )
{
	MainMenuBaseEventHandler::HandleEvent( event );

	if( !event.pControl )
		return;

//	CPhysicsManager_Visualizer *pVisualizer = g_pStage->GetPhysicsVisualizer();
/*	CJL_PhysicsVisualizer *pVisualizer = g_pStage->GetPhysicsVisualizer();
	CGM_ControlBase *pControl = event.pControl;

	switch( event.pControl->GetID() )
	{
	case ID_UI_PHYSSIM_CONTACT_POINTS:
		if( event.Type == CGM_Event::CHECKBOX_CHECKED )
            pVisualizer->RaiseRenderStateFlag( JL_VIS_CONTACT_POINTS );
		else if( event.Type == CGM_Event::CHECKBOX_CHECK_CLEARED )
			pVisualizer->ClearRenderStateFlag( JL_VIS_CONTACT_POINTS );
		break;

	case ID_UI_PHYSSIM_CONTACT_NORMALS:
		if( event.Type == CGM_Event::CHECKBOX_CHECKED )
			pVisualizer->RaiseRenderStateFlag( JL_VIS_CONTACT_NORMALS );
		else if( event.Type == CGM_Event::CHECKBOX_CHECK_CLEARED )
			pVisualizer->ClearRenderStateFlag( JL_VIS_CONTACT_NORMALS );
		break;

	case ID_UI_PHYSSIM_AABB:
		if( event.Type == CGM_Event::CHECKBOX_CHECKED )
			pVisualizer->RaiseRenderStateFlag( JL_VIS_AABB );
		else if( event.Type == CGM_Event::CHECKBOX_CHECK_CLEARED )
			pVisualizer->ClearRenderStateFlag( JL_VIS_AABB );
		break;

	case ID_UI_PHYSSIM_VELOCITY:
		if( event.Type == CGM_Event::CHECKBOX_CHECKED )
			pVisualizer->RaiseRenderStateFlag( JL_VIS_VELOCITY );
		else if( event.Type == CGM_Event::CHECKBOX_CHECK_CLEARED )
			pVisualizer->ClearRenderStateFlag( JL_VIS_VELOCITY );
		break;

	case ID_UI_PHYSSIM_ANGULAR_VELOCITY:
		if( event.Type == CGM_Event::CHECKBOX_CHECKED )
			pVisualizer->RaiseRenderStateFlag( ID_UI_PHYSSIM_ANGULAR_VELOCITY );
		else if( event.Type == CGM_Event::CHECKBOX_CHECK_CLEARED )
			pVisualizer->ClearRenderStateFlag( ID_UI_PHYSSIM_ANGULAR_VELOCITY );
		break;
	}*/
}


void MainMenuEventHandler::HandleEvent( CGM_Event &event )
{
	MainMenuBaseEventHandler::HandleEvent( event );

	if( !event.pControl )
		return;

	switch( event.pControl->GetID() )
	{
	case ID_UI_RESUME_GAME:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
//			GameTask_MainMenu::SetNextTaskID( GameTask::ID_STAGE );
			GameTask_MainMenu::SetNextTaskID( GameTask::ID_PREVTASK );
		break;
	case ID_UI_SAVE:
		break;
	case ID_UI_LOAD:
		break;

	case ID_UI_EXIT:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
		{
			//========== destroy stage object before ending application ==========
			g_pStage.reset();
			//====================================================================

			// release player's properties
			// - items, etc.
			SinglePlayerInfo().Release();

			LOG_PRINT_ERROR( " Exit command has not been implemented." );
//			RequestAppExit();
//			PostMessage( GameWindowManager_Win32().GetWindowHandle(), WM_QUIT, 0, 0 );
		}
		break;
	}
}



CResolutionChangeEventHandler::CResolutionChangeEventHandler()
{
	m_SelectedScreenWidth  = GetGameWindowManager().GetScreenWidth();
	m_SelectedScreenHeight = GetGameWindowManager().GetScreenHeight();
	m_bFulscreenModeSelected = false;
}


//void ResolutionChangeEventHandler( eGameMenuEventType event, int iControlID, CGM_Control* pControl )
void CResolutionChangeEventHandler::HandleEvent( CGM_Event &event )
{
	if( !event.pControl )
		return;

	CGM_ControlBase *pControl = event.pControl;
	int control_id = pControl->GetID();

//	if( event.pControl->GetType() == CGM_Control::RADIOBUTTON )
	if( ID_UI_RESOLUTION_LOWEST < control_id && control_id < ID_UI_RESOLUTION_HIGHEST )
	{
		// this is a resolution switch - get the requested resolution
		const string& text = ((CGM_Static *)(event.pControl))->GetText();
		sscanf( text.c_str(), "%d x %d", &m_SelectedScreenWidth, &m_SelectedScreenHeight );
	}

	switch( control_id )
	{
/*	case ID_UI_RESOLUTION_640X480:
		m_SelectedScreenWidth  = 640;	m_SelectedScreenHeight = 480;		break;
	case ID_UI_RESOLUTION_800X600:
		m_SelectedScreenWidth  = 800;	m_SelectedScreenHeight = 600;		break;
	case ID_UI_RESOLUTION_1024X768:
		m_SelectedScreenWidth  = 1024;	m_SelectedScreenHeight = 768;		break;
	case ID_UI_RESOLUTION_1280X960:
		m_SelectedScreenWidth  = 1280;	m_SelectedScreenHeight = 960;		break;*/
	case ID_UI_RESOLUTION_FULLSCREEN:
		m_bFulscreenModeSelected = ((CGM_CheckBox *)(pControl))->IsChecked() ? true : false;
		break;
	case ID_UI_RESOLUTION_APPLY:
		GetGameWindowManager().ChangeScreenSize( m_SelectedScreenWidth,
			                                m_SelectedScreenHeight,
											m_bFulscreenModeSelected );
		break;
	}
}



} // namespace amorphous
