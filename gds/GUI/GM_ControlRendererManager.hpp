#ifndef  __GM_CONTROLRENDERERMANAGER_H__
#define  __GM_CONTROLRENDERERMANAGER_H__


#include <vector>
#include <string>

#include "fwd.hpp"
#include "GM_ControlRenderer.hpp"
#include "Graphics/fwd.hpp"


/**
 writing your own UI graphics code

 1. create control renderers classes for each controls
 2. create UI dialogs and controls
 3. register control renderers to controls
   A. implement CGM_ControlRendererManager::CreateXXXRenderer() functions
     - These are default renderers used as fallback when no specific renderers are registered to the control
	   when you call CGM_Dialog::AddXXX()
   B. provide specific control renderer to each control
     - supply shared ptr to CGM_Dialog::AddXXX() reference e.g.) pButton->RegisterRenderer( new UserButtonRenderer() )
	 - use this to set different renderers to controls of a same type
*/


class CGM_ControlRendererManager
{
protected:

	/// owned reference
//	std::vector<CGM_ControlRenderer *> m_vecpControlRenderer;

	/// borrowed reference
	CGraphicsElementManager *m_pGraphicsElementManager;

	/// owned / borrowed reference
	CAnimatedGraphicsManagerSharedPtr m_pGraphicsEffectManager;

protected:

//	void RegisterRenderer( CGM_ControlRenderer *pControlRenderer, CGM_ControlBase *pControl );

public:

	CGM_ControlRendererManager();

	CGM_ControlRendererManager( CAnimatedGraphicsManagerSharedPtr pGraphicsEffectManager );

	virtual ~CGM_ControlRendererManager();

	void Release();

//	virtual void OnControlCreated( CGM_Control *pControl );

	virtual CGM_ControlRenderer *CreateStaticRenderer( CGM_Static *pStatic ) { return new CGM_ControlRenderer(); }
	virtual CGM_ControlRenderer *CreateButtonRenderer( CGM_Button *pButton ) { return new CGM_ControlRenderer(); }
	virtual CGM_ControlRenderer *CreateCheckBoxRenderer( CGM_CheckBox *pCheckBox ) { return new CGM_ControlRenderer(); }
	virtual CGM_ControlRenderer *CreateRadioButtonRenderer( CGM_RadioButton *pRadioButton ) { return new CGM_ControlRenderer(); }
	virtual CGM_ControlRenderer *CreateDialogCloseButtonRenderer( CGM_DialogCloseButton *pDlgCloseButton ) { return new CGM_ControlRenderer(); }
	virtual CGM_ControlRenderer *CreateSliderRenderer( CGM_Slider *pSlider ) { return new CGM_ControlRenderer(); }
	virtual CGM_ControlRenderer *CreateListBoxRenderer( CGM_ListBox *pListBox ) { return new CGM_ControlRenderer(); }
	virtual CGM_ControlRenderer *CreateScrollBarRenderer( CGM_ScrollBar *pScrollBar ) { return new CGM_ControlRenderer(); }
	virtual CGM_ControlRenderer *CreateSubDialogButtonRenderer( CGM_SubDialogButton *pSubDlgButton ) { return new CGM_ControlRenderer(); }
	virtual CGM_ControlRenderer *CreateDialogRenderer( CGM_Dialog *pDialog ) { return new CGM_ControlRenderer(); }
	virtual CGM_ControlRenderer *CreateCaptionRenderer() { return new CGM_ControlRenderer(); }

	virtual int GetNumGraphicsLayersPerUILayer() { return 8; } ///< called by control renderers

	virtual int CalcUILayerIndex( CGM_ControlRenderer *pControlRenderer );

//	virtual void ChangeScale( float factor ) {}


	CGraphicsElementManager *GetGraphicsElementManager() { return m_pGraphicsElementManager; }

	CAnimatedGraphicsManagerBase *GetGraphicsEffectManager() { return m_pGraphicsEffectManager.get(); }

//	void UpdateGraphicsLayers();

	void Render();

	void Update( float dt );
};


/**
 writing your own UI graphics code

 1. create control renderers classes for each controls
 2. create UI dialogs and controls
 3. register control renderers to controls
   A. implement CGM_ControlRendererManager::OnXXXCreated() functions
     - default renderers used as fallback when no specific renderers are registered to the control
   B. call CGM_ControlBase::RegisterRenderer() for each control
     - supply owned reference e.g.) pButton->RegisterRenderer( new UserButtonRenderer() )
	 - use this to set different renderers to each control

*/


/*
	virtual void OnStaticCreated( CGM_Static *pStatic ) {}
	virtual void OnButtonCreated( CGM_Button *pButton ) {}
	virtual void OnCheckBoxCreated( CGM_CheckBox *pCheckBox ) {}
	virtual void OnRadioButtonCreated( CGM_RadioButton *pRadioButton ) {}
	virtual void OnDialogCloseButtonCreated( CGM_DialogCloseButton *pDlgCloseButton ) {}
	virtual void OnSliderCreated( CGM_Slider *pSlider ) {}
	virtual void OnListBoxCreated( CGM_ListBox *pListBox ) {}
	virtual void OnScrollBarCreated( CGM_ScrollBar *pScrollBar ) {}
	virtual void OnSubDialogButtonCreated( CGM_SubDialogButton *pSubDlgButton ) {}
	virtual void OnDialogCreated( CGM_Dialog *pDialog ) {}
*/


// 17:03 12/10/2007
// plan1
// - define CGM_ControlRenderer as a base class for user-defined control renderers
// - define CGM_XXXRenderer classes for each control
//   - but they do not inherit CGM_ControlRenderer
//   - In CGM_XXXRenderer class, define virutual functions for all the events that can occur to the control of that type
// - using multiple inheritance, derive user-defined control renderer class from the corresponding CGM_XXXRenderer class and CGM_Renderer
//   - could be from the corresponding CGM_XXXRenderer class and CGM_UserRendererBase, which inherits CGM_Renderer
//   - implement virtual functions in each user-defined renderer classes
// 
// pros.
// - User-defined classes can have hierarchy
// cons.
// - Obviously, there will be a lot of typing involved.
//   - each CGM_XXXRenderer class needs to define all the event handler virtual functions.
//   - this is also true to user-defined classes



#endif		/*  __GM_CONTROLRENDERERMANAGER_H__  */
