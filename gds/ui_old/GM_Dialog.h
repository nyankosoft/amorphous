
#ifndef  __GM_DIALOG_H__
#define  __GM_DIALOG_H__


#include <d3dx9.h>

#include "GM_GraphicElement.h"

#include <vector>
#include <string>
using namespace std;

#include "Graphics/Point.h"
using namespace Graphics;


enum eGameMenuEventType
{
	GM_EVENT_BUTTON_CLICKED = 0,
	GM_EVENT_COMBOBOX_SELECTION_CHANGED,
	GM_EVENT_RADIOBUTTON_CHANGED,
	GM_EVENT_CHECKBOX_CHANGED,
	GM_EVENT_SLIDER_VALUE_CHANGED,

	GM_EVENT_PAINTBAR_VALUE_CHANGED
};


struct SInputData;
class CFontBase;
class CGM_Control;
class CGM_ControlDesc;
class CGM_DialogDesc;
class CGM_DialogManager;

typedef void (*PCALLBACK_GM_GUIEVENT) ( eGameMenuEventType event, int iControlID, CGM_Control* pControl );


//========================================================================================
// CGM_Dialog
//========================================================================================

class CGM_Dialog
{
protected:

	CGM_DialogManager *m_pDialogManager;

	bool m_bIsOpen;

	vector<CGM_Control *> m_vecpControl;

	bool m_bMinimized;
	bool m_bCaption;
	string m_strCaption;
	int m_iCaptionHeight;

	bool m_bNonUserEvents;

    float m_fPosX, m_fPosY;
    float m_fWidth, m_fHeight;
    float m_nCaptionHeight;

	CGM_TextureRectElement m_BackgroundRect;

//	D3DCOLOR m_colorTopLeft, m_colorTopRight, m_colorBottomLeft, m_colorBottomRight;

	PCALLBACK_GM_GUIEVENT m_pEventHandler;	// Callback function to handle events

	CGM_Control* m_pControlMouseOver;           // The control which is hovered over

    static CGM_Control* ms_pControlFocus;        // The control which has focus
    static CGM_Control* ms_pControlPressed;      // The control currently pressed


    // Windows message handlers
    void OnMouseMove( SPoint& pt );
    void OnMouseUp( SPoint& pt );

    // Control events
	void OnCycleFocus( bool bForward );
	void OnMouseEnter( CGM_Control* pControl );
	void OnMouseLeave( CGM_Control* pControl );

public:
	CGM_Dialog(	CGM_DialogManager *pDialogManager, CGM_DialogDesc& desc );
	~CGM_Dialog();

	/// add a control to the dialog
	/// and returns a pointer to the created control
	CGM_Control *AddControl( CGM_ControlDesc *pControlDesc );

	bool IsOpen();
	void Open();

	/// close this dialog and all the subdialogs
	void Close();

	/// close all the sub dialogs belonging to this dialog
	void CloseSubDialogs();

	bool HandleInput( SInputData& input );

	void SendEvent( eGameMenuEventType event, bool bTriggeredByUser, CGM_Control* pControl );
    void RequestFocus( CGM_Control* pControl );

	CGM_Control *GetControlAtPoint( SPoint& pt );


	void ClearRadioButtonGroup( int iButtonGroup );

	static void ClearFocus();

	void Render( float fElapsedTime );

	CFontBase *GetFont( int iIndex );

	LPDIRECT3DTEXTURE9 GetTexture( int iIndex );

};



#endif		/*  __GM_DIALOG_H__  */