#ifndef  __GM_CONTROLDESC_H__
#define  __GM_CONTROLDESC_H__

#include "GM_Control.h"

class CGM_Dialog;


class CGM_ControlDescBase
{
public:

	int ID;

	std::string StringID;

	SRect Rect;	///< rectangular area of this control

	/// NOTE: owned reference
	/// - set to control and released by the control
	/// - user must not release the renderer
	CGM_ControlRendererSharedPtr pRenderer;

public:

	CGM_ControlDescBase() { SetDefault(); }

	virtual ~CGM_ControlDescBase() {}

	virtual void SetDefault();
};


class CGM_ControlDesc : public CGM_ControlDescBase
{
public:

	enum eCoordType
	{
		COORD_LOCAL,    ///< (default) - origin at the top left corner of the parent dialog
		COORD_GLOBAL,   ///< origin at the top left corner of the screen
		NUM_COORD_TYPES
	};

	bool bEnabled;
	bool bVisible;
	bool bIsDefault;
	bool bHasFocus;

	std::string strCaption;	///< text to explain the role of the control

	/// holds coord type of 'Rect' (see CGM_ControlDesc::eCoordType)
	unsigned int coord_type; 

public:

	CGM_ControlDesc() { SetDefault(); }

	virtual ~CGM_ControlDesc() {}

	virtual void SetDefault();

	virtual unsigned int GetType() = 0;

	/// set top-left corner position, width and height
//	inline void SetPositionXYWH( float x, float y, float w, float h );
//	void SetPositionLTBR( float l, float t, float b, float r );
};


#include <string>


class CGM_StaticDesc : public CGM_ControlDesc
{
public:

	std::string strText;

public:

	CGM_StaticDesc() { SetDefault(); }
	virtual ~CGM_StaticDesc() {}

	virtual void SetDefault();

	virtual unsigned int GetType() { return CGM_Control::STATIC; }
};


class CGM_ButtonDesc : public CGM_StaticDesc
{
public:

	bool bPressed;

public:

	CGM_ButtonDesc() { SetDefault(); }
	virtual ~CGM_ButtonDesc() {}

	virtual void SetDefault();

	virtual unsigned int GetType() { return CGM_Control::BUTTON; }
};


class CGM_CheckBoxDesc : public CGM_ButtonDesc
{
public:

	bool bChecked;

public:

	CGM_CheckBoxDesc() { SetDefault(); }
	virtual ~CGM_CheckBoxDesc() {}

	virtual void SetDefault();

	virtual unsigned int GetType() { return CGM_Control::CHECKBOX; }
};


class CGM_RadioButtonDesc : public CGM_CheckBoxDesc
{
public:

	int iButtonGroup;

public:

	CGM_RadioButtonDesc() { SetDefault(); }

	virtual void SetDefault();

	virtual unsigned int GetType() { return CGM_Control::RADIOBUTTON; }
};


class CGM_SubDialogButtonDesc : public CGM_ButtonDesc
{
public:
	CGM_SubDialogButtonDesc() { SetDefault(); }

	CGM_Dialog *pSubDialog;

	virtual void SetDefault();

	virtual unsigned int GetType() { return CGM_Control::SUBDIALOGBUTTON; }
};


class CGM_DialogCloseButtonDesc : public CGM_ButtonDesc
{
public:
	CGM_DialogCloseButtonDesc() { SetDefault(); }

//	virtual void SetDefault();

	virtual unsigned int GetType() { return CGM_Control::DIALOGCLOSEBUTTON; }
};


class CGM_SliderDesc : public CGM_ControlDesc
{
public:

	int iMin;	///< maximum slider value
	int iMax;	///< maximum slider value

	int iInitialValue;	///< initial slider value

public:

	CGM_SliderDesc() { SetDefault(); }

	virtual void SetDefault();

	virtual unsigned int GetType() { return CGM_Control::SLIDER; }
};



#endif		/*  __GM_CONTROLDESC_H__  */
