
#include "GM_SoundPlayer.h"
#include "Sound/SoundManager.h"


//=================================================================================
// CGM_DialogSoundPlayer
//=================================================================================

void CGM_DialogSoundPlayer::HandleEvent( CGM_Event &event )
{
	switch( event.Type )
	{
	case CGM_Event::FOCUS_SHIFTED:
		if( 0 < m_SoundOnControlFocusShifted.GetResourceName().length() )
			SoundManager().Play( m_SoundOnControlFocusShifted );
		break;

	case CGM_Event::DIALOG_CLOSED:
		switch( event.SubType )
		{
		case CGM_SubEvent::DC_CANCELED:
			if( 0 < m_SoundOnDialogClosedByCancelInput.GetResourceName().length() )
				SoundManager().Play( m_SoundOnDialogClosedByCancelInput );
			break;

		case CGM_SubEvent::DC_LISTBOXITEM_SELECTED:
			if( 0 < m_SoundOnDialogClosedByListBoxItemSelection.GetResourceName().length() )
				SoundManager().Play( m_SoundOnDialogClosedByListBoxItemSelection );
			break;

		case CGM_SubEvent::DC_DIALOG_SWITCHED:
			if( 0 < m_SoundOnDialogClosedByDialogSwitching.GetResourceName().length() )
				SoundManager().Play( m_SoundOnDialogClosedByDialogSwitching );
			break;

		case CGM_SubEvent::DC_DLGCLOSEBUTTON_PRESSED:
			if( 0 < m_SoundOnDialogClosedByDialogCloseButton.GetResourceName().length() )
				SoundManager().Play( m_SoundOnDialogClosedByDialogCloseButton );
			break;

		default:
			break;
		}
		break;

	case CGM_Event::OPENDIALOG_ATTEMPTED_TO_CLOSE:
		if( 0 < m_SoundOnOpenDialogAttemptedToClose.GetResourceName().length() )
			SoundManager().Play( m_SoundOnOpenDialogAttemptedToClose );
		break;
	default:
		break;
	}
}


//=================================================================================
// CGM_ButtonSoundPlayer
//=================================================================================

void CGM_ButtonSoundPlayer::OnPressed()
{
	if( 0 < m_SoundOnButtonPressed.GetResourceName().length() )
		SoundManager().Play( m_SoundOnButtonPressed );
}


void CGM_ButtonSoundPlayer::OnReleased()
{
	if( 0 < m_SoundOnButtonReleased.GetResourceName().length() )
		SoundManager().Play( m_SoundOnButtonReleased );
}


//=================================================================================
// CGM_ListBoxSoundPlayer
//=================================================================================

void CGM_ListBoxSoundPlayer::OnItemSelected( CGM_ListBoxItem& item, int item_index )
{
	if( 0 < m_SoundOnItemSelected.GetResourceName().length() )
		SoundManager().Play( m_SoundOnItemSelected );
}


void CGM_ListBoxSoundPlayer::OnItemSelectionChanged( CGM_ListBoxItem& item, int item_index )
{
	if( 0 < m_SoundOnItemFocusShifted.GetResourceName().length() )
		SoundManager().Play( m_SoundOnItemFocusShifted );
}



//=================================================================================
// CGM_GlobalSoundPlayer
//=================================================================================

CGM_GlobalSoundPlayer::CGM_GlobalSoundPlayer()
{
	m_pDialogSoundPlayer            = CGM_DialogSoundPlayerSharedPtr( new CGM_DialogSoundPlayer() );
	m_pButtonSoundPlayer            = CGM_ButtonSoundPlayerSharedPtr( new CGM_ButtonSoundPlayer() );
	m_pCheckBoxSoundPlayer          = CGM_ButtonSoundPlayerSharedPtr( new CGM_ButtonSoundPlayer() );
	m_pRadioButtonSoundPlayer       = CGM_ButtonSoundPlayerSharedPtr( new CGM_ButtonSoundPlayer() );
	m_pSubDialogButtonSoundPlayer   = CGM_ButtonSoundPlayerSharedPtr( new CGM_ButtonSoundPlayer() );
	m_pDialogCloseButtonSoundPlayer = CGM_ButtonSoundPlayerSharedPtr( new CGM_ButtonSoundPlayer() );
	m_pListBoxSoundPlayer           = CGM_ListBoxSoundPlayerSharedPtr( new CGM_ListBoxSoundPlayer() );
}
