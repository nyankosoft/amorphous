#include "GUIGameTask.hpp"

#include "UI.hpp"
#include "Graphics/GraphicsElementManager.hpp"
#include "Input/InputHub.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/Log/StateLog.hpp"
#include "Support/Log/DefaultLog.hpp"

using namespace std;
using namespace boost;


CGUIGameTask::CGUIGameTask()
:
m_RenderGUI(true)
{
	CGM_ControlRendererManagerSharedPtr pRenderMgr( new CGM_StdControlRendererManager() );
	m_pUIRendererManager= pRenderMgr;

    m_pDialogBoxManager	= CGM_DialogManagerSharedPtr( new CGM_DialogManager() );

	// set input handler for dialog menu
	m_pInputHandler = new CInputHandler_Dialog( m_pDialogBoxManager );
	InputHub().PushInputHandler( m_pInputHandler );
}


CGUIGameTask::~CGUIGameTask()
{
}


int CGUIGameTask::FrameMove( float dt )
{
	int ret = CGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( m_pDialogBoxManager )
		m_pDialogBoxManager->Update( dt );

	return CGameTask::ID_INVALID;
}


void CGUIGameTask::Render()
{
	// render stage select dialog
	if( m_pDialogBoxManager && m_RenderGUI )
		m_pDialogBoxManager->Render();
}


void CGUIGameTask::SetGUIRendererManager( CGM_ControlRendererManagerSharedPtr pRendererManager )
{
//	m_pDialogBoxManager->Set...
}
