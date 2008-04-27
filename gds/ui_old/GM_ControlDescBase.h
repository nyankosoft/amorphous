
#ifndef  __GM_CONTROLDESC_H__
#define  __GM_CONTROLDESC_H__

#include "GM_Control.h"

class CGM_Dialog;
class CGM_FontElement;
class CGM_TextureRectElement;

class CGM_ControlDesc
{
protected:

	CGM_Control::eGameMenuControlType m_Type;

public:

	int ID;

	float fPosX, fPosY;
	float fWidth, fHeight;

	bool bEnabled;
	bool bVisible;
	bool bIsDefault;
	bool bHasFocus;

	string strCaption;	// text to explain the role of the control

	CGM_ControlDesc();
	~CGM_ControlDesc();

	CGM_Control::eGameMenuControlType GetType() { return m_Type; }

	virtual void SetDefault();

	/// set top-left corner position, width and height
	inline void SetPositionXYWH( float x, float y, float w, float h );

};


inline void CGM_ControlDesc::SetPositionXYWH( float x, float y, float w, float h )
{
	fPosX = x;
	fPosY = y;
	fWidth = w;
	fHeight = h;
}



#include <string>
using namespace std;

class CGM_StaticDesc : public CGM_ControlDesc
{
public:
	CGM_StaticDesc() { m_Type = CGM_Control::STATIC; SetDefault(); }

	string strText;
	CGM_FontElement *pDefaultFontElement;
	CGM_TextureRectElement *pDefaultRectElement;

	virtual void SetDefault();
};


class CGM_ButtonDesc : public CGM_StaticDesc
{
public:
	CGM_ButtonDesc() { m_Type = CGM_Control::BUTTON; SetDefault(); }

	bool bPressed;

	virtual void SetDefault();
};


class CGM_CheckBoxDesc : public CGM_ButtonDesc
{
public:
	CGM_CheckBoxDesc() { m_Type = CGM_Control::CHECKBOX; SetDefault(); }

	bool bChecked;

	virtual void SetDefault();
};


class CGM_RadioButtonDesc : public CGM_CheckBoxDesc
{
public:
	CGM_RadioButtonDesc() { m_Type = CGM_Control::RADIOBUTTON; SetDefault(); }

	int iButtonGroup;

	virtual void SetDefault();
};


class CGM_SubDialogButtonDesc : public CGM_ButtonDesc
{
public:
	CGM_SubDialogButtonDesc() { m_Type = CGM_Control::SUBDIALOGBUTTON; SetDefault(); }

	CGM_Dialog *pSubDialog;

	virtual void SetDefault();
};


class CGM_SliderDesc : public CGM_ControlDesc
{
public:
	CGM_SliderDesc() { m_Type = CGM_Control::SLIDER; SetDefault(); }

	int iMin;	// maximum slider value
	int iMax;	// maximum slider value

	int iInitialValue;	// initial slider value

	CGM_TextureRectElement *pRectElement;
	CGM_TextureRectElement *pButtonElement;

	virtual void SetDefault();
};



#endif		/*  __GM_CONTROLDESC_H__  */