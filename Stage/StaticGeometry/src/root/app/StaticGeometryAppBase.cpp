#include "StaticGeometryAppBase.hpp"

#include "gds/Graphics/GraphicsElementManager.hpp"
#include "gds/Support.hpp"
#include "gds/Input.hpp"
#include "gds/Stage.hpp"
#include "gds/Task.hpp"
#include "gds/Script.hpp"
#include "gds/GUI.hpp"
#include "gds/GameCommon/MouseCursor.hpp"
#include "gds/App/GameWindowManager_Win32.hpp"


using namespace std;
using namespace boost;


static string sg_TestStageScriptToLoad = "./Script/hills.bin";


extern CApplicationBase *CreateApplicationInstance() { return new CStaticGeometryAppBase(); }


class StageSelectListBoxEventHandler : public CGM_ListBoxEventHandler
{
	CStaticGeometryAppGUITask *m_pTask;

public:

	StageSelectListBoxEventHandler( CStaticGeometryAppGUITask *pTask )
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


CStaticGeometryAppTask::CStaticGeometryAppTask()
{
	CScriptManager::ms_UseBoostPythonModules = true;

	CStageLoader stg_loader;
	LoadParamFromFile( "debug/params.txt", "script", sg_TestStageScriptToLoad );
	m_pStage = stg_loader.LoadStage( sg_TestStageScriptToLoad );

	CameraController()->SetPose( Matrix34( Vector3(0,320,-15), Matrix33Identity() ) );
	CameraController()->SetTranslationSpeed( 50.0f );
	Camera().SetFarClip( 5000.0f );
}


CStaticGeometryAppGUITask::CStaticGeometryAppGUITask()
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


void CStaticGeometryAppGUITask::LoadStage( const std::string& stage_script_name )
{
	m_StageScriptToLoad = stage_script_name;
}


int CStaticGeometryAppGUITask::FrameMove( float dt )
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
// CStaticGeometryAppBase
//========================================================================================

CStaticGeometryAppBase::CStaticGeometryAppBase()
{
}


CStaticGeometryAppBase::~CStaticGeometryAppBase()
{
//	Release();
}

/*
void CStaticGeometryAppBase::Release()
{
}*/


const std::string CStaticGeometryAppBase::GetStartTaskName() const
{
	return string("");
}


int CStaticGeometryAppBase::GetStartTaskID() const
{
//	return CGameTask::ID_STAGE_VIEWER_TASK;
	return GAMETASK_ID_STATIC_GEOMETRY_STAGE_VIEWER;
}


bool CStaticGeometryAppBase::Init()
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

	return true;
}
