#include "StageViewerGameTask.hpp"

#include "Support/Log/DefaultLog.hpp"
#include "Graphics/GraphicsElementManager.hpp"
#include "Input/InputHub.hpp"
#include "Stage.hpp"
#include "GUI.hpp"


namespace amorphous
{

using namespace std;
using namespace boost;


class CGameTaskDesc
{
public:
	std::string ScriptName;
	int InputHandlerIndex;
};


CStageViewerGameTask::CStageViewerGameTask()
:
m_EnableCameraController(true)
{
	// An input handler for camera control is added as a child of m_pInputHandler.
	m_pCameraController.reset( new CCameraController( m_pInputHandler ) );

	m_Camera.SetAspectRatio( (float)CGraphicsComponent::GetScreenWidth() / (float)CGraphicsComponent::GetScreenHeight() );
}


CStageViewerGameTask::~CStageViewerGameTask()
{
}


int CStageViewerGameTask::FrameMove( float dt )
{
	int ret = CSingleStageGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

//	UpdateCameraPose();

	if( m_pCameraController && m_EnableCameraController )
	{
		m_pCameraController->UpdateCameraPose( dt );
		m_Camera.SetPose( m_pCameraController->GetPose() );
		m_Camera.UpdateVFTreeForWorldSpace();
	}

	return CGameTask::ID_INVALID;
}


void CStageViewerGameTask::Render()
{
	// render stage
	if( m_pStage )
		m_pStage->Render( m_Camera );

	// render GUI components over the stage
	CGUIGameTask::Render();
}


void CStageViewerGameTask::UpdateCameraPose()
{
//	m_Camera.SetPose(  );
}


} // namespace amorphous
