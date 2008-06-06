#ifndef  __FG_STDCONTROLRENDERER_H__
#define  __FG_STDCONTROLRENDERER_H__


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


class CFG_StdControlRenderer : public CGM_StdControlRenderer
{
public:

	CFG_StdControlRenderer();

	virtual ~CFG_StdControlRenderer() {}

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


class CFG_StdStaticRenderer : public CFG_StdControlRenderer
{
protected:

	CGE_Text *m_pText;

public:

	CFG_StdStaticRenderer() : m_pText(NULL) {}

	virtual ~CFG_StdStaticRenderer() {}

	virtual void Init();

	virtual void OnTextChanged();
};


class CFG_StdButtonRenderer : public CFG_StdStaticRenderer
{
protected:

	CGE_Rect *m_pRect;
	CGE_Rect *m_pFrameRect;

public:

	CFG_StdButtonRenderer() : m_pRect(NULL), m_pFrameRect(NULL) {}

	virtual ~CFG_StdButtonRenderer() {}

	virtual void Init();

//	virtual void OnMouseCursorEntered();

//	virtual void OnMouseCursorLeft();

	virtual void OnPressed() {}

	virtual void OnReleased() {}

//	virtual void OnFocused();

//	virtual void OnFocusCleared();
};


class CFG_StdCheckBoxRenderer : public CFG_StdButtonRenderer
{
protected:

	std::string m_BaseTitle;

public:

	CFG_StdCheckBoxRenderer() {}

	virtual ~CFG_StdCheckBoxRenderer() {}

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


class CFG_StdRadioButtonRenderer : public CFG_StdCheckBoxRenderer
{
public:

	CFG_StdRadioButtonRenderer() {}

	virtual ~CFG_StdRadioButtonRenderer() {}

	virtual void Init();

	virtual void OnChecked();

	virtual void OnCheckCleared();
};


/*
class CFG_StdSubDialogButtonRenderer
{
public:
	CGM_SubDialogButtonRenderer() {}
	virtual ~CGM_SubDialogButtonRenderer() {}
	virtual void Init() {}
};

class CFG_StdDialogCloseButtonRenderer
{
public:
	CGM_DialogCloseButtonRenderer() {}
	virtual ~CGM_DialogCloseButtonRenderer() {}
	virtual void Init() {}
};
*/


class CFG_StdListBoxRenderer : public CFG_StdControlRenderer
{
protected:

	CGE_Rect *m_pRect;
	CGE_Rect *m_pFrameRect;

	std::vector<CGE_Text *> m_vecpText;

protected:

	void UpdateItems( bool update_text = true );

public:

	CFG_StdListBoxRenderer() : m_pRect(NULL), m_pFrameRect(NULL) {}

	virtual ~CFG_StdListBoxRenderer() {}

	virtual void Init();

	virtual void OnFocused();

	virtual void OnItemSelectionChanged();

	virtual void OnItemSelected();

	virtual void OnItemAdded( int index );
	virtual void OnItemInserted( int index );
	virtual void OnItemRemoved( int index );

	virtual void OnItemTextChanged( CGM_ListBoxItem& item );
	virtual void OnItemDescChanged( CGM_ListBoxItem& item );
	virtual void OnItemUpdated( CGM_ListBoxItem& item );
};


class CFG_StdSliderRenderer : public CFG_StdControlRenderer
{
public:

	CFG_StdSliderRenderer() {}

	virtual ~CFG_StdSliderRenderer() {}

	virtual void Init() {}
};


class CFG_StdDialogRenderer : public CFG_StdControlRenderer
{
	CGE_Rect *m_pRect;
	CGE_Rect *m_pFrameRect;

	// for dialog title
	CGE_Rect *m_pTitleRect;
	CGE_Text *m_pTitle;

	Vector2 m_vSlideIn;
	Vector2 m_vSlideOut;

	CGraphicsEffectHandle m_PrevSlideEffect;

public:

	CFG_StdDialogRenderer();

	virtual ~CFG_StdDialogRenderer() {}

	virtual void Init();

	virtual void OnDialogOpened();
	virtual void OnDialogClosed();
	virtual void OnOpenDialogAttemptedToClose() {}

	void SetSlideInAmount( Vector2 vSlideIn ) { m_vSlideIn = vSlideIn; }
	void SetSlideOutAmount( Vector2 vSlideOut ) { m_vSlideOut = vSlideOut; }
	void SetSlideInOutAmount( Vector2 vSlideIn, Vector2 vSlideOut ) { m_vSlideIn = vSlideIn; m_vSlideOut = vSlideOut; }
};


#endif		/*  __FG_STDCONTROLRENDERER_H__  */
