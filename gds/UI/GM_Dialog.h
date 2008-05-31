#ifndef  __GM_DIALOG_H__
#define  __GM_DIALOG_H__

#include "ui_fwd.h"
#include "GM_Event.h"
#include "GM_GraphicElement.h"
#include "GM_EventHandlerBase.h"

#include <vector>
#include <string>

#include "Graphics/Point.h"
using namespace Graphics;


//========================================================================================
// CGM_Dialog
//========================================================================================

class CGM_Dialog : public CGM_ControlBase
{
public:

	enum eStyleFlag
	{
		STYLE_ALWAYS_OPEN        = ( 1 << 0 ), ///< can only be used for root dialogs
//		STYLE_USE_MOUSE_INPUT    = ( 1 << 1 ),
//		STYLE_USE_KEYBOARD_INPUT = ( 1 << 2 ),
//		STYLE_4                  = ( 1 << 3 ),
	};

	enum eState
	{
		STATE_OPEN,
		STATE_CLOSED,
		NUM_STATES
	};
	
	enum eAdjacentDialog
	{
		NEXT_UP,
		NEXT_DOWN,
		NEXT_RIGHT,
		NEXT_LEFT,
		NUM_MAX_ADJACENT_DIALOGS
	};

	enum eParams
	{
		MAX_UI_LAYER_DEPTH = 80
	};

protected:

	CGM_DialogManager *m_pDialogManager;

	bool m_bIsOpen;

	/// owned reference of controls
	std::vector<CGM_Control *> m_vecpControl;

	bool m_bMinimized;
	bool m_bCaption;
//	std::string m_strCaption;
	int m_iCaptionHeight;

	std::string m_strTitle;

	bool m_bNonUserEvents;

	bool m_bRootDialog;

	/// per dialog event handler
//	CGM_EventHandlerBase *m_pEventHandler;
	CGM_DialogEventHandlerSharedPtr m_pEventHandler;

	/// callback function to handle events
	PCALLBACK_GM_GUIEVENT m_pEventHandlerFn;

    /// The control which is hovered over
	CGM_Control* m_pControlMouseOver;

	/// the control which had focus last time the dialog was closed
	/// or a child dialog is opened
	CGM_Control* m_pLastFocusedControl;

	/// used to notify the parent button that the dialog is closed.
	/// then, the parent button will tell its parent dialog to set the focus
	/// on the control which had been focused before this dialog is opened
	CGM_SubDialogButton* m_pOwnerButton;

	/// adjacent dailogs
	/// - dialogs do not have to be physically adjacent or aligned in particular way
	/// - borrowed reference
	/// - must be set by user through
	CGM_Dialog *m_apNextDialog[NUM_MAX_ADJACENT_DIALOGS];

	unsigned int m_StyleFlag;

	CGM_DialogSoundPlayerSharedPtr m_SoundPlayer;

	/// The control which has focus
//	static CGM_Control* ms_pControlFocus;

	/// The control currently pressed
//	static CGM_Control* ms_pControlPressed;

	CGM_DialogSoundPlayerSharedPtr m_pSoundPlayer;

protected:

	CGM_ControlRendererManager *GetRendererMgr();

	void RegisterControl( CGM_Control *pNewControl, CGM_ControlRendererSharedPtr pRenderer );

	/// Windows message handlers
	void OnMouseMove( SPoint& pt );
	void OnMouseUp( SPoint& pt );

    /// Control events
	void OnCycleFocus( bool bForward );
	void OnMouseEnter( CGM_Control* pControl );
	void OnMouseLeave( CGM_Control* pControl );

	CGM_Control *GetControlForFirstFocus();

	CGM_Control *GetControlForNextFocus( CGM_Control *pCurrentlyFocused, unsigned int dir );

	bool OnFocusShiftRequested( unsigned int direction );

	// --- NOT PROPERLY IMPLEMENTED ---
	bool OpenNextDialog( unsigned int direction );

public:

	CGM_Dialog(	CGM_DialogManager *pDialogManager, CGM_DialogDesc& desc );
	~CGM_Dialog();

	CGM_DialogManager *GetDialogManager() { return m_pDialogManager; }

	/// add a control to the dialog
	/// and returns a pointer to the created control
	CGM_Control *AddControl( CGM_ControlDesc *pControlDesc );

	CGM_Static *AddControl( CGM_StaticDesc *pStaticDesc );
	CGM_Button *AddControl( CGM_ButtonDesc *pButtonDesc );
	CGM_SubDialogButton *AddControl( CGM_SubDialogButtonDesc *pSubDlgDesc );
	CGM_CheckBox *AddControl( CGM_CheckBoxDesc *pCheckBoxDesc );
	CGM_RadioButton *AddControl( CGM_RadioButtonDesc *pRButtonDesc );
	CGM_DialogCloseButton *AddControl( CGM_DialogCloseButtonDesc *pCButtonDesc );
	CGM_Slider *AddControl( CGM_SliderDesc *pSliderDesc );
	CGM_ListBox *AddControl( CGM_ListBoxDesc *pListBoxDesc );
	CGM_ScrollBar *AddControl( CGM_ScrollBarDesc *pScrollBarDesc );

	CGM_Static *AddStatic( int id, const SRect& bound_rect, const std::string& title, CGM_ControlRendererSharedPtr pRenderer = CGM_ControlRendererSharedPtr() );
	CGM_Button *AddButton( int id, const SRect& bound_rect, const std::string& title, CGM_ControlRendererSharedPtr pRenderer = CGM_ControlRendererSharedPtr()  );
	CGM_CheckBox *AddCheckBox( int id, const SRect& bound_rect, const std::string& title, bool checked = false, CGM_ControlRendererSharedPtr pRenderer = CGM_ControlRendererSharedPtr() );
	CGM_RadioButton *AddRadioButton( int id, const SRect& bound_rect, int group, const std::string& title, CGM_ControlRendererSharedPtr pRenderer = CGM_ControlRendererSharedPtr() );
	CGM_SubDialogButton *AddSubDialogButton( int id, const SRect& bound_rect, const std::string& title, CGM_Dialog* pSubDialog = NULL, CGM_ControlRendererSharedPtr pRenderer = CGM_ControlRendererSharedPtr() );
	CGM_DialogCloseButton *AddDialogCloseButton( int id, const SRect& bound_rect, const std::string& title, CGM_ControlRendererSharedPtr pRenderer = CGM_ControlRendererSharedPtr() );
	CGM_ListBox *AddListBox( int id, const SRect& bound_rect, const std::string& title, int style_flag, int item_text_height, CGM_ControlRendererSharedPtr pRenderer = CGM_ControlRendererSharedPtr() );
	CGM_Slider *AddSlider( int id, const SRect& bound_rect, int min_val, int max_val, int init_val, CGM_ControlRendererSharedPtr pRenderer = CGM_ControlRendererSharedPtr() );

//	CGM_ScrollBar *AddScrollBar( int id, const SRect& bound_rect, CGM_ControlRendererSharedPtr pRenderer = CGM_ControlRendererSharedPtr() );

	CGM_Control* GetControl( int id );

	bool IsRoot() const { return m_bRootDialog; }

	bool IsOpen() const;
	void Open();

	/// close this dialog and all the subdialogs
	/// - Usually, dialogs are closed by other controls. There are 5 possibilities
	///   - A dialog close button is pressed.
	///   - Another sub-dialog button is pressed in the parent dialog.
	///   - Dialog received input code 'CGM_Input::CANCEL'.
	///   - Dialog received input code 'CGM_Input::NEXT_DIALOG_*', and there is a valid dialog
	///     in the specified direction.
	///   - A list box has style flag 'CLOSE_DIALOG_ON_ITEM_SELECTION' and its item is selected.
	void Close( unsigned int sub_event = CGM_SubEvent::NONE );

	/// close all the sub dialogs belonging to this dialog
	void CloseSubDialogs();

	bool HandleInput( CGM_InputData& input );

	void SendEvent( unsigned int event, bool bTriggeredByUser, CGM_ControlBase* pControl, unsigned int sub_event = CGM_SubEvent::NONE );

    void RequestFocus( CGM_Control* pControl );

	CGM_Control *GetControlAtPoint( SPoint& pt );


	void ClearRadioButtonGroup( int iButtonGroup );

	void ClearFocus();

    void ClearPressedControl();

//	static void InitStaticProperties();

	void Render();

	const std::string GetTitle() const { return m_strTitle; }

	void SetFocusOnLastFocusedControl();

	/// remember the control which currently owns focucs.
	/// called either when the dialog is closed or a child dialog is opened
	/// in the latter case, the remembered control is always of 'CGM_SubDialogButton'
	void UpdateLastFocusedControl();

	void SetOwnerButton( CGM_SubDialogButton *pOwnerButton ) { m_pOwnerButton = pOwnerButton; }

	/// sets adjacency between dialogs
	/// \param direction CGM_Dialog::NEXT_UP, NEXT_DOWN, NEXT_RIGHT, NEXT_LEFT
	void SetNextDialog( int direction, CGM_Dialog* pDialog );

	void SetNextDialogsUD( CGM_Dialog* pUp,   CGM_Dialog* pDown );
	void SetNextDialogsLR( CGM_Dialog* pLeft, CGM_Dialog* pRight );

//	void SetEventHandler( CGM_EventHandlerBase* pEventHandler ) { m_pEventHandler = pEventHandler; }
	void SetEventHandler( CGM_DialogEventHandlerSharedPtr pEventHandler ) { m_pEventHandler = pEventHandler; }

	void SetEventHandler( PCALLBACK_GM_GUIEVENT pEventHandler )	{ m_pEventHandlerFn = pEventHandler; }

	CGM_Control* GetControlByID( int id );

	CGM_Control* GetControlByStringID( const std::string& string_id );

	std::vector<CGM_Control *>& GetControls() { return m_vecpControl; }

	virtual void SetDepth( int depth );

	int GetMaxDepth() const;

	virtual void UpdateGraphicsProperties();

	virtual void ChangeScale( float factor );

	void SetSoundPlayer( CGM_DialogSoundPlayerSharedPtr pSoundPlayer ) { m_pSoundPlayer = pSoundPlayer; }

	virtual bool HasSoundPlayer() const { return ( m_pSoundPlayer.get() != NULL ); }

	int m_UserIndex;

	void *m_pUserData;
};


#endif		/*  __GM_DIALOG_H__  */
