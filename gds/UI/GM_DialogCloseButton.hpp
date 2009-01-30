
#ifndef  __GM_DialogCloseButton_H__
#define  __GM_DialogCloseButton_H__

#include "GM_Button.hpp"


class CGM_DialogCloseButtonDesc;


/**
 * button that close the dialog when it is pressed
 * when a dialog close button is pressed & released, the 2 events are sent in the following order.
 *
 * - CGM_Event::BUTTON_CLICKED - sent right after the button is released
 * - CGM_Event::DIALOG_CLOSED/DIALOG_CLOSED
 */
class CGM_DialogCloseButton : public CGM_Button
{
protected:

	virtual void OnPressed();

	virtual void OnReleased();

public:

	CGM_DialogCloseButton( CGM_Dialog *pDialog, CGM_DialogCloseButtonDesc *pDesc );
	virtual ~CGM_DialogCloseButton() {}

	virtual unsigned int GetType() const { return DIALOGCLOSEBUTTON; }

	virtual void SetSoundPlayer( CGM_GlobalSoundPlayerSharedPtr pGlobalSoundPlayer );

//	virtual void OnFocusOut();

///	virtual void Render();

//	virtual unsigned int GetState() const;


};


#endif  /*  __GM_DialogCloseButton_H__  */
