#ifndef  __FG_CmdMenuControlRenderer_H__
#define  __FG_CmdMenuControlRenderer_H__


#include "UI/ui_fwd.h"
#include "UI/GM_StdControlRenderer.h"
#include "UI/GM_Control.h"

#include "3DCommon/fwd.h"
#include "3DCommon/FloatRGBAColor.h"
#include "Graphics/Point.h"
#include "Graphics/Rect.h"
using namespace Graphics;

#include <string>
#include <vector>


class CFG_CmdMenuControlRenderer : public CGM_StdControlRenderer
{
public:

	CFG_CmdMenuControlRenderer();

	virtual ~CFG_CmdMenuControlRenderer() {}

	virtual void OnFocused();
	virtual void OnFocusCleared();

	/// for button renderers
	virtual void OnPressed();
	virtual void OnReleased();

	/// for check box and radio button renderers
	virtual void OnChecked();
	virtual void OnCheckCleared();

	virtual void OnMouseCursorEntered();
	virtual void OnMouseCursorLeft();

	/// for list box renderer
	virtual void OnItemSelectionFocusCreated();
	virtual void OnItemSelectionChanged();
	virtual void OnItemSelected();

	/// for slider renderer
	virtual void OnSliderValueChanged();

	// for dialog
	virtual void OnDialogOpened();
	virtual void OnDialogClosed();
	virtual void OnOpenDialogAttemptedToClose();

	virtual void OnParentSubDialogButtonFocused();
	virtual void OnParentSubDialogButtonFocusCleared();
};


class CFG_CmdMenuStaticRenderer : public CFG_CmdMenuControlRenderer
{
protected:

	CGE_Text *m_pText;

public:

	CFG_CmdMenuStaticRenderer() : m_pText(NULL) {}

	virtual ~CFG_CmdMenuStaticRenderer() {}

	virtual void Init();
};


class CFG_CmdMenuButtonRenderer : public CFG_CmdMenuStaticRenderer
{
protected:

	CGE_Rect *m_pRect;
	CGE_Rect *m_pFrameRect;

public:

	CFG_CmdMenuButtonRenderer() : m_pRect(NULL), m_pFrameRect(NULL) {}

	virtual ~CFG_CmdMenuButtonRenderer() {}

	virtual void Init();

//	virtual void OnMouseCursorEntered();

//	virtual void OnMouseCursorLeft();

	virtual void OnPressed() {}

	virtual void OnReleased() {}

//	virtual void OnFocused();

//	virtual void OnFocusCleared();
};


class CFG_CmdMenuCheckBoxRenderer : public CFG_CmdMenuButtonRenderer
{
protected:

	std::string m_BaseTitle;
	std::string m_CheckedTitle;

public:

	CFG_CmdMenuCheckBoxRenderer() {}

	virtual ~CFG_CmdMenuCheckBoxRenderer() {}

	virtual void Init();

//	virtual void OnPressed() { OnPressedStd(); }
//	virtual void OnReleased() { OnReleasedStd(); }
//	virtual void OnMouseCursorEntered() {}
//	virtual void OnMouseCursorLeft() {}

	// what about OnPressed()?
	// - call OnPressed() & OnChecked()
	virtual void OnChecked();

	virtual void OnCheckCleared();
};


class CFG_CmdMenuRadioButtonRenderer : public CFG_CmdMenuCheckBoxRenderer
{
public:

	CFG_CmdMenuRadioButtonRenderer() {}

	virtual ~CFG_CmdMenuRadioButtonRenderer() {}

	virtual void Init();

	virtual void OnChecked();

	virtual void OnCheckCleared();
};


/*
class CFG_CmdMenuSubDialogButtonRenderer
{
public:

	CGM_SubDialogButtonRenderer() {}
	virtual ~CGM_SubDialogButtonRenderer() {}
	virtual void Init() {}
};


class CFG_CmdMenuDialogCloseButtonRenderer
{
public:

	CGM_DialogCloseButtonRenderer() {}
	virtual ~CGM_DialogCloseButtonRenderer() {}
	virtual void Init() {}
};
*/


class CFG_CmdMenuListBoxRenderer : public CFG_CmdMenuControlRenderer
{
protected:

	CGE_Rect *m_pRect;
	CGE_Rect *m_pFrameRect;

	std::vector<CGE_Text *> m_vecpText;

protected:

	void UpdateItems( bool update_text = true );

public:

	CFG_CmdMenuListBoxRenderer() : m_pRect(NULL), m_pFrameRect(NULL) {}

	virtual ~CFG_CmdMenuListBoxRenderer() {}

	virtual void Init();

	virtual void OnItemSelectionChanged();

	virtual void OnItemSelected();

	virtual void OnItemAdded( int index );
	virtual void OnItemInserted( int index );
	virtual void OnItemRemoved( int index );
};


class CFG_CmdMenuSliderRenderer : public CFG_CmdMenuControlRenderer
{
public:

	CFG_CmdMenuSliderRenderer() {}

	virtual ~CFG_CmdMenuSliderRenderer() {}

	virtual void Init() {}
};


class CFG_CmdMenuDialogRenderer : public CFG_CmdMenuControlRenderer
{
	CGE_Rect *m_pRect;
	CGE_Rect *m_pFrameRect;

	CGE_Text *m_pText;
	CGE_Rect *m_pSeparatorRect;

	Vector2 m_vSlideIn;
	Vector2 m_vSlideOut;

public:

	CFG_CmdMenuDialogRenderer();

	virtual ~CFG_CmdMenuDialogRenderer() {}

	virtual void Init();

	virtual void OnDialogOpened();
	virtual void OnDialogClosed();
	virtual void OnOpenDialogAttemptedToClose() {}
};


#endif		/*  __FG_CmdMenuControlRenderer_H__  */
