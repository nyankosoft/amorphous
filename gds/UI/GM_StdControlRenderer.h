#ifndef  __GM_STDCONTROLRENDERER_H__
#define  __GM_STDCONTROLRENDERER_H__


#include "ui_fwd.h"
#include "GM_ControlRenderer.h"
#include "GM_Control.h"

#include "3DCommon/fwd.h"
#include "3DCommon/FloatRGBAColor.h"
#include "3DCommon/GraphicsEffectHandle.h"
#include "Graphics/Point.h"
#include "Graphics/Rect.h"
using namespace Graphics;

#include <string>
#include <vector>


class CGM_StdControlRenderer : public CGM_ControlRenderer
{
private:

	class ColoredElementHolder
	{
	public:

		CGraphicsElement *m_pElement;
		CGraphicsEffectHandle m_PrevEffect;

		ColoredElementHolder() : m_pElement(NULL) {}
		ColoredElementHolder(CGraphicsElement *pElement) : m_pElement(pElement) {}
	};

	std::vector<ColoredElementHolder> m_vecColoredElement;

protected:

	SFloatRGBAColor m_aColor[ CGM_Control::NUM_CONTROL_STATES ];

protected:

	/// updates the color of graphics elements registered as colored elements
	/// called when a state of the owner control changes
	/// - focus in/out, mouse cursor entered/left
	void ChangeColorToCurrentState();

	void RegisterColoredElement(CGraphicsElement *pElement) { m_vecColoredElement.push_back( ColoredElementHolder(pElement) ); }

public:

	CGM_StdControlRenderer();

	virtual ~CGM_StdControlRenderer() {}

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


class CGM_StdStaticRenderer : public CGM_StdControlRenderer
{
protected:

	CGE_Text *m_pText;

public:

	CGM_StdStaticRenderer() : m_pText(NULL) {}

	virtual ~CGM_StdStaticRenderer() {}

	virtual void Init();
};


class CGM_StdButtonRenderer : public CGM_StdStaticRenderer
{
protected:

	CGE_Rect *m_pRect;
	CGE_Rect *m_pFrameRect;

public:

	CGM_StdButtonRenderer() : m_pRect(NULL), m_pFrameRect(NULL) {}

	virtual ~CGM_StdButtonRenderer() {}

	virtual void Init();

//	virtual void OnMouseCursorEntered();

//	virtual void OnMouseCursorLeft();

	virtual void OnPressed() {}

	virtual void OnReleased() {}

//	virtual void OnFocused();

//	virtual void OnFocusCleared();
};


class CGM_StdCheckBoxRenderer : public CGM_StdButtonRenderer
{
protected:

	std::string m_BaseTitle;

public:

	CGM_StdCheckBoxRenderer() {}

	virtual ~CGM_StdCheckBoxRenderer() {}

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


class CGM_StdRadioButtonRenderer : public CGM_StdCheckBoxRenderer
{
public:

	CGM_StdRadioButtonRenderer() {}

	virtual ~CGM_StdRadioButtonRenderer() {}

	virtual void Init();

	virtual void OnChecked();

	virtual void OnCheckCleared();
};


/*
class CGM_StdSubDialogButtonRenderer
{
public:

	CGM_SubDialogButtonRenderer() {}
	virtual ~CGM_SubDialogButtonRenderer() {}
	virtual void Init() {}
};


class CGM_StdDialogCloseButtonRenderer
{
public:

	CGM_DialogCloseButtonRenderer() {}
	virtual ~CGM_DialogCloseButtonRenderer() {}
	virtual void Init() {}
};
*/


class CGM_StdListBoxRenderer : public CGM_StdControlRenderer
{
protected:

	CGE_Rect *m_pRect;
	CGE_Rect *m_pFrameRect;

	std::vector<CGE_Text *> m_vecpText;

protected:

	void UpdateItems( bool update_text = true );

public:

	CGM_StdListBoxRenderer() : m_pRect(NULL), m_pFrameRect(NULL) {}

	virtual ~CGM_StdListBoxRenderer() {}

	virtual void Init();

	virtual void OnItemSelectionChanged();

	virtual void OnItemSelected();

	virtual void OnItemAdded( int index );
	virtual void OnItemInserted( int index );
	virtual void OnItemRemoved( int index );
};


class CGM_StdSliderRenderer : public CGM_StdControlRenderer
{

	CGE_Rect *m_pRect;
	CGE_Rect *m_pFrameRect;
	CGE_Rect *m_pSliderButtonRect;
	CGE_Rect *m_pSliderButtonFrameRect;

public:

	CGM_StdSliderRenderer()
		:
	m_pRect(NULL),
	m_pFrameRect(NULL),
	m_pSliderButtonRect(NULL),
	m_pSliderButtonFrameRect(NULL)
	{}

	virtual ~CGM_StdSliderRenderer() {}

	virtual void Init();

	virtual void OnSliderValueChanged();
};


class CGM_StdDialogRenderer : public CGM_StdControlRenderer
{
	CGE_Rect *m_pRect;
	CGE_Rect *m_pFrameRect;

	CGraphicsEffectHandle m_PrevSlideEffect;

public:

	CGM_StdDialogRenderer() : m_pRect(NULL), m_pFrameRect(NULL)  {}

	virtual ~CGM_StdDialogRenderer() {}

	virtual void Init();

	virtual void OnDialogOpened();
	virtual void OnDialogClosed();
	virtual void OnOpenDialogAttemptedToClose() {}
};


#endif		/*  __GM_STDCONTROLRENDERER_H__  */
