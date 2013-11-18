#include "PlanarReflectionAppBase.hpp"

#include "amorphous/Graphics/GraphicsElementManager.hpp"
#include "amorphous/Support.hpp"
#include "amorphous/Input.hpp"
#include "amorphous/Stage.hpp"
#include "amorphous/Stage/Trace.hpp"
#include "amorphous/Task.hpp"
#include "amorphous/Script.hpp"
#include "amorphous/GUI.hpp"


using namespace std;
using namespace boost;


static string sg_TestStageScriptToLoad = "./Script/default.bin";


extern ApplicationBase *amorphous::CreateApplicationInstance() { return new CPlanarReflectionAppBase(); }


class StageSelectListBoxEventHandler : public CGM_ListBoxEventHandler
{
	CPlanarReflectionAppGUITask *m_pTask;

public:

	StageSelectListBoxEventHandler( CPlanarReflectionAppGUITask *pTask )
		:
	m_pTask(pTask)
	{
	}

	void OnItemSelected( CGM_ListBoxItem& item, int item_index )
	{
		switch( item_index )
		{
		case 0:
			m_pTask->LoadStage( "./Script/???.bin" );
			break;
		default:
			break;
		}

	}
};


CPlanarReflectionAppTask::CPlanarReflectionAppTask()
{
	ScriptManager::ms_UseBoostPythonModules = true;

	StageLoader stg_loader;
	m_pStage = stg_loader.LoadStage( sg_TestStageScriptToLoad );

	GetCameraController()->SetPose( Matrix34( Vector3(0,1.8f,0), Matrix33Identity() ) );
}


void CPlanarReflectionAppTask::HandleInput( const InputData& input )
{
	switch( input.iGICode )
    {
	case GIC_MOUSE_BUTTON_L:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
//			OnTriggerPulled();
		}
		break;
	default:
		break;
	}
}



//======================================================================
// CPlanarReflectionAppGUITask
//======================================================================

CPlanarReflectionAppGUITask::CPlanarReflectionAppGUITask()
{
}


void CPlanarReflectionAppGUITask::LoadStage( const std::string& stage_script_name )
{
	m_StageScriptToLoad = stage_script_name;
}


int CPlanarReflectionAppGUITask::FrameMove( float dt )
{
	int ret = GUIGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;
/*
	if( 0 < m_StageScriptToLoad.length() )
	{
		sg_TestStageScriptToLoad = m_StageScriptToLoad;
		return GAMETASK_ID_SHADOWS_TEST_STAGE;
	}
*/
	return ID_INVALID;
}



//========================================================================================
// CPlanarReflectionAppBase
//========================================================================================

CPlanarReflectionAppBase::CPlanarReflectionAppBase()
{
}


CPlanarReflectionAppBase::~CPlanarReflectionAppBase()
{
}


int CPlanarReflectionAppBase::GetStartTaskID() const
{
	return GAMETASK_ID_PLANAR_REFLECTION_DEMO;
}
