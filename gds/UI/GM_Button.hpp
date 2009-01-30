#ifndef  __GM_BUTTON_H__
#define  __GM_BUTTON_H__

#include "fwd.hpp"
#include "GM_Static.hpp"


/**
 * base class for button control
 */
class CGM_Button : public CGM_Static
{
protected:

	bool m_bPressed;
	
	virtual void OnPressed();

	virtual void OnReleased();

	CGM_ButtonEventHandlerSharedPtr m_pEventHandler;

	CGM_ButtonSoundPlayerSharedPtr m_pSoundPlayer;

public:

	CGM_Button( CGM_Dialog *pDialog, CGM_ButtonDesc *pDesc );

	virtual ~CGM_Button();

	virtual unsigned int GetType() const { return BUTTON; }

	virtual bool HandleMouseInput( CGM_InputData& input );

	virtual bool HandleKeyboardInput( CGM_InputData& input );

	virtual bool CanHaveFocus() const { return (IsVisible() && IsEnabled()); }

	virtual void OnFocusOut();

//	virtual void Render();

	virtual unsigned int GetState() const;

	bool IsPressed() const { return m_bPressed; }

	/// takes pointer to the user defined event handler which inherits 'CGM_ButtonEventHandler'
	void SetButtonEventHandler( CGM_ButtonEventHandlerSharedPtr pEventHandler ) { m_pEventHandler = pEventHandler; }

	void SetButtonSoundPlayer( CGM_ButtonSoundPlayerSharedPtr pSoundPlayer ) { m_pSoundPlayer = pSoundPlayer; }

	virtual bool HasSoundPlayer() const { return ( m_pSoundPlayer.get() != NULL ); }

	virtual void SetSoundPlayer( CGM_GlobalSoundPlayerSharedPtr pGlobalSoundPlayer );
};


/**
 * check box
 * records on/off of something
 */
class CGM_CheckBox : public CGM_Button
{
	void SetCheckedInternal( bool bChecked, bool bFromInput );

protected:

	bool m_bChecked;

//	virtual void OnPressed();

	/// called when the same button is pressed and released
	virtual void OnReleased();

public:

	CGM_CheckBox( CGM_Dialog *pDialog, CGM_CheckBoxDesc *pDesc );
	virtual ~CGM_CheckBox() {}

	virtual unsigned int GetType() const { return CHECKBOX; }

//	virtual bool HandleMouseInput( CGM_InputData& input );

	bool IsChecked() const { return m_bChecked; }

//	virtual void Render();

};


/**
 * radio button
 * only one radio button in a same group can be selected at a time
 */
class CGM_RadioButton : public CGM_CheckBox
{
	int m_iButtonGroup;

protected:

    virtual void SetCheckedInternal( bool bChecked, bool bClearGroup, bool bFromInput );
	
//	virtual void OnPressed();

	virtual void OnReleased();

public:

	CGM_RadioButton( CGM_Dialog *pDialog, CGM_RadioButtonDesc *pDesc );
	virtual ~CGM_RadioButton() {}

	virtual unsigned int GetType() const { return RADIOBUTTON; }

//	virtual bool HandleMouseInput( CGM_InputData& input );

	int GetButtonGroup() const { return m_iButtonGroup; }

	/// called by the dialog to clear the check
	void SetChecked( bool bChecked, bool bClearGroup=true ) { SetCheckedInternal( bChecked, bClearGroup, false ); }

//	void Render();

};


/**
 * base class of event handler for each button
 *
 */
class CGM_ButtonEventHandler
{
public:
	CGM_ButtonEventHandler() {}
	virtual ~CGM_ButtonEventHandler() {}
	virtual void OnPressed() {}
	virtual void OnReleased() {}
};

/* --- template for user defined event handler
class UserButtonEventHandler : public CGM_ButtonEventHandler
{

public:
	UserButtonEventHandler() {}
	virtual ~UserButtonEventHandler() {}
	virtual void OnPressed() {}
	virtual void OnReleased() {}
};
*/

#endif  /*  __GM_BUTTON_H__  */
