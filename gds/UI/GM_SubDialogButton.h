#ifndef  __GM_SUBDIALOGBUTTON_H__
#define  __GM_SUBDIALOGBUTTON_H__

#include "GM_Button.h"


/**
 * button that can own a sub-dialog and open it when the button is pressed.
 * - sub-dialog can be set through the desc at the initialization
 *   or later by calling SetSubDialog()
 */
class CGM_SubDialogButton : public CGM_Button
{
	CGM_Dialog *m_pSubDialog;

	virtual void OnPressed();

	virtual void OnReleased();

public:

	CGM_SubDialogButton( CGM_Dialog *pDialog, CGM_SubDialogButtonDesc *pDesc );

	~CGM_SubDialogButton() {}

	virtual unsigned int GetType() const { return SUBDIALOGBUTTON; }

	bool HandleMouseInput( CGM_InputData& input );

	bool HandleInputInSubDialog( CGM_InputData& input );

//	virtual void OnFocusIn();

	/// used to set the dialog later after creating the control
	void SetSubDialog( CGM_Dialog* pDialog );

	void OpenDialog();

	/// recursively close all the child dialogs
	void CloseSubDialogs();

	void OnSubDialogClosed();

	bool IsSubDialogOpen() const;

	virtual unsigned int GetState() const;

	virtual void SetDepth( int depth );

	virtual void UpdateGraphicsProperties();

//	virtual void Render();

//	void RenderSubDialog();

	/// recursive
	int GetMaxDepth() const;

	virtual void SetSoundPlayer( CGM_GlobalSoundPlayerSharedPtr pGlobalSoundPlayer );
};


#endif  /*  __GM_SUBDIALOGBUTTON_H__  */
