#ifndef __AircraftSelectEventHandler_H__
#define __AircraftSelectEventHandler_H__


#include "UI/ui.h"

class CGameTask_AircraftSelect;


class TAS_AircraftSelectListBoxEventHandler : public CGM_ListBoxEventHandler
{
	CGameTask_AircraftSelect *m_pTask;

public:
	TAS_AircraftSelectListBoxEventHandler( CGameTask_AircraftSelect *pTask ) : m_pTask(pTask) {}
	virtual ~TAS_AircraftSelectListBoxEventHandler() {}

	virtual void OnItemSelected( CGM_ListBoxItem& item );
	virtual void OnItemSelectionChanged( CGM_ListBoxItem& item );
};


class TAS_AircraftSelectEventHandler : public CGM_EventHandlerBase
{
	CGameTask_AircraftSelect *m_pTask;

public:
	TAS_AircraftSelectEventHandler( CGameTask_AircraftSelect *pTask ) : m_pTask(pTask) {}
	virtual ~TAS_AircraftSelectEventHandler() {}

	void HandleEvent( CGM_Event &event );
};


class TAS_ItemsSelectEventHandler : public CGM_EventHandlerBase
{
	CGameTask_AircraftSelect *m_pTask;

public:
	TAS_ItemsSelectEventHandler( CGameTask_AircraftSelect *pTask ) : m_pTask(pTask) {}
	virtual ~TAS_ItemsSelectEventHandler() {}

	void HandleEvent( CGM_Event &event );
};


class TAS_ConfirmEventHandler : public CGM_EventHandlerBase
{
	CGameTask_AircraftSelect *m_pTask;

public:
	TAS_ConfirmEventHandler( CGameTask_AircraftSelect *pTask ) : m_pTask(pTask) {}
	virtual ~TAS_ConfirmEventHandler() {}

	void HandleEvent( CGM_Event &event );
};


#endif  /*  __AircraftSelectEventHandler_H__  */
