
#ifndef  __GM_BUTTON_H__
#define  __GM_BUTTON_H__

#include "GM_Static.hpp"


//========================================================================================
// CGM_Button
//========================================================================================

class CGM_ButtonDesc;

class CGM_Button : public CGM_Static
{
protected:
	bool m_bPressed;

public:
	CGM_Button( CGM_Dialog *pDialog, CGM_ButtonDesc *pDesc );
	~CGM_Button();

	bool HandleMouseInput( SInputData& input );

	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }

	virtual void Render( float fElapsedTime );

};


//========================================================================================
// CGM_CheckBox
//========================================================================================

class CGM_CheckBoxDesc;

class CGM_CheckBox : public CGM_Button
{
	SetCheckedInternal( bool bChecked, bool bFromInput );

protected:
	bool m_bChecked;

public:
	CGM_CheckBox( CGM_Dialog *pDialog, CGM_CheckBoxDesc *pDesc );
	~CGM_CheckBox();

	bool HandleMouseInput( SInputData& input );

	bool IsChecked() { return m_bChecked; }

	void Render( float fElapsedTime );

};


//========================================================================================
// CGM_RadioButton
//========================================================================================

class CGM_RadioButtonDesc;

class CGM_RadioButton : public CGM_CheckBox
{
	int m_iButtonGroup;

protected:
    virtual void SetCheckedInternal( bool bChecked, bool bClearGroup, bool bFromInput );

public:
	CGM_RadioButton( CGM_Dialog *pDialog, CGM_RadioButtonDesc *pDesc );
	~CGM_RadioButton();

	bool HandleMouseInput( SInputData& input );

	int GetButtonGroup() { return m_iButtonGroup; }
    void SetChecked( bool bChecked, bool bClearGroup=true ) { SetCheckedInternal( bChecked, bClearGroup, false ); }

   	void Render( float fElapsedTime );

};


#endif  /*  __GM_BUTTON_H__  */
