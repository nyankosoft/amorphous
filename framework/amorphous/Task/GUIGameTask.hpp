#ifndef  __GUIGameTask_H__
#define  __GUIGameTask_H__


#include "GameTask.hpp"
#include "amorphous/GUI/fwd.hpp"


namespace amorphous
{


class GUIGameTask : public GameTask
{
	/// Initialized in ctor
	CGM_DialogManagerSharedPtr m_pDialogBoxManager;

	/// default GUI conponents renderer, 'CFG_StdControlRendererManager' is set in ctor
	CGM_ControlRendererManagerSharedPtr m_pUIRendererManager;

	std::shared_ptr<CInputHandler_Dialog> m_pGUIInputHandler;

	bool m_RenderGUI;

protected:

	CGM_DialogManagerSharedPtr& DialogBoxManager() { return m_pDialogBoxManager; }

	void SetRenderGUI( bool render_gui ) { m_RenderGUI = render_gui; }

	void SetGUIRendererManager( CGM_ControlRendererManagerSharedPtr pRendererManager );

	CGM_ControlRendererManagerSharedPtr GetGUIRendererManager() { return m_pUIRendererManager; }

public:

	GUIGameTask();

	virtual ~GUIGameTask();

	virtual int FrameMove( float dt );

	virtual void Render();
};

} // namespace amorphous



#endif  /*  __GUIGameTask_H__  */
