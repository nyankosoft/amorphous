#include "FirearmsAppBase.hpp"

#include "amorphous/Graphics/GraphicsElementManager.hpp"
#include "amorphous/Input.hpp"
#include "amorphous/Stage.hpp"
#include "amorphous/Task.hpp"
#include "amorphous/Script.hpp"
#include "amorphous/GUI.hpp"
#include "amorphous/GameCommon/MouseCursor.hpp"
#include "amorphous/GameCommon/3DActionCode.hpp"
#include "amorphous/GameCommon/ThirdPersonCameraController.hpp"
#include "amorphous/GameCommon/ThirdPersonMotionController.hpp"
#include "amorphous/Item/ItemEntity.hpp"
#include "amorphous/Item/GameItem.hpp"
#include "amorphous/Item/Cartridge.hpp"
#include "amorphous/Item/Magazine.hpp"
#include "amorphous/Item/Firearm.hpp"
#include "amorphous/Item/ItemDatabaseManager.hpp"
#include "amorphous/Item/ItemStatusDebugDisplay.hpp"
#include "amorphous/Physics/ActorDesc.hpp"
#include "amorphous/Physics/Enums.hpp"
#include "amorphous/App/GameWindowManager.hpp"
#include "amorphous/Support/ParamLoader.hpp"

#include "amorphous/MotionSynthesis/MotionFSM.hpp"
#include "amorphous/Graphics/Mesh/SkeletalMesh.hpp"

#include "../item/ArmedCharacterOperations.hpp"

#include <boost/filesystem.hpp>

using std::string;
using std::vector;
using namespace boost;
using namespace amorphous;


static string sg_TestStageScriptToLoad = "./Script/default.bin";


ApplicationBase *amorphous::CreateApplicationInstance() { return new FirearmsAppBase(); }


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


void FirearmsAppTask::ArmCharacter( SkeletalCharacter& character )
{
	CItemStageUtility stg_util( m_pStage );

	shared_ptr<Firearm> pUSP = ItemDatabaseManager().GetItem<Firearm>( "USP (9mm)", 1 );
	if( !pUSP )
		return;

	shared_ptr<Cartridge> pCartridges = ItemDatabaseManager().GetItem<Cartridge>( "9x19mm (FMJ)", 1000 );
	if( !pCartridges )
		return;

	shared_ptr<Magazine> pMag = ItemDatabaseManager().GetItem<Magazine>( "USP Magazine (9mm, 15)", 1 );
	if( !pMag )
		return;

	DebugOutput.AddDebugItem( "firearm", new CItemStatusDebugDisplay(pUSP) );


	// Need to create separate instances for magazines

	stg_util.CreateItemEntity( pUSP, Vector3(0,0,0) );
	stg_util.CreateItemEntity( pCartridges, Vector3(0,0,0) );
	stg_util.CreateItemEntity( pMag, Vector3(0,0,0) );

	character.AddItem( pUSP );
	character.AddItem( pCartridges );
	character.AddItem( pMag );
}


void FirearmsAppTask::InitKeybind( KeyBind& keybind )
{
	// keyboad & mouse keybinds
	keybind.Assign( GIC_UP,              ACTION_MOV_FORWARD );
	keybind.Assign( GIC_DOWN,            ACTION_MOV_BACKWARD );
	keybind.Assign( GIC_RIGHT,           ACTION_MOV_TURN_R );
	keybind.Assign( GIC_LEFT,            ACTION_MOV_TURN_L );
	keybind.Assign( GIC_LSHIFT,          ACTION_MOV_BOOST );
	keybind.Assign( GIC_SPACE,           ACTION_MOV_JUMP );
	keybind.Assign( GIC_MOUSE_BUTTON_L,  ACTION_ATK_FIRE);
	keybind.Assign( 'G',                 ACTION_ATK_FIRE);
	keybind.Assign( GIC_MOUSE_BUTTON_R,  ACTION_CAMERA_ALIGN );
	keybind.Assign( 'Z',                 ACTION_CAMERA_ALIGN );

	// for FPS dudes
	keybind.Assign( 'W',                 ACTION_MOV_FORWARD );
	keybind.Assign( 'S',                 ACTION_MOV_BACKWARD );
	keybind.Assign( 'D',                 ACTION_MOV_TURN_R );
	keybind.Assign( 'A',                 ACTION_MOV_TURN_L );

	// gamepad keybinds
	keybind.Assign( GIC_GPD_BUTTON_00,   ACTION_CAMERA_ALIGN );
	keybind.Assign( GIC_GPD_BUTTON_01,   ACTION_MOV_JUMP );

	// analog input
	// - may need to invert the fParam1.
	// - inversion is currently turned on, and is done in CCharacterMotionNodeAlgorithm::HandleInput()
//	keybind.Assign( GIC_GPD_AXIS_Y,   ACTION_MOV_FORWARD );
	keybind.Assign( GIC_GPD_AXIS_Y,   ACTION_MOV_BACKWARD );
	keybind.Assign( GIC_GPD_AXIS_X,   ACTION_MOV_TURN_R );
//	keybind.Assign( GIC_GPD_AXIS_Z,   ACTION_MOV_TURN_R );

	keybind.Assign( GIC_MOUSE_AXIS_Y, ACTION_MOV_LOOK_UP );
}


FirearmsAppTask::FirearmsAppTask()
:
m_vPrevCamPos( Vector3(0,0,0) )
{
	ScriptManager::ms_UseBoostPythonModules = true;

	m_pKeyBind.reset( new KeyBind );

	InitKeybind( *m_pKeyBind );

	StageLoader stg_loader;
//	m_pStage = stg_loader.LoadStage( "shadow_for_directional_light.bin" );
	m_pStage = stg_loader.LoadStage( sg_TestStageScriptToLoad );

	SetCameraPose( Matrix34( Vector3(0.8f,1.9f,-3.5f), Matrix33Identity() ) );

	m_pThirdPersonCameraController.reset( new ThirdPersonCameraController );

	m_pThirdPersonMotionController.reset( new ThirdPersonMotionController );

	// trigger shape that detects collision of the character and other objects
	physics::CCapsuleShapeDesc cap_desc;
	cap_desc.fLength = 1.00f;
	cap_desc.fRadius = 0.25f;
	LoadParamFromFile( ".debug/skeletal_character.txt", "cap_length", cap_desc.fLength );
	LoadParamFromFile( ".debug/skeletal_character.txt", "cap_radius", cap_desc.fRadius );
//	cap_desc.ShapeFlags = physics::ShapeFlag::TriggerEnable;

	// core - used to make actor desc valid
//	physics::CBoxShapeDesc core_box_desc;
//	core_box_desc.vSideLength = Vector3(1,1,1) * 0.01f;

	// add sphere detect collision with 1x1x1 boxes
	physics::CSphereShapeDesc sphere_desc;
	sphere_desc.Radius = cap_desc.fRadius + 0.05f;

	physics::CActorDesc actor_desc;
	actor_desc.WorldPose = Matrix34Identity();
	actor_desc.WorldPose.vPosition = Vector3( 380.0f, 20.0f, 320.0f );
	actor_desc.BodyDesc.fMass = 0.1f;
	actor_desc.BodyDesc.LinearVelocity = Vector3(0,0,0);
//	actor_desc.BodyDesc.Flags = physics::PhysBodyFlag::Kinematic;
	actor_desc.BodyDesc.Flags = physics::PhysBodyFlag::DisableGravity;
	actor_desc.vecpShapeDesc.push_back( &cap_desc );
//	actor_desc.vecpShapeDesc.push_back( &core_box_desc );
	actor_desc.vecpShapeDesc.push_back( &sphere_desc );

	boost::shared_ptr<msynth::MotionFSMManager> pMotionFSMManager;
	pMotionFSMManager.reset( new msynth::MotionFSMManager );

	string motion_fsm_filepath = "motions/test_motion_fsm.bin";

	// Load FSM from XML file and update the binary file
	string xml_file = "../resources/misc/test_motion_fsm.xml";
	if( filesystem::exists(xml_file) )
	{
		pMotionFSMManager->LoadFromXMLFile( xml_file );
		bool saved = pMotionFSMManager->SaveToFile( motion_fsm_filepath );
		if( !saved )
			LOG_PRINT_WARNING( " pMotionFSMManager->SaveToFile() failed." );
	}

	// Clear the data loaded from XML
	pMotionFSMManager.reset();

	const int num_characters = 1;
	m_pCharacterItems.resize( num_characters );
	const char *meshes[] = { "models/characters/male_skinny_young.msh", "models/characters/female99-age17-muscle73-weight66-height1.52.msh" };
	for( int i=0; i<num_characters; i++ )
	{
		m_pCharacterItems[i] = CreateGameItem<SkeletalCharacter>();

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

		m_pCharacterItems[i]->InitClothSystem();


/*		shared_ptr<MeshObjectContainer> pMeshContainer;
		if( m_pCharacterItems[i]->MeshContainerRootNode().GetNumMeshContainers() == 0 )
		{
			pMeshContainer.reset( new MeshObjectContainer );
			m_pCharacterItems[i]->MeshContainerRootNode().AddMeshContainer( pMeshContainer );
		}
		else
			pMeshContainer = m_pCharacterItems[i]->MeshContainerRootNode().GetMeshContainer( 0 );

		pMeshContainer->m_MeshDesc.ResourcePath = meshes[i];
		pMeshContainer->m_MeshDesc.MeshType = MeshType::SKELETAL;
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
		pEntity->SetName( "player_character" );
//		pEntity->pBaseEntity->SetMeshRenderMethod( *pEntity ); // error: cannot access protected member declared in class 'BaseEntity'

		if( m_pThirdPersonCameraController )
			m_pThirdPersonCameraController->SetCameraPose( pEntity->GetWorldPose() );
//		m_CameraOrientation.target = m_CameraOrientation.current = Quaternion( pEntity->GetWorldPose().matOrient );
	}

	m_CharacterItemEntity = entity;

	m_pThirdPersonCameraController->SetTargetEntity( EntityHandle<>( weak_ptr<CCopyEntity>(pEntity) ) );

	// set keybind to the character item
	m_pCharacterItems[0]->SetKeyBind( m_pKeyBind );

	m_pThirdPersonMotionController->SetSkeletalCharacter( m_pCharacterItems[0] );

	ArmCharacter( *m_pCharacterItems[0] );
	boost::shared_ptr<CArmedCharacterOperations> pOperations( new CArmedCharacterOperations );
	pOperations->SetWeakPtr( pOperations );
	pOperations->Init( *m_pCharacterItems[0] );
	EntityHandle<ItemEntity> pFirearm = pOperations->CreateFirearmEntity( 0 );
	pOperations->SelectFirearmUnit( 0, 0 );
	m_pCharacterItems[0]->AddOperationsAlgorithm( pOperations );

	m_pInputHandler.reset( new CInputDataDelegate<FirearmsAppTask>( this ) );
	if( GetInputHub().GetInputHandler(2) )
		GetInputHub().GetInputHandler(2)->AddChild( m_pInputHandler.get() );
	else
		GetInputHub().PushInputHandler( 2, m_pInputHandler.get() );

	m_ScrollEffect.SetTextureFilepath( "textures/precipitation_mid-density-512.dds" );
//	m_ScrollEffect.SetTextureFilepath( "textures/tex1024_red.bmp" );
	m_ScrollEffect.Init();

	ScaleAnalogInputValueRanges();
}


void FirearmsAppTask::UpdateThirdPersonCamera( float dt )
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


int FirearmsAppTask::FrameMove( float dt )
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


void FirearmsAppTask::Render()
{
	StageViewerGameTask::Render();

//	m_ScrollEffect.Render();
}


void FirearmsAppTask::HandleInput( InputData& input )
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

	shared_ptr<GraphicsElementManager> pGraphicsElemetMgr
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
// FirearmsAppBase
//========================================================================================

FirearmsAppBase::FirearmsAppBase()
{
}


FirearmsAppBase::~FirearmsAppBase()
{
//	Release();
}

/*
void FirearmsAppBase::Release()
{
}*/


const std::string FirearmsAppBase::GetStartTaskName() const
{
	return string("");
}


int FirearmsAppBase::GetStartTaskID() const
{
//	return CGameTask::ID_STAGE_VIEWER_TASK;
	return GAMETASK_ID_BASIC_PHYSICS;
}


bool FirearmsAppBase::Init()
{
	//
	// Register base entity factory
	//
/*
	GetBaseEntityManager().RegisterBaseEntityFactory( BaseEntityFactorySharedPtr( new CFG_BaseEntityFactory() ) );

	GetBaseEntityManager().AddBaseEntityClassNameToIDMap( "CFG_AIAircraftBaseEntity", CFG_BaseEntityID::BE_AIAIRCRAFTBASEENTITY );
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
	//
	// Register (task name) : (task ID) maps
	//

	CGameTask::AddTaskNameToTaskIDMap( "TitleFG",           CGameTaskFG::ID_TITLE_FG );
	CGameTask::AddTaskNameToTaskIDMap( "MainMenuFG",        CGameTaskFG::ID_MAINMENU_FG );
	CGameTask::AddTaskNameToTaskIDMap( "ControlCustomizer", CGameTaskFG::ID_CONTROLCUSTOMIZER_FG );
*/
	return true;
}
