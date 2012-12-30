#ifndef  __GM_SoundPlayer_H__
#define  __GM_SoundPlayer_H__


#include "fwd.hpp"
#include "GM_Event.hpp"
#include "GM_EventHandlerBase.hpp"
#include "GM_ListBox.hpp"
#include "GM_Button.hpp"
#include "Sound/SoundHandle.hpp"


namespace amorphous
{


// >>>==================================== template ====================================

/*
void SetDefaultSound()
{
	CGM_DialogManagerSharedPtr pDialogManager;

	CGM_GlobalSoundPlayerSharedPtr pSoundPlayer = pDialogManager->GetDefaultSoundPlayer();

	pSoundPlayer->SetSoundOnFocusShifted(                       "ui_focus_shifted.wav" );
	pSoundPlayer->SetSoundOnDialogClosedByCancelInput(          "ui_dlg_canceled.wav" );
	pSoundPlayer->SetSoundOnDialogClosedByListBoxItemSelection( "ui_button_pressed.wav" );
//	pSoundPlayer->SetSoundOnDialogClosedByDialogSwitching(      "ui_.wav" );
//	pSoundPlayer->SetSoundOnDialogClosedByDialogCloseButton(    "ui_.wav" );
//	pSoundPlayer->SetSoundOnDialogClosed(                       "ui_dlg_closed.wav" );
	pSoundPlayer->SetSoundOnDialogAttemptedToClose(             "ui_dlg_cannot_close.wav" );
	pSoundPlayer->SetSoundOnButtonPressed(                      "ui_button_pressed.wav" );
//	pSoundPlayer->SetSoundOnButtonReleased(                     "ui_button_released.wav" );
	pSoundPlayer->SetSoundOnCheckBoxChecked(                    "ui_checked.wav" );
	pSoundPlayer->SetSoundOnCheckBoxCheckCleared(               "ui_unchecked.wav" );
	pSoundPlayer->SetSoundOnRadioButtonChecked(                 "ui_checked.wav" );
	pSoundPlayer->SetSoundOnRadioButtonCheckCleared(            "ui_unchecked.wav" );
//	pSoundPlayer->SetSoundOnDialogCloseButtonPressed(           ".wav" );
//	pSoundPlayer->SetSoundOnDialogCloseButtonReleased(          ".wav" );
	pSoundPlayer->SetSoundOnItemFocusShifted(                   "ui_focus_shifted.wav" );
	pSoundPlayer->SetSoundOnItemSelected(                       "ui_button_pressed.wav" );


	// pSoundPlayer->SetSoundOnDialogClosed()
	// - played whenever a dialog is closed, including when
	//   - a dialog is closed by dialog close button
	//   - An item is selected in list box with style 'CLOSE_DIALOG_ON_ITEM_SELECTION'
}
*/

// <<<==================================== template ====================================




class CGM_DialogSoundPlayer : public CGM_EventHandlerBase
{
//	CSoundHandle m_aSoundHandle[CGM_Event::NUM_EVENTS];

	CSoundHandle m_SoundOnControlFocusShifted;
	CSoundHandle m_SoundOnDialogClosedByCancelInput;
	CSoundHandle m_SoundOnDialogClosedByListBoxItemSelection;
	CSoundHandle m_SoundOnDialogClosedByDialogSwitching;
	CSoundHandle m_SoundOnDialogClosedByDialogCloseButton;
	CSoundHandle m_SoundOnOpenDialogAttemptedToClose;



public:

	CGM_DialogSoundPlayer() {}
	virtual ~CGM_DialogSoundPlayer() {}

	virtual void HandleEvent( CGM_Event &event );

	void SetSoundOnFocusShifted( const std::string& resource_name )                       { m_SoundOnControlFocusShifted.SetResourceName( resource_name ); }
//	void SetSoundOnDialogClosed( const std::string& resource_name )                       { .SetResourceName( resource_name ); }
	void SetSoundOnDialogClosedByCancelInput( const std::string& resource_name )          { m_SoundOnDialogClosedByCancelInput.SetResourceName( resource_name ); }
	void SetSoundOnDialogClosedByListBoxItemSelection( const std::string& resource_name ) { m_SoundOnDialogClosedByListBoxItemSelection.SetResourceName( resource_name ); }
	void SetSoundOnDialogClosedByDialogSwitching( const std::string& resource_name )      { m_SoundOnDialogClosedByDialogSwitching.SetResourceName( resource_name ); }
	void SetSoundOnDialogClosedByDialogCloseButton( const std::string& resource_name )    { m_SoundOnDialogClosedByDialogCloseButton.SetResourceName( resource_name ); }
	void SetSoundOnDialogAttemptedToClose( const std::string& resource_name )             { m_SoundOnOpenDialogAttemptedToClose.SetResourceName( resource_name ); }
};

class CGM_ButtonSoundPlayer : public CGM_ButtonEventHandler
{
//	CSoundHandle m_aSoundHandle[CGM_Event::NUM_EVENTS];

	CSoundHandle m_SoundOnButtonPressed;
	CSoundHandle m_SoundOnButtonReleased;

	CSoundHandle m_SoundOnChecked;
	CSoundHandle m_SoundOnCheckCleared;

	CSoundHandle m_SoundOnRadioButtonChecked;
	CSoundHandle m_SoundOnRadioButtonCheckCleared;

public:

	CGM_ButtonSoundPlayer() {}
	virtual ~CGM_ButtonSoundPlayer() {}

	virtual void OnPressed();
	virtual void OnReleased();

	void SetSoundOnButtonPressed( const std::string& resource_name )            { m_SoundOnButtonPressed.SetResourceName( resource_name ); }
	void SetSoundOnButtonReleased( const std::string& resource_name )           { m_SoundOnButtonReleased.SetResourceName( resource_name ); }
	void SetSoundOnChecked( const std::string& resource_name )                  { m_SoundOnChecked.SetResourceName( resource_name ); }
	void SetSoundOnCheckCleared( const std::string& resource_name )             { m_SoundOnCheckCleared.SetResourceName( resource_name ); }
	void SetSoundOnRadioButtonChecked( const std::string& resource_name )       { m_SoundOnRadioButtonChecked.SetResourceName( resource_name ); }
	void SetSoundOnRadioButtonCheckCleared( const std::string& resource_name )  { m_SoundOnRadioButtonCheckCleared.SetResourceName( resource_name ); }
};


class CGM_ListBoxSoundPlayer : public CGM_ListBoxEventHandler
{
//	CSoundHandle m_aSoundHandle[CGM_Event::NUM_EVENTS];

	CSoundHandle m_SoundOnItemSelected;
	CSoundHandle m_SoundOnItemFocusShifted;

public:

	CGM_ListBoxSoundPlayer() {}
	virtual ~CGM_ListBoxSoundPlayer() {}

	virtual void OnItemSelected( CGM_ListBoxItem& item, int item_index );
	virtual void OnItemSelectionChanged( CGM_ListBoxItem& item, int item_index );

	void SetSoundOnItemFocusShifted( const std::string& resource_name )         { m_SoundOnItemFocusShifted.SetResourceName( resource_name ); }
	void SetSoundOnItemSelected( const std::string& resource_name )             { m_SoundOnItemSelected.SetResourceName( resource_name ); }
};


class CGM_GlobalSoundPlayer
{
	/// sound player for dialogs
	CGM_DialogSoundPlayerSharedPtr m_pDialogSoundPlayer;

	/// sound player for buttons, check boxes and radio buttons
	CGM_ButtonSoundPlayerSharedPtr m_pButtonSoundPlayer;

	/// sound player for buttons, check boxes and radio buttons
	CGM_ButtonSoundPlayerSharedPtr m_pCheckBoxSoundPlayer;

	/// sound player for buttons, check boxes and radio buttons
	CGM_ButtonSoundPlayerSharedPtr m_pRadioButtonSoundPlayer;

	/// sound player for dialog close buttons
	CGM_ButtonSoundPlayerSharedPtr m_pSubDialogButtonSoundPlayer;

	/// sound player for dialog close buttons
	CGM_ButtonSoundPlayerSharedPtr m_pDialogCloseButtonSoundPlayer;

	/// sound player for list boxes
	CGM_ListBoxSoundPlayerSharedPtr m_pListBoxSoundPlayer;

public:

	CGM_GlobalSoundPlayer();
	virtual ~CGM_GlobalSoundPlayer() {}

	CGM_DialogSoundPlayerSharedPtr GetDialogSoundPlayer()            { return m_pDialogSoundPlayer; }
	CGM_ButtonSoundPlayerSharedPtr GetButtonSoundPlayer()            { return m_pButtonSoundPlayer; }
	CGM_ButtonSoundPlayerSharedPtr GetCheckBoxSoundPlayer()          { return m_pCheckBoxSoundPlayer; }
	CGM_ButtonSoundPlayerSharedPtr GetRadioBttonSoundPlayer()        { return m_pRadioButtonSoundPlayer; }
	CGM_ButtonSoundPlayerSharedPtr GetSubDialogButtonSoundPlayer()   { return m_pSubDialogButtonSoundPlayer; }
	CGM_ButtonSoundPlayerSharedPtr GetDialogCloseButtonSoundPlayer() { return m_pDialogCloseButtonSoundPlayer; }
	CGM_ListBoxSoundPlayerSharedPtr GetListBoxSoundPlayer()          { return m_pListBoxSoundPlayer; }

//	virtual void HandleEvent( CGM_Event &event );
//	void SetSoundOnFocusShifted( const std::string& resource_name )             { m_aSoundHandle[CGM_Event::FOCUS_SHIFTED].SetResourceName( resource_name ); }

	void SetSoundOnFocusShifted( const std::string& resource_name )                       { m_pDialogSoundPlayer->SetSoundOnFocusShifted( resource_name ); }
//	void SetSoundOnDialogClosed( const std::string& resource_name )                       { m_pDialogSoundPlayer->SetSoundOnDialogClosed( resource_name ); }
	void SetSoundOnDialogClosedByCancelInput( const std::string& resource_name )          { m_pDialogSoundPlayer->SetSoundOnDialogClosedByCancelInput( resource_name ); }
	void SetSoundOnDialogClosedByListBoxItemSelection( const std::string& resource_name ) { m_pDialogSoundPlayer->SetSoundOnDialogClosedByListBoxItemSelection( resource_name ); }
	void SetSoundOnDialogClosedByDialogSwitching( const std::string& resource_name )      { m_pDialogSoundPlayer->SetSoundOnDialogClosedByDialogSwitching( resource_name ); }
	void SetSoundOnDialogClosedByDialogCloseButton( const std::string& resource_name )    { m_pDialogSoundPlayer->SetSoundOnDialogClosedByDialogCloseButton( resource_name ); }
	void SetSoundOnDialogAttemptedToClose( const std::string& resource_name )             { m_pDialogSoundPlayer->SetSoundOnDialogAttemptedToClose( resource_name ); }

	void SetSoundOnButtonPressed( const std::string& resource_name )             { m_pButtonSoundPlayer->SetSoundOnButtonPressed( resource_name ); }
	void SetSoundOnButtonReleased( const std::string& resource_name )            { m_pButtonSoundPlayer->SetSoundOnButtonReleased( resource_name ); }
	void SetSoundOnCheckBoxChecked( const std::string& resource_name )           { m_pCheckBoxSoundPlayer->SetSoundOnChecked( resource_name ); }
	void SetSoundOnCheckBoxOnCheckCleared( const std::string& resource_name )    { m_pCheckBoxSoundPlayer->SetSoundOnCheckCleared( resource_name ); }
	void SetSoundOnRadioButtonChecked( const std::string& resource_name )        { m_pRadioButtonSoundPlayer->SetSoundOnRadioButtonChecked( resource_name ); }
	void SetSoundOnRadioButtonCheckCleared( const std::string& resource_name )   { m_pRadioButtonSoundPlayer->SetSoundOnRadioButtonCheckCleared( resource_name ); }
	void SetSoundOnSubDialogButtonPressed( const std::string& resource_name )    { m_pSubDialogButtonSoundPlayer->SetSoundOnButtonPressed( resource_name ); }
	void SetSoundOnSubDialogButtonReleased( const std::string& resource_name )   { m_pSubDialogButtonSoundPlayer->SetSoundOnButtonReleased( resource_name ); }
	void SetSoundOnDialogCloseButtonPressed( const std::string& resource_name )  { m_pDialogCloseButtonSoundPlayer->SetSoundOnButtonPressed( resource_name ); }
	void SetSoundOnDialogCloseButtonReleased( const std::string& resource_name ) { m_pDialogCloseButtonSoundPlayer->SetSoundOnButtonReleased( resource_name ); }

	void SetSoundOnItemFocusShifted( const std::string& resource_name )          { m_pListBoxSoundPlayer->SetSoundOnItemFocusShifted( resource_name ); }
	void SetSoundOnItemSelected( const std::string& resource_name )              { m_pListBoxSoundPlayer->SetSoundOnItemSelected( resource_name ); }

};

} // namespace amorphous



#endif		/*  __GM_SoundPlayer_H__  */

