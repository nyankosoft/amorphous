#ifndef  __GUIGameTask_H__
#define  __GUIGameTask_H__


#include "GameTask.hpp"
#include "GUI/fwd.hpp"


class CGUIGameTask : public CGameTask
{
	/// Initialized in ctor
	CGM_DialogManagerSharedPtr m_pDialogBoxManager;

	/// default GUI conponents renderer, 'CFG_StdControlRendererManager' is set in ctor
	CGM_ControlRendererManagerSharedPtr m_pUIRendererManager;

	bool m_RenderGUI;

protected:

	CGM_DialogManagerSharedPtr& DialogBoxManager() { return m_pDialogBoxManager; }

	void SetRenderGUI( bool render_gui ) { m_RenderGUI = render_gui; }

	void SetGUIRendererManager( CGM_ControlRendererManagerSharedPtr pRendererManager );

public:

	CGUIGameTask();

	virtual ~CGUIGameTask();

	virtual int FrameMove( float dt );

	virtual void Render();
};


#endif  /*  __GUIGameTask_H__  */
