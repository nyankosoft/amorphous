#include "CharacterMotionControlAppBase.hpp"

#include "amorphous/Graphics/GraphicsElementManager.hpp"
#include "amorphous/Support.hpp"
#include "amorphous/Input.hpp"
#include "amorphous/Stage.hpp"
#include "amorphous/Task.hpp"
#include "amorphous/Script.hpp"
#include "amorphous/GUI.hpp"
#include "amorphous/GameCommon/MouseCursor.hpp"
#include "amorphous/GameCommon/3DActionCode.hpp"
#include "amorphous/Item/ItemEntity.hpp"
#include "amorphous/Item/GameItem.hpp"
#include "amorphous/Physics/ActorDesc.hpp"
#include "amorphous/Physics/Enums.hpp"
#include "amorphous/App/GameWindowManager.hpp"

#include "amorphous/MotionSynthesis/MotionFSM.hpp"
#include "amorphous/Graphics/Mesh/SkeletalMesh.hpp"

#include "amorphous/Item/SkeletalCharacter.hpp"

using std::string;
using std::vector;
using namespace boost;


static string sg_TestStageScriptToLoad = "./Script/empty.bin";

/*
void CCharacterMotionInputHandler::ProcessInput( InputData& input )
{
	if( !m_pKeyBind )
		return;

	int ac = m_pKeyBind->GetActionCode( input.iGICode );

	m_pCharacter->ProcessInput( input, ac );
}
*/


ApplicationBase *amorphous::CreateApplicationInstance() { return new CCharacterMotionControlAppBase(); }

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


void ScaleAnalogInputValueRanges()
{
	DirectInputGamepad *pGamepad = GetPrimaryInputDevice<DirectInputGamepad>();
	if( !pGamepad )
		return;

	pGamepad->SetAnalogInputScale( DirectInputGamepad::AXIS_X, 2.0f );
	pGamepad->SetAnalogInputScale( DirectInputGamepad::AXIS_Y, 2.0f );
}



CCharacterMotionControlAppTask::CCharacterMotionControlAppTask()
:
m_vPrevCamPos( Vector3(0,0,0) )
{
	ScriptManager::ms_UseBoostPythonModules = true;

	m_pKeyBind.reset( new KeyBind );

	// keyboad & mouse keybinds
	m_pKeyBind->Assign( GIC_UP,              ACTION_MOV_FORWARD );
	m_pKeyBind->Assign( GIC_DOWN,            ACTION_MOV_BACKWARD );
	m_pKeyBind->Assign( GIC_RIGHT,           ACTION_MOV_TURN_R );
	m_pKeyBind->Assign( GIC_LEFT,            ACTION_MOV_TURN_L );
	m_pKeyBind->Assign( GIC_LSHIFT,          ACTION_MOV_BOOST );
	m_pKeyBind->Assign( GIC_SPACE,           ACTION_MOV_JUMP );
	m_pKeyBind->Assign( GIC_MOUSE_BUTTON_L,  ACTION_ATK_FIRE);
	m_pKeyBind->Assign( 'G',                 ACTION_ATK_FIRE);
	m_pKeyBind->Assign( GIC_MOUSE_BUTTON_R,  ACTION_CAMERA_ALIGN );
	m_pKeyBind->Assign( 'Z',                 ACTION_CAMERA_ALIGN );

	// for FPS dudes
	m_pKeyBind->Assign( 'W',                 ACTION_MOV_FORWARD );
	m_pKeyBind->Assign( 'S',                 ACTION_MOV_BACKWARD );
	m_pKeyBind->Assign( 'D',                 ACTION_MOV_TURN_R );
	m_pKeyBind->Assign( 'A',                 ACTION_MOV_TURN_L );

	// gamepad keybinds
	m_pKeyBind->Assign( GIC_GPD_BUTTON_00,   ACTION_CAMERA_ALIGN );
	m_pKeyBind->Assign( GIC_GPD_BUTTON_01,   ACTION_MOV_JUMP );

	// analog input
	// - may need to invert the fParam1.
	// - inversion is currently turned on, and is done in CCharacterMotionNodeAlgorithm::HandleInput()
//	m_pKeyBind->Assign( GIC_GPD_AXIS_Y,   ACTION_MOV_FORWARD );
	m_pKeyBind->Assign( GIC_GPD_AXIS_Y,   ACTION_MOV_BACKWARD );
	m_pKeyBind->Assign( GIC_GPD_AXIS_X,   ACTION_MOV_TURN_R );
//	m_pKeyBind->Assign( GIC_GPD_AXIS_Z,   ACTION_MOV_TURN_R );

	m_pKeyBind->Assign( GIC_MOUSE_AXIS_Y, ACTION_MOV_LOOK_UP );

	StageLoader stg_loader;
//	m_pStage = stg_loader.LoadStage( "shadow_for_directional_light.bin" );
	m_pStage = stg_loader.LoadStage( sg_TestStageScriptToLoad );

	GetCameraController()->SetPose( Matrix34( Vector3(0.8f,1.9f,-3.5f), Matrix33Identity() ) );

	m_pThirdPersonCameraController.reset( new ThirdPersonCameraController );

	m_pThirdPersonMotionController.reset( new ThirdPersonMotionController );

	// trigger shape that detects collision of the character and other objects
	physics::CCapsuleShapeDesc cap_desc;
	cap_desc.fLength = 1.00f;
	cap_desc.fRadius = 0.25f;
//	cap_desc.ShapeFlags = physics::ShapeFlag::TriggerEnable;

	// core - used to make actor desc valid
//	physics::CBoxShapeDesc core_box_desc;
//	core_box_desc.vSideLength = Vector3(1,1,1) * 0.01f;

	physics::CActorDesc actor_desc;
	actor_desc.WorldPose = Matrix34Identity();
//	actor_desc.WorldPose.vPosition = Vector3( 380.0f, 20.0f, 320.0f );
	actor_desc.WorldPose.vPosition = Vector3( 0.0f, 1.0f, 0.0f );
	actor_desc.BodyDesc.fMass = 0.1f;
	actor_desc.BodyDesc.LinearVelocity = Vector3(0,0,0);
//	actor_desc.BodyDesc.Flags = physics::PhysBodyFlag::Kinematic;
	actor_desc.BodyDesc.Flags = physics::PhysBodyFlag::DisableGravity;
	actor_desc.vecpShapeDesc.push_back( &cap_desc );
//	actor_desc.vecpShapeDesc.push_back( &core_box_desc );

	string motion_fsm_filepath = "motions/test_motion_fsm.bin";

	const int num_characters = 1;
	m_pCharacterItems.resize( num_characters );
	const char *meshes[] = { "models/male_skinny_young.msh", "models/female99-age17-muscle73-weight66-height1.52.msh" };
	for( int i=0; i<num_characters; i++ )
	{
		m_pCharacterItems[i].reset( new SkeletalCharacter );

		Result::Name res = m_pCharacterItems[i]->LoadCharacterMesh( meshes[i] );

		m_pCharacterItems[i]->InitMotionFSMs( motion_fsm_filepath );

		vector< shared_ptr<CCharacterMotionNodeAlgorithm> > pMotionNodes;
		pMotionNodes.resize( 4 );
		pMotionNodes[0].reset( new CFwdMotionNode );
		pMotionNodes[1].reset( new CRunMotionNode );
		pMotionNodes[2].reset( new CStandingMotionNode );
		pMotionNodes[3].reset( new CJumpMotionNode );

		const char *motion_names[] = { "fwd", "run", "standing", "vertical_jump" };
		for( size_t j=0; j<pMotionNodes.size(); j++ )
			m_pCharacterItems[i]->SetMotionNodeAlgorithm( motion_names[j], pMotionNodes[j] );  

/*		shared_ptr<CMeshObjectContainer> pMeshContainer;
		if( m_pCharacterItems[i]->MeshContainerRootNode().GetNumMeshContainers() == 0 )
		{
			pMeshContainer.reset( new CMeshObjectContainer );
			m_pCharacterItems[i]->MeshContainerRootNode().AddMeshContainer( pMeshContainer );
		}
		else
			pMeshContainer = m_pCharacterItems[i]->MeshContainerRootNode().GetMeshContainer( 0 );

		pMeshContainer->m_MeshDesc.ResourcePath = meshes[i];
		pMeshContainer->m_MeshDesc.MeshType = CMeshType::SKELETAL;
		bool mesh_loaded = m_pCharacterItems[i]->LoadMeshObject();*/
	}

	CItemStageUtility stg_util( m_pStage );
//	shared_ptr<SkeletalCharacter> pCharacter( new SkeletalCharacter ); // create an item
	shared_ptr<GameItem> pItem = m_pCharacterItems[0];
//	EntityHandle<ItemEntity> entity = stg_util.CreateItemEntity( pItem, Vector3(0,0,0) ); // create an entity for the item
	EntityHandle<ItemEntity> entity = stg_util.CreateItemEntity( pItem, actor_desc ); // create an entity for the item
	shared_ptr<ItemEntity> pEntity = entity.Get();
	if( pEntity )
	{
		m_pCharacterItems[0]->OnEntityCreated( *pEntity ); // set pointer of mesh render method to CCopyEntity::m_pMeshRenderMethod
		pEntity->RaiseEntityFlags( BETYPE_LIGHTING );
		pEntity->ClearEntityFlags( BETYPE_USE_PHYSSIM_RESULTS );
		pEntity->sState |= CESTATE_LIGHT_INFORMATION_INVALID;
		pEntity->InitMesh();
//		pEntity->pBaseEntity->SetMeshRenderMethod( *pEntity ); // error: cannot access protected member declared in class 'CBaseEntity'

		if( m_pThirdPersonCameraController )
			m_pThirdPersonCameraController->SetCameraPose( pEntity->GetWorldPose() );
//		m_CameraOrientation.target = m_CameraOrientation.current = Quaternion( pEntity->GetWorldPose().matOrient );
	}

	m_CharacterItemEntity = entity;

	m_pThirdPersonCameraController->SetTargetEntity( EntityHandle<>( weak_ptr<CCopyEntity>(pEntity) ) );

	// set keybind to the character item
	m_pCharacterItems[0]->SetKeyBind( m_pKeyBind );

	m_pThirdPersonMotionController->SetSkeletalCharacter( m_pCharacterItems[0] );

//	m_pInputHandler.reset( new CCharacterMotionInputHandler(pCharacter,m_pKeyBind) );
//	GetInputHub().SetInputHandler( 0, m_pInputHandler.get() );

	m_pInputHandler.reset( new CInputDataDelegate<CCharacterMotionControlAppTask>( this ) );
	if( GetInputHub().GetInputHandler(2) )
		GetInputHub().GetInputHandler(2)->AddChild( m_pInputHandler.get() );
	else
		GetInputHub().PushInputHandler( 2, m_pInputHandler.get() );

	m_ScrollEffect.SetTextureFilepath( "textures/precipitation_mid-density-512.dds" );
//	m_ScrollEffect.SetTextureFilepath( "textures/tex1024_red.bmp" );
	m_ScrollEffect.Init();

	ScaleAnalogInputValueRanges();
}


void CCharacterMotionControlAppTask::UpdateThirdPersonCamera( float dt )
{
	if( m_pCharacterItems.empty()
	 || !m_pCharacterItems[0] )
	{
		return;
	}

	if( !m_pThirdPersonCameraController )
		return;

	CInputState::Name input_state = m_pCharacterItems[0]->GetActionInputState( ACTION_CAMERA_ALIGN );

	bool close_up_camera = (input_state == CInputState::PRESSED) ? true : false;
	m_pThirdPersonCameraController->EnableCloseUpCamera( close_up_camera );

	m_pThirdPersonCameraController->Update( dt );

	SetCameraPose( m_pThirdPersonCameraController->GetCameraPose() );
}


int CCharacterMotionControlAppTask::FrameMove( float dt )
{
	int ret = StageViewerGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( IsCameraControllerEnabled() )
	{
		// let the camera controller control the camera movement
	}
	else
	{
		UpdateThirdPersonCamera( dt );

		if( m_pThirdPersonMotionController )
		{
			m_pThirdPersonMotionController->SetCameraPose( GetCamera().GetPose() );
			m_pThirdPersonMotionController->Update();
		}
	}

	m_ScrollEffect.SetCameraPose( GetCamera().GetPose() );
/*	Vector3 vDist = (Camera().GetPosition() - m_vPrevCamPos);
	if( 0.000001f < Vec3LengthSq( vDist ) )
		m_ScrollEffect.SetCameraVelocity( vDist / get_clamped( dt, 0.005f, 1.0f ) );
	else
		m_ScrollEffect.SetCameraVelocity( Vector3(0,0,0) );
	m_vPrevCamPos = Camera().GetPosition();*/
	m_ScrollEffect.Update( dt );

	return GameTask::ID_INVALID;
}


void CCharacterMotionControlAppTask::Render()
{
	StageViewerGameTask::Render();

	m_ScrollEffect.Render();
}


void CCharacterMotionControlAppTask::HandleInput( InputData& input )
{
	int action_code = m_pKeyBind ? m_pKeyBind->GetActionCode( input.iGICode ) : ACTION_NOT_ASSIGNED;

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

//	case GIC_MOUSE_BUTTON_R:
//		if( input.iType == ITYPE_KEY_PRESSED )
//		{
//			if( m_pCharacterItems.empty() )
//				return;
//
//			shared_ptr<ItemEntity> pEntity = m_pCharacterItems[0]->GetItemEntity().Get();
//			if( !pEntity )
//				return;
//
//			m_CameraOrientation.target.FromRotationMatrix( pEntity->GetWorldPose().matOrient );
//		}
//		break;

	default:
		break;
	}

	switch( action_code )
	{
	case ACTION_MOV_LOOK_UP:
		if( input.IsMouseInput() && input.iType == ITYPE_VALUE_CHANGED )
		{
			if( m_pThirdPersonCameraController )
			{
				const bool invert_mouse = true;
				const float s = invert_mouse ? -1 : 1;
				const float angle
					= m_pThirdPersonCameraController->GetTargetVerticalAngle()
					+ input.fParam1 * 0.005f * s;
				m_pThirdPersonCameraController->SetTargetVerticalAngle( angle );
			}
		}
		break;

	default:
		break;
	}

	if( m_pThirdPersonMotionController )
	{
		m_pThirdPersonMotionController->HandleInput( action_code, input );
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
//	return GameTask::ID_STAGE_VIEWER_TASK;
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
	bool base_init = GameApplicationBase::Init();
	if( !base_init )
		return false;


	//
	// Do initial settings using scripts
	//

	SRect cursor_rect = SRect( 0, 0, 27, 27 ) * ((float)GetGameWindowManager().GetScreenWidth()) / 800.0f;
	MouseCursor().Load( 0, cursor_rect, "./textures/crosshair_cursor.dds", 0xFFFFFFFF );

/*
	ScriptManager script_mgr;
	script_mgr.AddModule( "PlayerInfo",	g_PyModulePlayerMethod );
	script_mgr.LoadScriptArchiveFile( "Script/init.bin" );
	script_mgr.Update();

	//
	// Register (task name) : (task ID) maps
	//

	GameTask::AddTaskNameToTaskIDMap( "TitleFG",           CGameTaskFG::ID_TITLE_FG );
	GameTask::AddTaskNameToTaskIDMap( "MainMenuFG",        CGameTaskFG::ID_MAINMENU_FG );
	GameTask::AddTaskNameToTaskIDMap( "ControlCustomizer", CGameTaskFG::ID_CONTROLCUSTOMIZER_FG );
*/
	return true;
}
