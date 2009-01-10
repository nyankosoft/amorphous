#ifndef  __GM_EVENT_H__
#define  __GM_EVENT_H__


#include <stdlib.h>
#include "fwd.h"


class CGM_SubEvent
{
public:

	enum Type
	{
		NONE,

		// sub events about closing dialog box
		DC_CANCELED,
		DC_LISTBOXITEM_SELECTED,
		DC_DLGCLOSEBUTTON_PRESSED,
		DC_DIALOG_SWITCHED,

		NUM_SUB_EVENTS
	};
};


class CGM_Event
{
public:

	enum EventType
	{
		INVALID = 0,
		BUTTON_CLICKED,
//		BUTTON_PRESSED,
//		BUTTON_RELEASED,
		COMBOBOX_SELECTION_CHANGED,
		RADIOBUTTON_CHECKED,
		RADIOBUTTON_CHECK_CLEARED,
		CHECKBOX_CHECKED,
		CHECKBOX_CHECK_CLEARED,
		SLIDER_VALUE_CHANGED,
		PAINTBAR_VALUE_CHANGED,
		FOCUS_SHIFTED,
		LISTBOX_SELECTION,		///< item selection in list box was changed
		LISTBOX_ITEM_SELECTED,	///< a focused item in list box was selected
//		FOCUS_CHANGED,
		DIALOG_CLOSED,          ///< 
//		ROOTDIALOG_CLOSED,
		OPENDIALOG_ATTEMPTED_TO_CLOSE, ///< dialog with CGM_Dialog::STYLE_ALWAYS_OPEN flags was attempted to close
//		DIALOG_OPENED,
		NUM_EVENTS
	};

public:

	EventType Type;

	CGM_SubEvent::Type SubType;

	CGM_ControlBase *pControl;	/* 070225 - dialog is set when a DIALOG_CLOSED event is reported */

public:

	CGM_Event() : Type(INVALID), SubType(CGM_SubEvent::NONE), pControl(NULL) {}

	CGM_Event( EventType event_type, CGM_ControlBase* _pControl )	/* 070225 */
		: Type(event_type), SubType(CGM_SubEvent::NONE), pControl(_pControl) {}

	~CGM_Event() {}
};


#endif		/*  __GM_EVENT_H__  */
