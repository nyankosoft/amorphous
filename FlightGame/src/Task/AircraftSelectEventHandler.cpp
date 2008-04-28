
#include "AircraftSelectEventHandler.h"
#include "GameTask_AircraftSelect.h"


void TAS_AircraftSelectListBoxEventHandler::OnItemSelected( CGM_ListBoxItem& item )
{
	m_pTask->SetNextSubMenu( CGameTask_AircraftSelect::SM_ITEMS_SELECT );
}


void TAS_AircraftSelectListBoxEventHandler::OnItemSelectionChanged( CGM_ListBoxItem& item )
{
	m_pTask->OnAircraftChanged( item.GetText() );
}


void TAS_AircraftSelectEventHandler::HandleEvent( CGM_Event& event )
{
	if( event.Type == CGM_Event::BUTTON_CLICKED )
	{
		int id = event.pControl->GetID();
		m_pTask->SetNextSubMenu( CGameTask_AircraftSelect::SM_ITEMS_SELECT );
	}
	else if( event.Type == CGM_Event::FOCUS_SHIFTED )
	{
		int id = event.pControl->GetID();
		m_pTask->OnAircraftChanged( id - CGameTask_AircraftSelect::AIRCRAFT_BUTTON_ID_OFFSET );
//		pCurrentAircraft = aircraft_list[id - AIRCRAFT_BUTTON_ID_OFFSET];
	}
	else if( event.Type == CGM_Event::DIALOG_CLOSED
	 && event.pControl->GetID() == ID_TAS_DLG_AIRCRAFTSELECT
	 && m_pTask->GetNextSubMenu() == CGameTask_AircraftSelect::SM_INVALID )
	{
//		m_pTask->RequestTaskTransition( CGameTask::ID_PREVTASK );
		m_pTask->RequestTaskTransition( CGameTaskFG::ID_MAINMENU_FG );
	}

}


void TAS_ItemsSelectEventHandler::HandleEvent( CGM_Event& event )
{
	if( !event.pControl )
		return;

	if( event.Type == CGM_Event::DIALOG_CLOSED
	 && event.pControl->GetID() == ID_TAS_DLG_AMMOSELECT
	 && m_pTask->GetNextSubMenu() == CGameTask_AircraftSelect::SM_INVALID )
	{
		// return to the aricraft select submenu
		m_pTask->SetNextSubMenu( CGameTask_AircraftSelect::SM_AIRCRAFT_SELECT );
		return;
	}
	else if( event.Type == CGM_Event::FOCUS_SHIFTED )
	{
		switch( event.pControl->GetID() )
		{
		case ID_TAS_GUN:	m_pTask->OnAmmoTypeFocusChanged( CGameTask_AircraftSelect::AMMO_BULLET );	break;
		case ID_TAS_MISSILE:m_pTask->OnAmmoTypeFocusChanged( CGameTask_AircraftSelect::AMMO_MISSILE );	break;
		case ID_TAS_SPW:	m_pTask->OnAmmoTypeFocusChanged( CGameTask_AircraftSelect::AMMO_SPW );		break;
		default:	break;
		}
	}

	else if( event.Type == CGM_Event::LISTBOX_SELECTION )
	{
		// update the ammo name on the sub-dialog button
		CGM_ListBox* listbox = (CGM_ListBox *)event.pControl;
		switch( event.pControl->GetID() )
		{
		case ID_TAS_LISTBOX_GUN:	m_pTask->OnAmmoFocusChanged( CGameTask_AircraftSelect::AMMO_BULLET );	break;
		case ID_TAS_LISTBOX_MISSILE:m_pTask->OnAmmoFocusChanged( CGameTask_AircraftSelect::AMMO_MISSILE );	break;
		case ID_TAS_LISTBOX_SPW:	m_pTask->OnAmmoFocusChanged( CGameTask_AircraftSelect::AMMO_SPW );		break;
		default:			break;
		}
	}

	switch( event.pControl->GetID() )
	{
	case ID_TAS_ITEMSELECT_OK:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
		{
			m_pTask->SetNextSubMenu( CGameTask_AircraftSelect::SM_CONFIRM );
		}
		break;

/*	case ID_TAS_CONFIRM_CANCEL:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
		{
			m_pTask->SetNextSubMenu( CGameTask_AircraftSelect::SM_AIRCRAFT_SELECT );
//			m_pTask->SetRequest( CGameTask_AircraftSelect::CONFIRM_CANCELED );
		}
		break;
*/
	default:
		break;
	}
}


void TAS_ConfirmEventHandler::HandleEvent( CGM_Event& event )
{
	if( !event.pControl )
		return;

	if( event.Type == CGM_Event::DIALOG_CLOSED
	 && event.pControl->GetID() == ID_TAS_DLG_ROOT_CONFIRM )
	{
		m_pTask->SetNextSubMenu( CGameTask_AircraftSelect::SM_ITEMS_SELECT );
		return;
	}

	switch( event.pControl->GetID() )
	{
	case ID_TAS_CONFIRM_OK:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
		{
			m_pTask->OnAircraftSelected();

			m_pTask->SetNextSubMenu( CGameTask_AircraftSelect::SM_INVALID );

			// request the next task
			m_pTask->RequestTransitionToNextTask();
		}
		break;

	case ID_TAS_CONFIRM_CANCEL:
		if( event.Type == CGM_Event::BUTTON_CLICKED )
		{
			m_pTask->SetNextSubMenu( CGameTask_AircraftSelect::SM_ITEMS_SELECT );
		}
		break;

	default:
		break;
	}
}


