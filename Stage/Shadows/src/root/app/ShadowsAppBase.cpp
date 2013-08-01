#include "ShadowsAppBase.hpp"

#include "amorphous/Graphics/GraphicsElementManager.hpp"
#include "amorphous/Support.hpp"
#include "amorphous/Input.hpp"
#include "amorphous/Stage.hpp"
#include "amorphous/Task.hpp"
#include "amorphous/Script.hpp"
#include "amorphous/GUI.hpp"
#include "amorphous/GameCommon/MouseCursor.hpp"
#include "amorphous/App/GameWindowManager_Win32.hpp"


using std::string;
using std::vector;
using boost::shared_ptr;


static string sg_TestStageScriptToLoad;


extern CApplicationBase *amorphous::CreateApplicationInstance() { return new CShadowsAppBase(); }


class StageSelectListBoxEventHandler : public CGM_ListBoxEventHandler
{
	CShadowAppStageSelectTask *m_pTask;

public:

	StageSelectListBoxEventHandler( CShadowAppStageSelectTask *pTask )
		:
	m_pTask(pTask)
	{
	}

	void OnItemSelected( CGM_ListBoxItem& item, int item_index )
	{
		switch( item_index )
		{
		case 0:
			m_pTask->LoadStage( "./Script/shadow_for_directional_light.bin" );
			break;
		case 1:
			m_pTask->LoadStage( "./Script/shadow_for_directional_light-mesh.bin" );
			break;
		default:
			break;
		}

	}
};


CShadowAppTask::CShadowAppTask()
{
	ScriptManager::ms_UseBoostPythonModules = true;

	StageLoader stg_loader;
//	m_pStage = stg_loader.LoadStage( "shadow_for_directional_light.bin" );
	m_pStage = stg_loader.LoadStage( sg_TestStageScriptToLoad );
}


CShadowAppStageSelectTask::CShadowAppStageSelectTask()
{
	int w = 1200, h = 300;
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
	pStageSelectListBox->InsertItem( 1, "directional light (on mesh)", NULL );
	pStageSelectListBox->InsertItem( 2, "point light", NULL );
	pStageSelectListBox->InsertItem( 3, "spotlight", NULL );

	shared_ptr<CGM_ListBoxEventHandler> pEventHandler( new StageSelectListBoxEventHandler(this) );
	pStageSelectListBox->SetEventHandler( pEventHandler );

	shared_ptr<GraphicsElementManager> pGraphicsElemetMgr
		= GetGUIRendererManager()->GetGraphicsElementManager();

	pGraphicsElemetMgr->LoadFont( 0, "./Fonts/mono966_rld_b.TTF", 24, 48 );
}


void CShadowAppStageSelectTask::LoadStage( const std::string& stage_script_name )
{
	m_StageScriptToLoad = stage_script_name;
}


int CShadowAppStageSelectTask::FrameMove( float dt )
{
	int ret = GUIGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( 0 < m_StageScriptToLoad.length() )
	{
		sg_TestStageScriptToLoad = m_StageScriptToLoad;
		return GAMETASK_ID_SHADOWS_TEST_STAGE;
	}

	return ID_INVALID;
}




//========================================================================================
// CShadowsAppBase
//========================================================================================

CShadowsAppBase::CShadowsAppBase()
{
}


CShadowsAppBase::~CShadowsAppBase()
{
//	Release();
}

/*
void CShadowsAppBase::Release()
{
}*/


const std::string CShadowsAppBase::GetStartTaskName() const
{
	return string("");
}


int CShadowsAppBase::GetStartTaskID() const
{
//	return CGameTask::ID_STAGE_VIEWER_TASK;
	return GAMETASK_ID_SHADOWS_STAGE_SELECT;
}


bool CShadowsAppBase::Init()
{
	//
	// Register base entity factory
	//
/*
	GetBaseEntityManager().RegisterBaseEntityFactory( BaseEntityFactorySharedPtr( new CFG_BaseEntityFactory() ) );

	GetBaseEntityManager().AddBaseEntityClassNameToIDMap( "CFG_AIAircraftBaseEntity", CFG_BaseEntityID::BE_AIAIRCRAFTBASEENTITY );
*/
	bool base_init = CGameApplicationBase::Init();
	if( !base_init )
		return false;


	//
	// Do initial settings using scripts
	//
/*
	ScriptManager script_mgr;
	script_mgr.AddModule( "PlayerInfo",	g_PyModulePlayerMethod );
	script_mgr.AddModule( "Shop",		g_PyModuleShopMethod );
	script_mgr.AddModule( "StageGraph",	g_PyModuleStageGraphMethod );
	script_mgr.LoadScriptArchiveFile( "Script/init.bin" );
	script_mgr.Update();

	SRect cursor_rect = SRect( 0, 0, 27, 27 ) * ((float)GameWindowManager().GetScreenWidth()) / 800.0f;
	MouseCursor().Load( 0, cursor_rect, "./Texture/cursor.dds", 0xFFFFFFFF );

	//
	// Register (task name) : (task ID) maps
	//

	CGameTask::AddTaskNameToTaskIDMap( "TitleFG",           CGameTaskFG::ID_TITLE_FG );
	CGameTask::AddTaskNameToTaskIDMap( "MainMenuFG",        CGameTaskFG::ID_MAINMENU_FG );
	CGameTask::AddTaskNameToTaskIDMap( "ControlCustomizer", CGameTaskFG::ID_CONTROLCUSTOMIZER_FG );
*/
	return true;
}
