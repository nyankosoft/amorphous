#include "BillboardAnimationAppBase.hpp"

#include "amorphous/Graphics/GraphicsElementManager.hpp"
#include "amorphous/Support.hpp"
#include "amorphous/Input.hpp"
#include "amorphous/Stage.hpp"
#include "amorphous/Stage/Trace.hpp"
#include "amorphous/Task.hpp"
#include "amorphous/Script.hpp"
#include "amorphous/GUI.hpp"
//#include "amorphous/GameCommon/MouseCursor.hpp"
//#include "amorphous/App/GameWindowManager.hpp"


using namespace std;


static string sg_TestStageScriptToLoad = "./Script/explosion.bin";


extern ApplicationBase *amorphous::CreateApplicationInstance() { return new BillboardAnimationAppBase(); }


class StageSelectListBoxEventHandler : public CGM_ListBoxEventHandler
{
	BillboardAnimationAppGUITask *m_pTask;

public:

	StageSelectListBoxEventHandler( BillboardAnimationAppGUITask *pTask )
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


BillboardAnimationAppTask::BillboardAnimationAppTask()
{
	ScriptManager::ms_UseBoostPythonModules = true;

	StageLoader stg_loader;
//	m_pStage = stg_loader.LoadStage( "shadow_for_directional_light.bin" );
	m_pStage = stg_loader.LoadStage( sg_TestStageScriptToLoad );

	GetCameraController()->SetPose( Matrix34( Vector3(0,20,-15), Matrix33Identity() ) );
}


void BillboardAnimationAppTask::OnTriggerPulled()
{
	if( !m_pStage )
		return;

	// raycast
	Matrix34 cam_pose = GetCameraController()->GetPose();
	Vector3 vStart = cam_pose.vPosition;
	Vector3 vGoal  = vStart + cam_pose.matOrient.GetColumn(2) * 10.0f;
	STrace tr;
	tr.vStart = vStart;
	tr.vGoal  = vGoal;
	tr.bvType = BVTYPE_DOT;

	m_pStage->ClipTrace( tr );

	// create explosion animation at a position where ray hit
	const char *base_entity_name = "imex";
//	const char *base_entity_name = "imex_L";
	CCopyEntityDesc desc;
	BaseEntityHandle handle( base_entity_name );
	desc.pBaseEntityHandle = &handle;
	desc.WorldPose.vPosition = tr.vEnd;
	desc.WorldPose.matOrient = Matrix33Identity();

	m_pStage->CreateEntity( desc );
}


void BillboardAnimationAppTask::HandleInput( const InputData& input )
{
	switch( input.iGICode )
    {
	case GIC_MOUSE_BUTTON_L:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			OnTriggerPulled();
		}
		break;
	default:
		break;
	}
}



//======================================================================
// BillboardAnimationAppGUITask
//======================================================================

BillboardAnimationAppGUITask::BillboardAnimationAppGUITask()
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


void BillboardAnimationAppGUITask::LoadStage( const std::string& stage_script_name )
{
	m_StageScriptToLoad = stage_script_name;
}


int BillboardAnimationAppGUITask::FrameMove( float dt )
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
// BillboardAnimationAppBase
//========================================================================================

BillboardAnimationAppBase::BillboardAnimationAppBase()
{
}


BillboardAnimationAppBase::~BillboardAnimationAppBase()
{
//	Release();
}

/*
void BillboardAnimationAppBase::Release()
{
}*/


const std::string BillboardAnimationAppBase::GetStartTaskName() const
{
	return string("");
}


int BillboardAnimationAppBase::GetStartTaskID() const
{
	return GAMETASK_ID_BILLBOARD_ANIMATION;
}


//bool BillboardAnimationAppBase::Init()
//{
//	bool base_init = GameApplicationBase::Init();
//	if( !base_init )
//		return false;
//
//	SRect cursor_rect = SRect( 0, 0, 27, 27 ) * ((float)GetGameWindowManager().GetScreenWidth()) / 800.0f;
//	GetMouseCursor().Load( 0, cursor_rect, "./textures/crosshair_cursor.dds", 0xFFFFFFFF );
//
//	return true;
//}
