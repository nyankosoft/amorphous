#include "PlanarReflectionAppBase.hpp"

#include "gds/Graphics/GraphicsElementManager.hpp"
#include "gds/Support.hpp"
#include "gds/Input.hpp"
#include "gds/Stage.hpp"
#include "gds/Stage/Trace.hpp"
#include "gds/Task.hpp"
#include "gds/Script.hpp"
#include "gds/GUI.hpp"
#include "gds/GameCommon/MouseCursor.hpp"
#include "gds/App/GameWindowManager.hpp"


using namespace std;
using namespace boost;


static string sg_TestStageScriptToLoad = "./Script/default.bin";


extern CApplicationBase *CreateApplicationInstance() { return new CPlanarReflectionAppBase(); }


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
	CScriptManager::ms_UseBoostPythonModules = true;

	CStageLoader stg_loader;
	m_pStage = stg_loader.LoadStage( sg_TestStageScriptToLoad );

	CameraController()->SetPose( Matrix34( Vector3(0,1.8f,0), Matrix33Identity() ) );
}


void CPlanarReflectionAppTask::HandleInput( const SInputData& input )
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
/*	int w = 1200, h = 300;
	SRect root_dlg_rect = RectLTWH( 50, 50, w, h );
	CGM_Dialog *pRootDlg
		= DialogBoxManager()->AddRootDialog(
		GUI_ID_DLG_ROOT_STAGE_SELECT,
		root_dlg_rect,
		"shadows test",
		CGM_Dialog::STYLE_ALWAYS_OPEN
		);

	SRect lbx_rect = RectLTWH( 10, 10, w - 20, h - 20 );

	CGM_ListBox *pStageSelectListBox
		= pRootDlg->AddListBox( GUI_ID_LBX_STAGE_SELECT, lbx_rect, "", 0, 40 );

	DialogBoxManager()->OpenRootDialog( GUI_ID_DLG_ROOT_STAGE_SELECT );

	pStageSelectListBox->InsertItem( 0, "directional light", NULL );
	pStageSelectListBox->InsertItem( 1, "point light", NULL );
	pStageSelectListBox->InsertItem( 2, "spotlight", NULL );

	shared_ptr<CGM_ListBoxEventHandler> pEventHandler( new StageSelectListBoxEventHandler(this) );
	pStageSelectListBox->SetEventHandler( pEventHandler );

	shared_ptr<CGraphicsElementManager> pGraphicsElemetMgr
		= GetGUIRendererManager()->GetGraphicsElementManager();

	pGraphicsElemetMgr->LoadFont( 0, "./Fonts/mono966_rld_b.TTF", 24, 48 );
*/
}


void CPlanarReflectionAppGUITask::LoadStage( const std::string& stage_script_name )
{
	m_StageScriptToLoad = stage_script_name;
}


int CPlanarReflectionAppGUITask::FrameMove( float dt )
{
	int ret = CGUIGameTask::FrameMove(dt);
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
//	Release();
}

/*
void CPlanarReflectionAppBase::Release()
{
}*/


const std::string CPlanarReflectionAppBase::GetStartTaskName() const
{
	return string("");
}


int CPlanarReflectionAppBase::GetStartTaskID() const
{
//	return CGameTask::ID_STAGE_VIEWER_TASK;
	return GAMETASK_ID_BASIC_PHYSICS;
}


bool CPlanarReflectionAppBase::Init()
{
	//
	// Register base entity factory
	//
/*
	BaseEntityManager().RegisterBaseEntityFactory( CBaseEntityFactorySharedPtr( new CFG_BaseEntityFactory() ) );

	BaseEntityManager().AddBaseEntityClassNameToIDMap( "CFG_AIAircraftBaseEntity", CFG_BaseEntityID::BE_AIAIRCRAFTBASEENTITY );
*/
	bool base_init = CGameApplicationBase::Init();
	if( !base_init )
		return false;


	//
	// Do initial settings using scripts
	//

	SRect cursor_rect = SRect( 0, 0, 27, 27 ) * ((float)GameWindowManager().GetScreenWidth()) / 800.0f;
	MouseCursor().Load( 0, cursor_rect, "./textures/crosshair_cursor.dds", 0xFFFFFFFF );

/*
	//
	// Register (task name) : (task ID) maps
	//

	CGameTask::AddTaskNameToTaskIDMap( "TitleFG",           CGameTaskFG::ID_TITLE_FG );
	CGameTask::AddTaskNameToTaskIDMap( "MainMenuFG",        CGameTaskFG::ID_MAINMENU_FG );
	CGameTask::AddTaskNameToTaskIDMap( "ControlCustomizer", CGameTaskFG::ID_CONTROLCUSTOMIZER_FG );
*/
	return true;
}
