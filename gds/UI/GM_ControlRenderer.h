#ifndef  __GM_CONTROLRENDERER_H__
#define  __GM_CONTROLRENDERER_H__



/**
  10:53 2007-12-19
    GM_Control::m_pRenderer holds a shared pointer of renderer
	- user does not have to release control renderers

	creating control renderers
	- define default control renderers
	- set special control renderers to each control
	  - supply shared ptr to CGM_Dialog::AddXXX() call
	    or CGM_ControlDescBase::pRenderer and call CGM_Dialog::AddControl()
	  - 

*/


#include "3DCommon/fwd.h"
#include "3DMath/Vector2.h"

#include "ui_fwd.h"
#include "GM_Static.h"
#include "GM_Button.h"
#include "GM_ListBox.h"
#include "GM_Slider.h"
#include "GM_ScrollBar.h"
#include "GM_SubDialogButton.h"
#include "GM_Dialog.h"

#include "Graphics/Point.h"
#include "Graphics/Rect.h"
using namespace Graphics;

#include <string>
#include <vector>



/**

  - support predifined animations / efffects for control renderers
    - needs feature to store animations / effects descs to each control renderers



*/


class CGM_ControlRenderer
{
public:

	enum eEffectTriggerEvent
	{
		ETE_PRESSED,
		ETE_RELEASED,
		ETE_CHECKED,
		ETE_CHECK_CLEARED,
		ETE_FOCUSED,
		ETE_FOCUS_CLEARED,
		ETE_MOUSECURSOR_ENTERED,
		ETE_MOUSECURSOR_LEFT,
		ETE_ITEMSELECTION_CHANGED,
		ETE_ITEM_SELECTED,
		ETE_SLIDERVALUE_CHANGED,
		ETE_DIALOG_CLOSED,
		ETE_DIALOG_OPENED,
		ETE_OPENDIALOG_ATTEMPTED_TO_CLOSE,
		ETE_PARENT_SUBDLGBTN_FOCUSED,
		ETE_PARENT_SUBDLGBTN_FOCUS_CLEARED,
		NUM_EFFECTTRIGGEREVENTS
	};

protected:

	CGraphicsElementManager *m_pGraphicsElementManager;

	CAnimatedGraphicsManagerBase *m_pGraphicsEffectManager;

//	CGM_DialogRenderer *m_pDialogRenderer;

	CGM_ControlRendererManager *m_pRendererManager; ///< borrowed reference

	/// Derived classes should store ids of all the graphics elements here,
	/// then the base class def of GetGraphicsElementIDs() will return all the
	/// element ids when requested by dialog renderer
	/// - If you group all the elements ids in your renderer class instance,
	///   only the id for that group needs to be placed here.
	///   12:29 2007-12-21 - changed: Need to be grouped into one group element
	///   01:55 2007-12-22 - reverted: Some controls may not want to group all their elements into one,
	///                      besides, vector style is easier since it allows the hierarchically defined renderer
	///                      classes to set elements incrementally in their hierarchy of Init() calls
	/// - borrowed reference
	std::vector<CGraphicsElement *> m_vecpElementToRegisterToParentDialog;
//	CGraphicsElement *m_pElementGroupToRegisterToParentDialog;

	std::vector<CGraphicsElement *> m_vecpGraphicsElementForPredefinedEffect;

	/// target control
	/// - borrowed reference
	CGM_ControlBase *m_pControl;

	/// used by the renderer of dialog
	CGE_Group *m_pGroupElement;

//	std::vector<CElementEffectDesc *> m_vecpEffectDesc[NUM_EFFECTTRIGGEREVENTS];

	class CLocalLayerInfo
	{
	public:
		int layer_index;
		CGraphicsElement *pElement;

		CLocalLayerInfo() : layer_index(0), pElement(NULL) {}
		CLocalLayerInfo( int index, CGraphicsElement *pElem) : layer_index(index), pElement(pElem) {}
	};

	std::vector<CLocalLayerInfo> m_vecLocalLayerInfo;

	/// owned reference of graphics elements added by AddGraphicsElement()
	std::vector<CGraphicsElement *> m_vecpExtraGraphicsElement;

protected:

	CGM_Control *GetControl()         { return dynamic_cast<CGM_Control *>(m_pControl);}
	CGM_Static *GetStatic()           { return dynamic_cast<CGM_Static *>(m_pControl);}
	CGM_Button *GetButton()           { return dynamic_cast<CGM_Button *>(m_pControl); }
	CGM_CheckBox *GetCheckBox()       { return dynamic_cast<CGM_CheckBox *>(m_pControl); }
	CGM_RadioButton *GetRadioButton() { return dynamic_cast<CGM_RadioButton *>(m_pControl); }
	CGM_ListBox *GetListBox()         { return dynamic_cast<CGM_ListBox *>(m_pControl); }
	CGM_ScrollBar *GetScrollBar()     { return dynamic_cast<CGM_ScrollBar *>(m_pControl); }
	CGM_Slider *GetSlider()           { return dynamic_cast<CGM_Slider *>(m_pControl); }
	CGM_SubDialogButton *GetSubDialogButton() { return dynamic_cast<CGM_SubDialogButton *>(m_pControl); }

	CGM_Dialog *GetDialog() { return dynamic_cast<CGM_Dialog *>(m_pControl); }

//	CGM_DialogCloseButton *Get() { return NULL; }

	/// register a graphics element to the owner dialog
	/// - 
	void RegisterGraphicsElementToParentDialog( CGraphicsElement *pElement ) { m_vecpElementToRegisterToParentDialog.push_back( pElement ); }

	/// sets the local rendering order (0: rendered on top).
	/// - Layer indices are not immediately calculated when this function is called.
	///   they are calculated at the first rendering call
	/// - Use this instead of RegisterGraphicsElement() if you do not want to register the element to owner dialog
	void SetLocalLayerOffset( int local_layer_index, CGraphicsElement *pElement );

	/// equivalent to calling the following 2 functions
	/// - RegisterGraphicsElementToParentDialog()
	/// - SetLocalLayerOffset()
	void RegisterGraphicsElement( int local_layer_index, CGraphicsElement *pElement );

	/// called when m_pGroupElement is created
	/// - used by the renderer of dialog to set all of its controls transparent
	///   at the start
	virtual void OnGroupElementCreated() {}

public:

	CGM_ControlRenderer();

	virtual ~CGM_ControlRenderer();

	virtual void Init() {}

	virtual void OnFocused() {}
	virtual void OnFocusCleared() {}

	/// for static control renderers
	/// - not called when the text is first set to a static control in its ctor
	virtual void OnTextChanged() {}

	/// for button renderers
	virtual void OnPressed() {}
	virtual void OnReleased() {}

	/// for check box and radio button renderers
	virtual void OnChecked() {}
	virtual void OnCheckCleared() {}

	virtual void OnMouseCursorEntered() {}
	virtual void OnMouseCursorLeft() {}

	/// for list box renderer
	/// - changes to text & desc are notified automatically
	/// - if you want to notify other changes to control renderer of listbox, call CGM_ListBoxItem::OnItemUpdated()
	virtual void OnItemSelectionFocusCreated() {} ///< new selection focus was created. done when an item is placed in a list box for the first time
	virtual void OnItemSelectionChanged() {} ///< selection focus was changed
	virtual void OnItemSelected() {}
	virtual void OnItemAdded( int index ) {}
	virtual void OnItemInserted( int index ) {}
	virtual void OnItemRemoved( int index ) {}
	virtual void OnItemTextChanged( CGM_ListBoxItem& item ) {} ///< called when a listbox item text was set through CGM_ListBoxItem::SetText()
	virtual void OnItemDescChanged( CGM_ListBoxItem& item ) {} ///< called when a listbox item desc was set through CGM_ListBoxItem::SetDesc()
	virtual void OnItemUpdated( CGM_ListBoxItem& item ) {} ///< called when a listbox item was modified


	/// for scroll bar renderer
	/// \param delta the number of items scrolled (pos: scrolled down / neg: scrolled up)
//	virtual void OnScrolled( CGM_ScrollBar *pScrollbar, int delta ) {}
//	virtual void OnTrackRangeChanged( CGM_ScrollBar *pScrollbar, int start, int end ) {}
	virtual void OnThumbUpdated( CGM_ScrollBar *pScrollbar ) {}

	/// for slider renderer
	virtual void OnSliderValueChanged() {}

	/// for dialog
	virtual void OnDialogOpened() {}
	virtual void OnDialogClosed() {}
	virtual void OnOpenDialogAttemptedToClose() {} ///< called when a dialog with CGM_Dialog::STYLE_ALWAYS_OPEN is attempted to close

	virtual void OnParentSubDialogButtonFocused() {}
	virtual void OnParentSubDialogButtonFocusCleared() {}

	/// called when the renderer is registered as a caption renderer
	virtual void OnFocusedControlChanged( CGM_Control *pNewFocusedControl, CGM_Control *pPrevFocusedControl ) {}
	virtual void OnMouseOverControlChanged( CGM_Control *pControlUnderMouse ) {}


	virtual void GetGraphicsElements( std::vector<CGraphicsElement *>& rvecpDestElement );

	CGM_ControlBase *GetBaseControl() { return m_pControl; }

	void UpdateGraphicsLayerSettings();

//	virtual void CreateGraphicsElementGroup();

	void SetControl( CGM_ControlBase *pControl ) { m_pControl = pControl; }

	void SetControlRendererManager( CGM_ControlRendererManager *pRenderMgr );

	/// mainly used by renderers for dialog
	/// - group the graphics elements of the controls belonging to the dialog
	/// - do sth like recrsive calls for controls on the dialog
	/// - but different from recursive call in that it only collects the controls on one dialog
	///   - does not recurse down through sub-dialog buttons
	///   - i.e. does not collect child dialogs of the dialog
	void GroupGraphicsElements();

	/// \param pEffectDescBase owned reference of an effect desc
//	void AddEffect( int effect_trigger_event, CElementEffectDesc *pEffectDescBase );

	/// \param pElement owned reference of a graphics element
	/// The added element is registered to owner dialog.
	/// - moved or faded together with the dialog
	void AddGraphicsElement( int local_layer_index, CGraphicsElement *pElement );

	virtual void ChangeScale( float scale );
};


//	CGM_Static *GetStatic() { if( m_pControl && m_pControl->GetType() == CGM_Control::STATIC ) return (CGM_Static *)m_pControl; else return NULL}


/*
  11:28 2007-12-18
  notes:
    - make CGM_Control::m_pRenderer a owned reference?
	- or use shared_ptr?
	- 2 ways to register control renderers
	  - use default renderers created in GM_ControlRendererManager::OnXXXCreate()
	  - set specific renderers to each control through CGM_Control::RegisterRenderer()




*/

#endif		/*  __GM_CONTROLRENDERER_H__  */
