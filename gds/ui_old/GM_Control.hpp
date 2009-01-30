
#ifndef  __GM_CONTROL_H__
#define  __GM_CONTROL_H__


#include "Graphics/Point.hpp"
#include "Graphics/Rect.hpp"
using namespace Graphics;


// experiment
#include "D:\3D_Development\temp_code\UI\GM_RectRenderRoutine.h"



/// CGM_Control
/// base class of game menu controls

#include <string>
using namespace std;


struct SInputData;
class CGM_Dialog;
class CGM_ControlDesc;


#define GM_SCREEN_WIDTH		800
#define GM_SCREEN_HEIGHT	600


//========================================================================================
// CGM_Control
//========================================================================================

class CGM_Control
{
public:

	enum eGameMenuControlType
	{
		BUTTON = 0, 
		SUBDIALOGBUTTON, 
		STATIC, 
		CHECKBOX,
		RADIOBUTTON,
		COMBOBOX,
		SLIDER,
		EDITBOX,
		IMEEDITBOX,
		LISTBOX,
		SCROLLBAR,

		PAINTBAR
	};

protected:

	eGameMenuControlType m_Type;		// set in the constructor

	int m_ID;

	float m_fPosX, m_fPosY;
	float m_fWidth, m_fHeight;

	bool m_bEnabled;
	bool m_bVisible;
	bool m_bIsDefault;
	bool m_bMouseOver;
	bool m_bHasFocus;

	string m_strCaption;

	SRect m_BoundingBox;

	void UpdateRects();

public:

	CGM_Control( CGM_Dialog *pDialog, CGM_ControlDesc *pDesc );
	~CGM_Control();

	inline eGameMenuControlType GetType() const { return m_Type; }

	inline int GetID() const { return m_ID; }

	inline bool IsPointInside( SPoint& pt ) { return m_BoundingBox.ContainsPoint(pt); }

	inline string& GetCaptionText() { return m_strCaption; }


    virtual void Render( float fElapsedTime );


	virtual bool HandleMouseInput( SInputData& input ) { return false; }

    virtual bool CanHaveFocus() { return false; }
    virtual void OnFocusIn() { m_bHasFocus = true; }
    virtual void OnFocusOut() { m_bHasFocus = false; }

	virtual void OnMouseEnter() { m_bMouseOver = true; }
	virtual void OnMouseLeave() { m_bMouseOver = false; }

    virtual void SetEnabled( bool bEnabled ) { m_bEnabled = bEnabled; }
    virtual bool IsEnabled() { return m_bEnabled; }

    virtual void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
    virtual bool IsVisible() { return m_bVisible; }


	CGM_Dialog *m_pDialog;
};


#endif		/*  __GM_CONTROL_H__  */