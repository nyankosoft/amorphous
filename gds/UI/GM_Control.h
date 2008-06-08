#ifndef  __GM_CONTROL_H__
#define  __GM_CONTROL_H__


#include "Graphics/Point.h"
#include "Graphics/Rect.h"
using namespace Graphics;

#include <string>


#include "ui_fwd.h"


/**
 * base class of UI components
 * - CGM_Control and CGM_Dialog are derived from this class
 */
class CGM_ControlBase
{
protected:

	int m_ID;

	/// unique name of the control???
	std::string m_StringID;

	/// valid area of the control
	/// e.g.) clickable area of the button
	SRect m_BoundingBox;

	/// depth in the tree of UI controls
	int m_Depth;

	CGM_ControlRendererSharedPtr m_pRenderer;

public:

	CGM_ControlBase( CGM_ControlDescBase* pDesc );
	virtual ~CGM_ControlBase();

	int GetID() const { return m_ID; }

	const std::string& GetStringID() const { return m_StringID; }

	const SRect& GetBoundingBox() const { return m_BoundingBox; }

	int GetDepth() const { return m_Depth; }

	virtual void SetDepth( int depth ) { m_Depth = depth; }

	// returns the raw pointer of control renderer as a borrowed reference
	CGM_ControlRenderer *GetRenderer() { return m_pRenderer.get(); }

	// returns the shared pointer of the control renderer
	CGM_ControlRendererSharedPtr GetRendererSharedPtr() { return m_pRenderer; }

	/// \param pRenderer owned reference
	virtual void SetRendererSharedPtr( CGM_ControlRendererSharedPtr pRenderer ) { m_pRenderer = pRenderer; }

	virtual void UpdateGraphicsProperties();

	virtual void ChangeScale( float factor );
};


/**
 * base class of UI controls
 * - used as a base class for single UI controls
 * - dialog, which holds a set of single controls, are not derived
 * from this class, but derived from CGM_ControlBase
 */
class CGM_Control : public CGM_ControlBase
{
public:

	enum eGameMenuControlType
	{
		BUTTON = 0, 
		SUBDIALOGBUTTON,
		STATIC,
		CHECKBOX,
		RADIOBUTTON,
		DIALOGCLOSEBUTTON,
		COMBOBOX,
		SLIDER,
		EDITBOX,
		IMEEDITBOX,
		LISTBOX,
		SCROLLBAR,

		PAINTBAR,
		NUM_CONTROL_TYPES
	};

	enum eGM_State
	{
		STATE_NORMAL = 0,
		STATE_DISABLED,
		STATE_HIDDEN,
		STATE_FOCUS,
		STATE_MOUSEOVER,
		STATE_PRESSED,
		STATE_SUBDIALOGOPEN,	///< used only by sub-dialog buttons
		NUM_CONTROL_STATES
	};

	enum eAdjacentControl
	{
		NEXT_UP,
		NEXT_DOWN,
		NEXT_RIGHT,
		NEXT_LEFT,
		NUM_MAX_ADJACENT_CONTROLS
	};

protected:

	bool m_bEnabled;
	bool m_bVisible;
	bool m_bIsDefault;
	bool m_bMouseOver;

	/// focus on control is set to true when, 
	/// a button is pressed
	/// ...
	/// ...
	bool m_bHasFocus;

	std::string m_strCaption;

//	unsigned int m_State;

	/// owner of this control
	CGM_Dialog *m_pDialog;

	CGM_Control *m_apNext[NUM_MAX_ADJACENT_CONTROLS];

//	void UpdateRects();

public:

	CGM_Control( CGM_Dialog *pDialog, CGM_ControlDesc *pDesc );

	virtual ~CGM_Control();

	virtual unsigned int GetType() const = 0;

	virtual bool IsPointInside( SPoint& pt ) const { return m_BoundingBox.ContainsPoint(pt); }

	inline std::string& GetCaptionText() { return m_strCaption; }

	CGM_Dialog *GetOwnerDialog() { return m_pDialog; }

	/// get the bounding box in the local coordinates of owner dialog
	SRect GetLocalRect();


	virtual bool HandleMouseInput( CGM_InputData& input ) { return false; }

	virtual bool HandleKeyboardInput( CGM_InputData& input ) { return false; }

	virtual bool CanHaveFocus() const { return false; }
	virtual void OnFocusIn();
	virtual void OnFocusOut();

	virtual void OnMouseEnter() { m_bMouseOver = true; }
	virtual void OnMouseLeave() { m_bMouseOver = false; }

	bool HasFocus() const { return m_bHasFocus; }

	virtual void SetEnabled( bool bEnabled ) { m_bEnabled = bEnabled; }
	virtual bool IsEnabled() const { return m_bEnabled; }

	virtual void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
	virtual bool IsVisible() const { return m_bVisible; }

	bool IsMouseOver() const { return m_bMouseOver; }

	virtual unsigned int GetState() const;

	CGM_Control *GetNextControl( unsigned int dir ) { return m_apNext[dir]; }

	void SetNextControl( unsigned int dir, CGM_Control *pControl ) { m_apNext[dir] = pControl; }

	virtual bool HasSoundPlayer() const { return false; }

	/// retrieve appropriate sound player from the global sound player, store the pointer to a member variable
	virtual void SetSoundPlayer( CGM_GlobalSoundPlayerSharedPtr pGlobalSoundPlayer ) {}

	/// \param pRenderer owned reference
//	virtual void RegisterRenderer( CGM_ControlRenderer* pRenderer );

public:

	int m_UserIndex;

	void *m_pUserData;

	friend class CGM_Dialog;
};


#endif		/*  __GM_CONTROL_H__  */
