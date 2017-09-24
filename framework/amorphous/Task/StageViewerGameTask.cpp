#include "StageViewerGameTask.hpp"

#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Graphics/GraphicsElementManager.hpp"
#include "amorphous/Input/InputHub.hpp"
#include "amorphous/Stage.hpp"
#include "amorphous/GUI.hpp"


namespace amorphous
{

using namespace std;
using namespace boost;


class GameTaskDesc
{
public:
	std::string ScriptName;
	int InputHandlerIndex;
};


StageViewerGameTask::StageViewerGameTask()
:
m_EnableCameraController(true)
{
	// An input handler for camera control is added as a child of m_pInputHandler.
	m_pCameraController.reset( new CameraController( m_pInputHandler ) );

	m_Camera.SetAspectRatio( (float)GraphicsComponent::GetScreenWidth() / (float)GraphicsComponent::GetScreenHeight() );
}


StageViewerGameTask::~StageViewerGameTask()
{
}


int StageViewerGameTask::FrameMove( float dt )
{
	int ret = SingleStageGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

//	UpdateCameraPose();

	if( m_pCameraController && m_EnableCameraController )
	{
		m_pCameraController->UpdateCameraPose( dt );
		m_Camera.SetPose( m_pCameraController->GetPose() );
		m_Camera.UpdateVFTreeForWorldSpace();
	}

	return GameTask::ID_INVALID;
}


void StageViewerGameTask::Render()
{
	// render stage
	if( m_pStage )
		m_pStage->Render( m_Camera );

	// render GUI components over the stage
	GUIGameTask::Render();
}


void StageViewerGameTask::UpdateCameraPose()
{
//	m_Camera.SetPose(  );
}


} // namespace amorphous
