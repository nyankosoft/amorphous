#include "CharacterMotionControlAppBase.hpp"

#include <gds/Graphics/GraphicsElementManager.hpp>
#include <gds/Support.hpp>
#include <gds/Input.hpp>
#include <gds/Stage.hpp>
#include <gds/Task.hpp>
#include <gds/Script.hpp>
#include <gds/GUI.hpp>
#include <gds/GameCommon/MouseCursor.hpp>
#include <gds/GameCommon/3DActionCode.hpp>
#include <gds/Item/ItemEntity.hpp>
#include <gds/App/GameWindowManager_Win32.hpp>

#include <gds/Item/GameItem.hpp>
#include <gds/MotionSynthesis/MotionFSM.hpp>
#include <gds/Graphics/Mesh/SkeletalMesh.hpp>

#include "../item/SkeletalCharacter.hpp"

using namespace std;
using namespace boost;


static string sg_TestStageScriptToLoad = "./Script/empty.bin";

/*
void CCharacterMotionInputHandler::ProcessInput( SInputData& input )
{
	if( !m_pKeyBind )
		return;

	int ac = m_pKeyBind->GetActionCode( input.iGICode );

	m_pCharacter->ProcessInput( input, ac );
}
*/


void CCharacterEntity::Draw()
{
}


CApplicationBase *CreateApplicationInstance() { return new CCharacterMotionControlAppBase(); }

/*
class StageSelectListBoxEventHandler : public CGM_ListBoxEventHandler
{
	CCharacterMotionControlAppGUITask *m_pTask;
public:
	StageSelectListBoxEventHandler( CCharacterMotionControlAppGUITask *pTask )
		:
	m_pTask(pTask)
	{}
	void OnItemSelected( CGM_ListBoxItem& item, int item_index )
	{
		switch( item_index )
		{
		case 0:
//			m_pTask->LoadStage( "./Script/???.bin" );
			break;
		default:
			break;
		}
	}
};
*/

CCharacterMotionControlAppTask::CCharacterMotionControlAppTask()
:
m_vPrevCamPos( Vector3(0,0,0) )
{
	m_pKeyBind = shared_ptr<CKeyBind>( new CKeyBind );

	m_pKeyBind->Assign( GIC_UP,    ACTION_MOV_FORWARD );
	m_pKeyBind->Assign( GIC_DOWN,  ACTION_MOV_BACKWARD );
	m_pKeyBind->Assign( GIC_RIGHT, ACTION_MOV_TURN_R );
	m_pKeyBind->Assign( GIC_LEFT,  ACTION_MOV_TURN_L );

	// analog input
	// - may need to invert the fParam1.
	// - inversion is currently turned on, and is done in CCharacterMotionNodeAlgorithm::HandleInput()
	m_pKeyBind->Assign( GIC_GPD_AXIS_Y,  ACTION_MOV_FORWARD );
	m_pKeyBind->Assign( GIC_GPD_AXIS_X,  ACTION_MOV_TURN_R );
//	m_pKeyBind->Assign( GIC_GPD_AXIS_Z,  ACTION_MOV_TURN_R );

	CScriptManager::ms_UseBoostPythonModules = true;

	CStageLoader stg_loader;
//	m_pStage = stg_loader.LoadStage( "shadow_for_directional_light.bin" );
	m_pStage = stg_loader.LoadStage( sg_TestStageScriptToLoad );

	CameraController()->SetPose( Matrix34( Vector3(0.8f,1.9f,-3.5f), Matrix33Identity() ) );

	CItemStageUtility stg_util( m_pStage );
	shared_ptr<CSkeletalCharacter> pCharacter( new CSkeletalCharacter ); // create an item
	shared_ptr<CGameItem> pItem = pCharacter;
	CEntityHandle<CItemEntity> entity = stg_util.CreateItemEntity( pItem, Vector3(0,0,0) ); // create an entity for the item
	shared_ptr<CItemEntity> pEntity = entity.Get();
	if( pEntity )
	{
		pCharacter->OnEntityCreated( *pEntity ); // set pointer of mesh render method to CCopyEntity::m_pMeshRenderMethod
		pEntity->RaiseEntityFlags( BETYPE_LIGHTING );
		pEntity->sState |= CESTATE_LIGHT_INFORMATION_INVALID;
		pEntity->InitMesh();
//		pEntity->pBaseEntity->SetMeshRenderMethod( *pEntity ); // error: cannot access protected member declared in class 'CBaseEntity'
	}

	m_CharacterItemEntity = entity;

	// set keybind to the character item
	pCharacter->SetKeyBind( m_pKeyBind );

//	m_pInputHandler.reset( new CCharacterMotionInputHandler(pCharacter,m_pKeyBind) );
//	InputHub().SetInputHandler( 0, m_pInputHandler.get() );

	m_pInputHandler.reset( new CDelegateInputHandler<CCharacterMotionControlAppTask>( this ) );
	if( InputHub().GetInputHandler(2) )
		InputHub().GetInputHandler(2)->AddChild( m_pInputHandler.get() );
	else
		InputHub().PushInputHandler( 2, m_pInputHandler.get() );

	m_ScrollEffect.SetTextureFilepath( "textures/precipitation_mid-density-512.dds" );
//	m_ScrollEffect.SetTextureFilepath( "textures/tex1024_red.bmp" );
	m_ScrollEffect.Init();
}


int CCharacterMotionControlAppTask::FrameMove( float dt )
{
	int ret = CStageViewerGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( IsCameraControllerEnabled() )
	{
		// let the camera controller control the camera movement
	}
	else
	{
		shared_ptr<CItemEntity> pEntity = m_CharacterItemEntity.Get();
		if( pEntity )
		{
			Vector3 vPos = pEntity->GetWorldPose().vPosition + Vector3(0.0f, 2.0f, -3.8f);
			vPos.y = 2.0f;
			Camera().SetPose( Matrix34( vPos, Matrix33Identity() ) );
		}
	}

	m_ScrollEffect.SetCameraPose( Camera().GetPose() );
/*	Vector3 vDist = (Camera().GetPosition() - m_vPrevCamPos);
	if( 0.000001f < Vec3LengthSq( vDist ) )
		m_ScrollEffect.SetCameraVelocity( vDist / get_clamped( dt, 0.005f, 1.0f ) );
	else
		m_ScrollEffect.SetCameraVelocity( Vector3(0,0,0) );
	m_vPrevCamPos = Camera().GetPosition();*/
	m_ScrollEffect.Update( dt );

	return CGameTask::ID_INVALID;
}


void CCharacterMotionControlAppTask::Render()
{
	CStageViewerGameTask::Render();

	m_ScrollEffect.Render();
}


void CCharacterMotionControlAppTask::HandleInput( SInputData& input )
{
	switch( input.iGICode )
	{
	case '1':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			// toggle camera control
			bool enabled = IsCameraControllerEnabled();
			EnableCameraController( !enabled );
		}
		break;
	default:
		break;
	}
}


CCharacterMotionControlAppGUITask::CCharacterMotionControlAppGUITask()
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

/*
void CCharacterMotionControlAppGUITask::LoadStage( const std::string& stage_script_name )
{
	m_StageScriptToLoad = stage_script_name;
}
*/

int CCharacterMotionControlAppGUITask::FrameMove( float dt )
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
// CCharacterMotionControlAppBase
//========================================================================================

CCharacterMotionControlAppBase::CCharacterMotionControlAppBase()
{
}


CCharacterMotionControlAppBase::~CCharacterMotionControlAppBase()
{
//	Release();
}

/*
void CCharacterMotionControlAppBase::Release()
{
}*/


const std::string CCharacterMotionControlAppBase::GetStartTaskName() const
{
	return string("");
}


int CCharacterMotionControlAppBase::GetStartTaskID() const
{
//	return CGameTask::ID_STAGE_VIEWER_TASK;
	return GAMETASK_ID_BASIC_PHYSICS;
}


bool CCharacterMotionControlAppBase::Init()
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
	CScriptManager script_mgr;
	script_mgr.AddModule( "PlayerInfo",	g_PyModulePlayerMethod );
	script_mgr.LoadScriptArchiveFile( "Script/init.bin" );
	script_mgr.Update();

	//
	// Register (task name) : (task ID) maps
	//

	CGameTask::AddTaskNameToTaskIDMap( "TitleFG",           CGameTaskFG::ID_TITLE_FG );
	CGameTask::AddTaskNameToTaskIDMap( "MainMenuFG",        CGameTaskFG::ID_MAINMENU_FG );
	CGameTask::AddTaskNameToTaskIDMap( "ControlCustomizer", CGameTaskFG::ID_CONTROLCUSTOMIZER_FG );
*/
	return true;
}
