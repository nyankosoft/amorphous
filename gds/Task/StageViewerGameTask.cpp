#include "StageViewerGameTask.hpp"

#include "Support/Log/DefaultLog.hpp"
#include "Graphics/GraphicsElementManager.hpp"
#include "Input/InputHub.hpp"
#include "Stage.hpp"
#include "GUI.hpp"

using namespace std;
using namespace boost;


class CGameTaskDesc
{
public:
	std::string ScriptName;
	int InputHandlerIndex;
};


CStageViewerGameTask::CStageViewerGameTask()
{
	const int input_handler_index = 1;
	m_pCameraController = shared_ptr<CCameraController>( new CCameraController( input_handler_index ) );
}


CStageViewerGameTask::~CStageViewerGameTask()
{
}


int CStageViewerGameTask::FrameMove( float dt )
{
	int ret = CSingleStageGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	UpdateCameraPose();

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
