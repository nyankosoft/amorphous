
#ifndef  __GM_SUBDIALOGBUTTON_H__
#define  __GM_SUBDIALOGBUTTON_H__

#include "GM_Button.hpp"


//========================================================================================
// CGM_SubDialogButton
//========================================================================================

class CGM_SubDialogButtonDesc;

class CGM_SubDialogButton : public CGM_Button
{
	CGM_Dialog *m_pSubDialog;

public:

	CGM_SubDialogButton( CGM_Dialog *pDialog, CGM_SubDialogButtonDesc *pDesc );

	~CGM_SubDialogButton() {}

	bool HandleMouseInput( SInputData& input );

	bool HandleInputInSubDialog( SInputData& input );

	void CloseSubDialogs();

	void SetSubDialog( CGM_Dialog* pDialog ) { m_pSubDialog = pDialog; }

	void Render( float fElapsedTime );

};


#endif  /*  __GM_SUBDIALOGBUTTON_H__  */