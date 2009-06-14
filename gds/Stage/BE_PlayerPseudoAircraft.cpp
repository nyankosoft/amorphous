#include "BE_PlayerPseudoAircraft.hpp"
#include <Stage/PlayerInfo.hpp>
#include <Stage/EntitySet.hpp>
#include <Stage/CopyEntityDesc.hpp>
#include <Stage/Stage.hpp>
#include <Stage/ScreenEffectManager.hpp>
#include <Stage/HUD_PlayerAircraft.hpp>
#include <Stage/GameMessage.hpp>
#include <Stage/Input/InputHandler_PlayerPAC.hpp>
#include <Stage/ViewFrustumTest.hpp>
#include <Stage/trace.hpp>
#include <Stage/OverlapTestAABB.hpp>
#include <Stage/BE_HomingMissile.hpp>
#include <Stage/BaseEntity_Draw.hpp>

#include "3DMath/MathMisc.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/MTRand.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Macro.h"
#include "Support/Vec3_StringAux.hpp"
#include "Support/Profile.hpp"
#include "Item/WeaponSystem.hpp"
#include "Item/GI_MissileLauncher.hpp"
#include "Item/ItemDatabaseManager.hpp"
#include "GameCommon/RandomDirectionTable.hpp"
#include "GameCommon/ShockWaveCameraEffect.hpp"
#include "GameCommon/PseudoAircraftSimulator.hpp"
#include "GameCommon/MeshBoneController_Aircraft.hpp"
#include "Graphics/3DGameMath.hpp"
#include "Graphics/D3DXSMeshObject.hpp"
#include "Physics/Actor.hpp"
#include "GameTextSystem/TextMessageManager.hpp"
#include "GUI.hpp"


using namespace std;
using namespace boost;
using namespace physics;


/// action code -> ui input code (CGM_Input)
static int g_GMInputCodeForActionCode[NUM_ACTION_CODES];

static void SetDefaultGMInputCodesForActionCodes()
{
	int *tbl = g_GMInputCodeForActionCode;
/*
	tbl[ACTION_MENU_OPEN_MENU_0]       = CGM_Input:;
	tbl[ACTION_MENU_OPEN_MENU_1]       = CGM_Input:;
	tbl[ACTION_MENU_OPEN_MENU_2]       = CGM_Input:;
	tbl[ACTION_MENU_OPEN_MENU_3]       = CGM_Input:;

	tbl[ACTION_MENU_CLOSE_MENU_0]      = CGM_Input:;
	tbl[ACTION_MENU_CLOSE_MENU_1]      = CGM_Input:;
	tbl[ACTION_MENU_CLOSE_MENU_2]      = CGM_Input:;
	tbl[ACTION_MENU_CLOSE_MENU_3]      = CGM_Input:;
*/

	tbl[ACTION_MENU_UP]                = CGM_Input::SHIFT_FOCUS_UP;
	tbl[ACTION_MENU_DOWN]              = CGM_Input::SHIFT_FOCUS_DOWN;
	tbl[ACTION_MENU_LEFT]              = CGM_Input::SHIFT_FOCUS_LEFT;
	tbl[ACTION_MENU_RIGHT]             = CGM_Input::SHIFT_FOCUS_RIGHT;

	tbl[ACTION_MENU_NEXT_DIALOG_UP]    = CGM_Input::NEXT_DIALOG_UP;
	tbl[ACTION_MENU_NEXT_DIALOG_DOWN]  = CGM_Input::NEXT_DIALOG_DOWN;
	tbl[ACTION_MENU_NEXT_DIALOG_LEFT]  = CGM_Input::NEXT_DIALOG_LEFT;
	tbl[ACTION_MENU_NEXT_DIALOG_RIGHT] = CGM_Input::NEXT_DIALOG_RIGHT;

	tbl[ACTION_MENU_OK]                = CGM_Input::OK;
	tbl[ACTION_MENU_CANCEL]            = CGM_Input::CANCEL;
//	tbl[ACTION_MENU_BACK]              = CGM_Input::;
}


//================================================================================
// CBE_PlayerPseudoAircraft
//================================================================================

map<int,int> CBE_PlayerPseudoAircraft::ms_mapEntityTypeIDtoTargetTypeFlag;
map<int,int> CBE_PlayerPseudoAircraft::ms_mapEntityGroupToTargetGroupFlag;
map<int,int> CBE_PlayerPseudoAircraft::ms_mapEntityGroupToTargetTypeFlag;

vector<int> CBE_PlayerPseudoAircraft::ms_vecFocusTargetEntityGroup;


CBE_PlayerPseudoAircraft::CBE_PlayerPseudoAircraft()
:
m_pAircraft(NULL),
m_pPlayerAircraftHUD(NULL)
{
//	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_PLAYER;

//	m_ActorDesc.ActorFlag |= JL_ACTOR_APPLY_NO_ANGULARIMPULSE;

	// pseudo aircraft will not be affected by the gravity
	// since it does not simulate the physics
//	m_ActorDesc.ActorFlag |= JL_ACTOR_KINEMATIC;
	m_ActorDesc.BodyDesc.Flags = BodyFlag::Kinematic;

	RaiseEntityFlag( BETYPE_RIGIDBODY );

	ClearEntityFlag( BETYPE_USE_PHYSSIM_RESULTS );	// pose & vel are determined by pseudo simulator

//	m_fCamHeading = m_fCamHeadingVel = m_fCamPitch = m_fCamPitchVel = 0.0f;

	m_CamHeading.SetZeroState();
	m_CamPitch.SetZeroState();
	m_CamHeading.smooth_time = 0.15f;
	m_CamPitch.smooth_time = 0.15f;

	m_CameraLocalPose.Identity();

	m_CameraOrient.target = Matrix33Identity();
	m_CameraOrient.vel = Matrix33(0);
	m_CameraOrient.smooth_time = 0.06f;

//	m_fAccel = 30.0f;	m_fBoostAccel = 150.0f;	m_fBrakeAccel = 5.0f;

//	SCDVariable<Vector3> smth_vec;
//	smth_vec.SetZeroState();

	m_vecWeaponSlotLocalPose.resize( CWeaponSystem::NUM_WEAPONSLOTS, Matrix34Identity()  );

	m_Viewpoint = FIRST_PERSON_VIEW;

	m_CurrentTargetFocusIndex = 0;

	SetAircraftState( STATE_NORMAL );

	SetMissionState( MSTATE_NOT_IN_MISSION );

	m_WorldPoseOnMissionFailed = Matrix34Identity();

	m_VisionMode = CPlayerVisionMode::Normal;

	// mapping table from action input codes to UI input codes
	SetDefaultGMInputCodesForActionCodes();
}


CBE_PlayerPseudoAircraft::~CBE_PlayerPseudoAircraft()
{
	SafeDelete( m_pPlayerAircraftHUD );
}


void CBE_PlayerPseudoAircraft::Init()
{
	CBE_Player::Init();

	RaiseEntityFlag( BETYPE_PLAYER | BETYPE_FLOATING );

	float near_clip	= 1.0f;
	float far_clip	= 50000.0f;

	m_Camera.SetFOV( (float)PI / 4.0f );
//	m_Camera.SetFOV( (float)PI / 8.0f );
	m_Camera.SetNearClip( near_clip );
	m_Camera.SetFarClip( far_clip );
	m_Camera.SetAspectRatio( CGraphicsComponent::GetAspectRatio() );

	Init3DModel();

	// TODO: set base entity names from text file
	m_aExtraBaseEntity[EBE_SMOKE_TRAIL].Handle.SetBaseEntityName( "bsmkt_L" );
	m_aExtraBaseEntity[EBE_FLAME_TRAIL].Handle.SetBaseEntityName( "FlameTraceL2" );
//	m_aExtraBaseEntity[EBE_EXPLOSION].Handle.SetBaseEntityName( "" );

	LoadBaseEntity( m_aExtraBaseEntity[EBE_SMOKE_TRAIL].Handle );
	LoadBaseEntity( m_aExtraBaseEntity[EBE_FLAME_TRAIL].Handle );

	SinglePlayerInfo().SetStage( m_pStage->GetWeakPtr() );

	m_pPlayerAircraftHUD = new HUD_PlayerAircraft;
	if( m_pPlayerAircraftHUD )
	{
		m_pPlayerAircraftHUD->Init();

		CTextMessageManager* pTextMsgMgr = m_pPlayerAircraftHUD->GetTextMessageManager();
		int msg_priority = 0;

		const char *msgs[] =
		{
			"Way to go, captain!",
			"That was dead on!",
		};

		for( int i=0; i<numof(msgs); i++ )
		{
			int msg_index = pTextMsgMgr->StartLoadMessage( msg_priority );
			pTextMsgMgr->AddMessageRef( "Somebody", msgs[i] );

			m_vecMessageIndex[TM_DESTROYED_ENEMY].push_back( msg_index );
		}
	}

	m_pShortRangeRadar = ItemDatabaseManager().GetItem<CRadar>( "ShortRangeRadar", 1 );
	m_pLongRangeRadar  = ItemDatabaseManager().GetItem<CRadar>( "LongRangeRadar", 1 );
	if( m_pShortRangeRadar )
		m_pShortRangeRadar->SetStageWeakPtr( m_pStage->GetWeakPtr() );
	if( m_pLongRangeRadar )
		m_pLongRangeRadar->SetStageWeakPtr( m_pStage->GetWeakPtr() );

	// supply radar items to the single player info
	// - stage weak ptr is set by the system
/*	SinglePlayerInfo().SupplyItem( "ShortRangeRadar", 1 );
	SinglePlayerInfo().SupplyItem( "LongRangeRadar", 1 );
	m_pShortRangeRadar = SinglePlayerInfo().GetItemByName( "ShortRangeRadar" );
	m_pLongRangeRadar  = SinglePlayerInfo().GetItemByName( "LongRangeRadar" );
*/
}


void CBE_PlayerPseudoAircraft::CreateNozzleFlames( CCopyEntity* pCopyEnt )
{
	if( !m_pAircraft )
		return;

	// create nozzle flame object(s) as a child / entities
	m_NozzleFlames.CreateNozzleFlames( pCopyEnt, m_pAircraft, m_pStage );

	UpdateNozzleFlamesVisibility();

//	CBaseEntityHandle base_entity;
//	base_entity.SetBaseEntityName( "nozzle_flame" );

/*
	CCopyEntityDesc desc;
	desc.pParent = pCopyEnt;
	desc.pBaseEntityHandle = &base_entity;

	vector<NozzleFlameParams>& NozzleFlameParams = m_pAircraft->GetNozzleFlameParams();
	size_t i, num_nozzles = NozzleFlameParams.size();
	for( i=0; i<num_nozzles; i++ )
	{
		desc.SetWorldPose( pCopyEnt->GetWorldPose() * NozzleFlameParams[i].LocalPose );
		desc.f1 = NozzleFlameParams[i].start_pos;
		desc.f2 = NozzleFlameParams[i].length;
		desc.f3 = NozzleFlameParams[i].radius;

		m_pStage->CreateEntity( desc );
	}*/
}


CInputHandler_PlayerBase *CBE_PlayerPseudoAircraft::CreatePlayerInputHandler()
{
	return new CInputHandler_PlayerPAC();
}


//void CBE_PlayerPseudoAircraft::EnableNightVision( bool enable )
void CBE_PlayerPseudoAircraft::SetVisionMode( int vision_mode )
{
	CScreenEffectManager* pScreenEffectManager = m_pStage->GetScreenEffectManager();

	switch( vision_mode )
	{
	case CPlayerVisionMode::Normal:
	default:
		pScreenEffectManager->SetMonochromeEffect( 0.0f, 0.0f, 0.0f );
		pScreenEffectManager->ClearEffectFlag( ScreenEffect::PseudoNightVision );
		pScreenEffectManager->ClearEffectFlag( ScreenEffect::MonochromeColor );
		break;

	case CPlayerVisionMode::NightVision:
		pScreenEffectManager->FadeInFrom( 0xFF000000, 0.6f, AlphaBlend::One );
		pScreenEffectManager->RaiseEffectFlag( ScreenEffect::PseudoNightVision );
		pScreenEffectManager->RaiseEffectFlag( ScreenEffect::MonochromeColor );
		pScreenEffectManager->SetMonochromeEffect( -0.1f, 0.3f, -0.1f );
		break;

	case CPlayerVisionMode::Thermal:
		// NOT IMPLEMENTED
		break;
	}
}


void CBE_PlayerPseudoAircraft::SetSubDisplayType( CSubDisplayType::Name type )
{
	if( !m_pPlayerAircraftHUD )
		return;

	m_pPlayerAircraftHUD->GetSubDisplay()->SetMonitor( type );
/*
//	HUD_SubDisplay* pSubDisplay = m_pPlayerAircraftHUD->GetSubDisplay();

	switch(type)
	{
	case CSubDisplayType::NONE:
		break;
	case CSubDisplayType::AUTO:
	case CSubDisplayType::FOCUSED_TARGET_TRACKER:
//		pSubDisplay->SetMonitorIndex(
		break;
	case CSubDisplayType::FRONT_VIEW:
	case CSubDisplayType::REAR_VIEW:
		break;
	case CSubDisplayType::MISSILE_VIEW:
		break;
	default:
		break;
	}*/
}


void CBE_PlayerPseudoAircraft::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	LOG_PRINT( " - initializing a pseudo aircraft copy entity" );
	LOG_PRINT( string(" - player aircraft at ") + to_string(pCopyEnt->Position()) );

	CBE_Player::InitCopyEntity( pCopyEnt );

	// sets the borrowed reference to the aircraft item player has selected
	SetAircraft();

	if( !m_pAircraft )
	{
		LOG_PRINT_ERROR( " - no aircraft item" );
		return;
	}

	// create mesh render method and set it to pCopyEnt
	// - use the shader & shader technique stored in m_MeshProperty
	if( 0 < m_pAircraft->MeshContainerRootNode().GetNumMeshContainers() )
	{
		shared_ptr<CMeshObjectContainer> pMeshContainer = m_pAircraft->MeshContainerRootNode().MeshContainer(0);
//		if( pMeshContainer && pMeshContainer->m_ShaderTechnique.size_x() )
		if( pMeshContainer && m_MeshProperty.m_ShaderTechnique.size_x() )
		{
			pCopyEnt->m_MeshHandle = pMeshContainer->m_MeshObjectHandle;
			CreateMeshRenderMethod(
				CEntityHandle<>( pCopyEnt->Self() ),
//				pMeshContainer->m_ShaderHandle,
				m_MeshProperty.m_ShaderHandle,
//				pMeshContainer->m_ShaderTechnique(0,0)
				m_MeshProperty.m_ShaderTechnique(0,0)
				);
		}
	}

	// create transparent parts of the model as separate entities
	if( m_EntityFlag & BETYPE_SUPPORT_TRANSPARENT_PARTS )
		CreateAlphaEntities( pCopyEnt );

	m_pAircraft->ResetAircraftControls();

//	Vector3 vInitVelocity = Vector3(0,0,5);
//	pCopyEnt->SetVelocity( vInitVelocity );

//	Matrix34 init_pose( Vector3( 0,1200, -100 ), Matrix33Identity() );
//	pCopyEnt->SetWorldPose( init_pose );

//	physics::CActor *pPhysicsActor = pCopyEnt->pPhysicsActor;
	physics::CActor *pPhysicsActor = NULL;
	if( 0 < pCopyEnt->m_vecpPhysicsActor.size() )
		pPhysicsActor = pCopyEnt->m_vecpPhysicsActor[0];

	if( pPhysicsActor )
	{
		pPhysicsActor->SetWorldPose( pCopyEnt->GetWorldPose() );
		pPhysicsActor->SetLinearVelocity( pCopyEnt->Velocity() );
//		pPhysicsActor->SetPosition( init_pose.vPosition );
//		pPhysicsActor->SetOrientation( init_pose.matOrient );
//		pCopyEnt->SetVelocity( vInitVelocity );
	}

	PseudoSimulator().ResetSimulator();

	PseudoSimulator().SetWorldPose( pCopyEnt->GetWorldPose() );

	// TODO: should be able to set arbitrary velocity to the player aircraft at the start?
	Vector3 vFwdDir = pCopyEnt->GetDirection();
	PseudoSimulator().SetForwardVelocity( vFwdDir * Vec3Dot(vFwdDir,pCopyEnt->Velocity()) );

	PseudoSimulator().SetAccel( m_pAircraft->GetDefaultAccel() );
	PseudoSimulator().SetYawAccel( 0.0f );
	PseudoSimulator().SetPitchAccel( 0.0f );
	PseudoSimulator().SetRollAccel( 0.0f );

	m_CameraOrient.target = pCopyEnt->GetWorldPose().matOrient;
	m_CameraOrient.current = pCopyEnt->GetWorldPose().matOrient;

	pCopyEnt->fLife = 1000.0f;

	CreateNozzleFlames(pCopyEnt);

	InitSubDisplay();

	// mission started - set the mission state
	SetMissionState( MSTATE_IN_MISSION );

	LOG_PRINT( " - end" );
}


void CBE_PlayerPseudoAircraft::Move( CCopyEntity *pCopyEnt )
{
//	if( !pCopyEnt->pPhysicsActor )
	if( pCopyEnt->m_vecpPhysicsActor.size() == 0
	 || pCopyEnt->m_vecpPhysicsActor[0] == NULL )
	{
//		assert( pCopyEnt->pPhysicsActor );
		ONCE( LOG_PRINT_ERROR( " No physics actor" ) );
		return;
	}

//	CActor& rPhysicsActor = *pCopyEnt->pPhysicsActor;
	CActor& rPhysicsActor = *(pCopyEnt->m_vecpPhysicsActor[0]);
}


void CBE_PlayerPseudoAircraft::InitSubDisplay()
{
	// set up the sub-display
	Matrix34 front_local_pose = Matrix34( Vector3( 0.0f, 0.0f, -20.0f ), Matrix33Identity() );	// front view
	Matrix34 rear_local_pose  = Matrix34( Vector3( 0.0f, 0.0f, -20.0f ), Matrix33RotationY( 3.141592f ) );	// rear view
	vector< shared_ptr<SubMonitor> >& submonitor = m_SubDisplay.Monitor();
	submonitor.resize( CSubDisplayType::NUM_TYPES, shared_ptr<SubMonitor>() );
	submonitor[CSubDisplayType::NONE]                   = shared_ptr<SubMonitor>( new SubMonitor_Null() );
	submonitor[CSubDisplayType::AUTO]                   = shared_ptr<SubMonitor>( new SubMonitor_Null() );
	submonitor[CSubDisplayType::FRONT_VIEW]             = shared_ptr<SubMonitor>( new SubMonitor_FixedView( front_local_pose ) );
	submonitor[CSubDisplayType::REAR_VIEW]              = shared_ptr<SubMonitor>( new SubMonitor_FixedView( rear_local_pose ) );
	submonitor[CSubDisplayType::MISSILE_VIEW]           = shared_ptr<SubMonitor>( new SubMonitor_Null() );
	submonitor[CSubDisplayType::FOCUSED_TARGET_TRACKER] = shared_ptr<SubMonitor>( new SubMonitor_EntityTracker() );

	m_SubDisplay.SetMonitor( CSubDisplayType::NONE );

	m_SubDisplay.SetStage( m_pStageWeakPtr );

	// set borrowed reference
	if( m_pPlayerAircraftHUD )
		m_pPlayerAircraftHUD->SetSubDisplay( &m_SubDisplay );

	CCopyEntity* pPlayerEntity = GetPlayerCopyEntity();
	Vector3 vPos = pPlayerEntity->Position() + pPlayerEntity->GetWorldPose().matOrient.GetColumn(2) * 1000.0f;
	m_SubDisplay.SetTargetPosition( vPos );
	m_SubDisplay.SetTargetRadius( 45.0f );
}


bool CBE_PlayerPseudoAircraft::SetAircraft()
{
	CGI_Aircraft* pAircraft = SinglePlayerInfo().GetAircraft();

	if( !pAircraft )
	{
		LOG_PRINT_WARNING( " - an aircraft item is not selected." );
		return false;
	}

	m_pAircraft = pAircraft;

	m_CameraLocalPose = m_pAircraft->GetCockpitLocalPose();

	// set release positions for missile launchers
	m_pAircraft->InitWeaponSystem();

	m_pAircraft->WeaponSystem().SetProjectileGroup( GetEntityGroupID( m_ProjectileEntityGroup ) );

	// get mesh filename from the aircraft item and load the mesh
	m_MeshProperty.Release();

//	const CMeshObjectContainer &mesh_container = pAircraft->GetMeshObjectContainer();
	const CMeshObjectContainer &mesh_container = *(pAircraft->MeshContainerRootNode().MeshContainer(0).get());
	if( !mesh_container.m_MeshObjectHandle.IsLoaded() )
		pAircraft->LoadMeshObject(); // load the mesh of the aircraft item - the mesh controller uses it

	// copy mesh related stuff from item to base entity...
//	m_MeshProperty.m_MeshDesc         = pAircraft->GetMeshObjectContainer().m_MeshDesc;
	m_MeshProperty.m_MeshDesc         = mesh_container.m_MeshDesc;

	// Set target material indices to create alpha entity
	Init3DModel();

	if( m_MeshProperty.m_MeshObjectHandle.GetMesh()
	 && m_MeshProperty.m_MeshObjectHandle.GetMeshType() == CMeshType::SKELETAL )
	{
		m_pAircraft->ResetMeshController();
		shared_ptr<CD3DXMeshObjectBase> pBaseMesh = m_MeshProperty.m_MeshObjectHandle.GetMesh();
		shared_ptr<CD3DXSMeshObject> pSkeletalMesh = boost::dynamic_pointer_cast<CD3DXSMeshObject,CD3DXMeshObjectBase>(pBaseMesh);
		m_pAircraft->InitMeshController( pSkeletalMesh );
	}

	// set mesh bone controller (experimental)
/*	SBE_MeshObjectProperty& rMesh = m_MeshProperty;
	SafeDeleteVector( rMesh.m_vecpMeshBoneController );
	if( rMesh.m_pMeshObject && rMesh.m_pMeshObject->GetMeshType() == CD3DXMeshObjectBase::TYPE_SMESH )
	{
		// skeletal mesh
		rMesh.m_vecpMeshBoneController.resize( 2 );

		rMesh.m_vecpMeshBoneController[0]
		= new CMeshBoneController_Flap( (CD3DXSMeshObject *)rMesh.m_pMeshObject, &m_pAircraft->PseudoSimulator() );
		rMesh.m_vecpMeshBoneController[0]->Init();

		rMesh.m_vecpMeshBoneController[1]
		= new CMeshBoneController_VFlap( (CD3DXSMeshObject *)rMesh.m_pMeshObject,
			&m_pAircraft->PseudoSimulator(), CMeshBoneController_VFlap::TYPE_SINGLE );
		rMesh.m_vecpMeshBoneController[1]->Init();
	}*/

	return true;
}


void CBE_PlayerPseudoAircraft::UpdateFocusCandidateTargets( const vector<CCopyEntity *>& vecpEntityBuffer )
{
	m_vecFocusCandidate.resize( 0 );

	// get the range of the current missile
	float effective_range = 25000.0f;
	if( m_pAircraft )
	{
		SWeaponSlot& slot = m_pAircraft->WeaponSystem().GetPrimaryWeaponSlot();
		if( slot.pChargedAmmo )
			effective_range = slot.pChargedAmmo->GetRange() * 2.0f;
	}

	const float effective_range_sq = effective_range * effective_range;
	const Vector3 vPlayerPos = m_pPlayerCopyEntity->Position();

//	size_t num_targets_on_radar = m_RadarInfo.m_vecTargetInfo.size();
//	size_t num_candidates = NUM_MAX_FOCUS_CANDIDATES < num_entities ? NUM_MAX_FOCUS_CANDIDATES : num_entities;
//	float score = 0;
	size_t i, num_entities = vecpEntityBuffer.size();
	size_t num_valid_entities = 0;
	for( i=0; i<num_entities && m_vecFocusCandidate.size() < NUM_MAX_FOCUS_CANDIDATES; i++ )
	{
		if( !IsValidEntity(vecpEntityBuffer[i]) )
			continue;

		if( effective_range_sq < Vec3LengthSq( vecpEntityBuffer[i]->Position() - vPlayerPos ) )
			continue;

		num_valid_entities++;

		
//		if( vecpEntityBuffer[i]->GroupIndex == CE_GROUP_ENEMY )
		if( IsFocusTargetEntity( vecpEntityBuffer[i]->GroupIndex ) )
		{
            m_vecFocusCandidate.push_back( FocusCandidate( vecpEntityBuffer[i]->Self(), 1.0f ) );
		}

//		score = target.pos
//		m_RadarInfo.m_vec
	}

	if( m_vecFocusCandidate.size() == 0 )
	{
		// no candidate entities to focus on
		m_CurrentTargetFocusIndex = -1;
		m_FocusedTarget = CEntityHandle<>();
		return;
	}
	else
	{
		// if there is at least one target on radar and no focus is set on any target
		// right now, set the focus on the first target in m_vecFocusCandidate
		shared_ptr<CCopyEntity> pEntity = m_FocusedTarget.Get();
		if( !pEntity /*&& 0 < m_vecFocusCandidate.size()*/ )
		{
			m_FocusedTarget = m_vecFocusCandidate[0].entity;
			m_CurrentTargetFocusIndex = 0;
		}
		else
		{
			size_t num_focus_candidates = m_vecFocusCandidate.size();
			for( i=0; i<num_focus_candidates; i++ )
			{
				pEntity = m_vecFocusCandidate[i].entity.Get();
				shared_ptr<CCopyEntity> pFocusedEntity = m_FocusedTarget.Get();
				if( pFocusedEntity == pEntity )
				{
					// keep the current focused target
					m_CurrentTargetFocusIndex = (int)i;
					break;
				}
			}
			if( num_focus_candidates <= i )
			{
//				MsgBoxFmt( "changing target focus (entities: %d, valid: %d, candidates: %d)", num_entities, num_valid_entities, num_focus_candidates );
				// set a new target focus
				m_FocusedTarget = m_vecFocusCandidate[0].entity;
				m_CurrentTargetFocusIndex = 0;
			}
		}
	}

//	Limit( m_CurrentTargetFocusIndex, 0, m_vecFocusCandidate.size() - 1 );

	// update focused target in missile launchers

	return;

	if( !m_pAircraft )
		return;

	CWeaponSystem& weapon_system = m_pAircraft->WeaponSystem();
	for( i=0; i<CWeaponSystem::NUM_WEAPONSLOTS; i++ )
	{
		CGI_Weapon* pWeapon = weapon_system.GetWeaponSlot(i).pWeapon;
		if( !pWeapon )
			continue;

		if( pWeapon->GetArchiveObjectID() != CGameItem::ID_MISSILELAUNCHER )
			continue;

		CGI_MissileLauncher *pLauncher = dynamic_cast<CGI_MissileLauncher *>(pWeapon); // should always succeed

		// set primary target to lock-on
		pLauncher->SetPrimaryTarget( m_FocusedTarget );
	}
}


void CBE_PlayerPseudoAircraft::UpdateRadarInfo( CCopyEntity* pCopyEnt, float dt )
{
	PROFILE_FUNCTION();

/*	float effective_radar_radius = 500000.0f; // 500[km]
	const float r = effective_radar_radius;

	// cube with each edge 1000[km]
	// TODO: use a proper bounding-box that contains the entire stage
	AABB3 aabb = AABB3(
		Vector3(-1,-1,-1) * effective_radar_radius + pCopyEnt->Position(),
		Vector3( 1, 1, 1) * effective_radar_radius + pCopyEnt->Position() );	// m_pStage->GetAABB();

	COverlapTestAABB overlap_test( aabb, &s_vecpEntityBuffer, ENTITY_GROUP_MIN_ID );

	m_pStage->GetEntitySet()->GetOverlappingEntities( overlap_test );
*/
//	UpdateFocusCandidateTargets( s_vecpEntityBuffer );

	if( !m_pLongRangeRadar || !m_pShortRangeRadar )
		return;

	m_pLongRangeRadar->SetRadarWorldPose( pCopyEnt->GetWorldPose() );
	m_pLongRangeRadar->Update( dt );

	m_pShortRangeRadar->SetRadarWorldPose( pCopyEnt->GetWorldPose() );
	m_pShortRangeRadar->Update( dt );

	UpdateFocusCandidateTargets( m_pShortRangeRadar->EntityRawPtrBuffer() );

	// clear all the previous target info
/*	m_RadarInfo.ClearTargetInfo();

	Vector3 vCamFwdDir = m_Camera.GetFrontDirection();// pCopyEnt->GetDirection();
*/
	// get missile laucnher on the current aircraft
	CGI_Weapon *pPrimaryWeapon = m_pAircraft->WeaponSystem().GetPrimaryWeapon();
	CGI_MissileLauncher* pLauncher = NULL;
	if( pPrimaryWeapon && pPrimaryWeapon->GetArchiveObjectID() == CGameItem::ID_MISSILELAUNCHER )
		pLauncher = dynamic_cast<CGI_MissileLauncher *>(pPrimaryWeapon); // should always succeed

	// set primary target to lock-on
	shared_ptr<CCopyEntity> pFocusedTarget = m_FocusedTarget.Get();
	if( pLauncher && pFocusedTarget )
		pLauncher->SetPrimaryTarget( m_FocusedTarget );

	if( pFocusedTarget )
	{
		m_SubDisplay.SetTargetPosition( pFocusedTarget->Position() );
		m_SubDisplay.SetTargetRadius( pFocusedTarget->GetRadius() );
	}

/*
	size_t i, num_entities = s_vecpEntityBuffer.size();
	for( i=0; i<num_entities; i++ )
	{
		CCopyEntity *pEntity = s_vecpEntityBuffer[i];

		if( !IsValidEntity( pEntity ) )
			continue;

		CBaseEntity* pBaseEntity = pEntity->pBaseEntity;

		unsigned int id = pBaseEntity->GetArchiveObjectID();
		int tgt_type = 0;

		switch(id)
		{
		case BE_PLAYERPSEUDOAIRCRAFT:
			tgt_type = HUD_TargetInfo::PLAYER | HUD_TargetInfo::TGT_AIR;
			break;

		case BE_HOMINGMISSILE:
			if( MissileState(pEntity) == CBE_HomingMissile::MS_IGNITED )
				tgt_type = HUD_TargetInfo::MISSILE;
			break;

		default:
			map<int,int>::iterator itr_group
				= ms_mapEntityGroupToTargetGroupFlag.find( pEntity->GroupIndex );

			if( itr_group != ms_mapEntityGroupToTargetGroupFlag.end() )
			{
				// set the target type
				// - HUD_TargetInfo::PLAYER, ALLY or ENEMY
				tgt_type |= itr_group->second;
			}

			map<int,int>::iterator itr_type
				= ms_mapEntityTypeIDtoTargetTypeFlag.find( pEntity->GetEntityTypeID() );

			if( itr_type != ms_mapEntityTypeIDtoTargetTypeFlag.end() )
			{
				// set the target group
				// - UD_TargetInfo::TGT_AIR or TGT_SURFACE
				tgt_type |= itr_type->second;
			}

			break;
		}

		if( tgt_type != 0 )
		{
			if( pEntity == m_pFocusedTarget )
			{
				tgt_type |= HUD_TargetInfo::FOCUSED;
				m_RadarInfo.m_FocusedTargetIndex = m_RadarInfo.m_vecTargetInfo.size();
			}

			if( pLauncher && pLauncher->IsLockingOn(pEntity) )
			{
				tgt_type |= HUD_TargetInfo::LOCKED_ON;
			}

			m_RadarInfo.m_vecTargetInfo.push_back( HUD_TargetInfo( pEntity->Position(), "", tgt_type ) );
			m_RadarInfo.m_vecTargetInfo.back().direction = pEntity->GetDirection();
			m_RadarInfo.m_vecTargetInfo.back().radius = pEntity->fRadius * 1.5f;

			Vector3 vPlayerToTargetDir = Vec3GetNormalized( pEntity->Position() - pCopyEnt->Position() );

			// mark as visible if the target is in the view frustum
			if( acos(Vec3Dot(vCamFwdDir,vPlayerToTargetDir)) < deg_to_rad(30.0f) 
			&& tgt_type != HUD_TargetInfo::MISSILE )
				m_RadarInfo.m_vecVisibleTargetIndex.push_back( m_RadarInfo.m_vecTargetInfo.size() - 1 );
		}
	}*/

	vector<HUD_TargetInfo>& target_info
		= m_pShortRangeRadar->RadarInfo().TargetInfo();
	const size_t num_targets = target_info.size();

	// set the 'focused' flag
	pFocusedTarget = m_FocusedTarget.Get();
	if( pFocusedTarget )
	{
		for( size_t i=0; i<num_targets; i++ )
		{
			if( pFocusedTarget->GetID() == target_info[i].entity_id )
				m_pShortRangeRadar->RadarInfo().SetFocusedTargetIndex( i );
		}
	}

	// set the 'locked on' flag
	if( pLauncher )
	{
		for( size_t i=0; i<num_targets; i++ )
		{
			if( pLauncher->IsLockingOn( target_info[i].entity_id ) )
				target_info[i].type |= HUD_TargetInfo::LOCKED_ON;
		}
	}
/*
	m_State = STATE_NORMAL;
	for( size_t i=0; i<num_entities; i++ )
	{
		CCopyEntity *pEntity = s_vecpEntityBuffer[i];

		if( !IsValidEntity( pEntity ) )
			continue;

		if( pEntity->pBaseEntity->GetArchiveObjectID() == BE_HOMINGMISSILE )
		{
			if( pEntity->m_Target.GetRawPtr() == pCopyEnt )
				m_State = STATE_MISSILE_APPROACHING;
		}
	}*/
}


void CBE_PlayerPseudoAircraft::Act( CCopyEntity* pCopyEnt )
{
	CBE_Player::Act( pCopyEnt );

	if( !m_pAircraft )
		return;

//	if( pCopyEnt->fLife <= 0 )
//	{
		// player aircraft is destroyed - call update routines of child entities
		for( int i=0; i<pCopyEnt->GetNumChildren(); i++ )
			pCopyEnt->GetChild(i)->Act();

//		return;
//	}

	UpdateRadarInfo( pCopyEnt, m_pStage->GetFrameTime() );

	int nozzle_frame_entity_offset = m_pLaserDotEntity ? 1 : 0;

	m_NozzleFlames.UpdateNozzleFlames( pCopyEnt, nozzle_frame_entity_offset, m_pAircraft );

	m_SubDisplay.SetOwnerWorldPose( pCopyEnt->GetWorldPose() );
	m_SubDisplay.Update( m_pStage->GetFrameTime() );

	// test for nozzle flash
//	size_t i, num_nozzles = m_pAircraft->GetNozzleFlameParams().size();

/*
	if( m_Viewpoint == FIRST_PERSON_VIEW )
	{
		for( i=0; i<num_nozzles; i++ )
			pCopyEnt->apChild[i]->EntityFlag &= ~BETYPE_VISIBLE;	// don't render muzzle flash
	}
	else
	{
		for( i=0; i<num_nozzles; i++ )
			pNozzleFlame->EntityFlag |= BETYPE_VISIBLE;
	}
*/
}


void CBE_PlayerPseudoAircraft::OnDestroyingEnemyEntity( const SGameMessage& msg )
{
	CBE_Player::OnDestroyingEnemyEntity( msg );

	if( m_pPlayerAircraftHUD )
	{
		CTextMessageManager* pTextMsgMgr = m_pPlayerAircraftHUD->GetTextMessageManager();
		if( pTextMsgMgr )
		{
			int msg_index = RangedRand( 0, (int)m_vecMessageIndex[TM_DESTROYED_ENEMY].size() - 1 );
			pTextMsgMgr->ScheduleTextMessage( msg_index, 1.0, true, 5.0 );
//			pTextMsgMgr->StartTextMessage( 
		}
	}
}


void CBE_PlayerPseudoAircraft::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
	float& rfLife = pCopyEnt_Self->fLife;
	SGameMessage msg;
	int iVariation;

	switch( rGameMessage.iEffect )
	{
	case GM_DAMAGE:
		// play a damage sound according to the type of damage
//		iVariation = PLYAER_DAMAGESOUND_VARIATION * rand() / RAND_MAX;
		iVariation = RangedRand( 0, PLYAER_DAMAGESOUND_VARIATION );
		if( PLYAER_DAMAGESOUND_VARIATION <= iVariation )
			iVariation = PLYAER_DAMAGESOUND_VARIATION - 1;

//		SoundManager().PlayAt( m_aDamageSound[rGameMessage.s1][iVariation], pCopyEnt_Self->Position() );

		// flash screen with red
//		m_pStage->GetScreenEffectManager()->FadeInFrom( 0x40F82000, 0.20f, D3DBLEND_INVSRCALPHA );

		rfLife -= rGameMessage.fParam1;
		if( rfLife <= 0 )
		{
			rfLife = 0;
			pCopyEnt_Self->Velocity() = Vector3(0,0,0);
//			PseudoSimulator().SetAccel( 0 );

			if( GetMissionState() != MSTATE_CRASHED )
			{
                SetMissionState( MSTATE_KIA );
				m_WorldPoseOnMissionFailed = pCopyEnt_Self->GetWorldPose();

				// player no longer has control over the aircraft
				pCopyEnt_Self->RaiseEntityFlags( BETYPE_USE_PHYSSIM_RESULTS );

				// create flame & smoke trail
				for( int i=0; i<pCopyEnt_Self->GetNumChildren(); i++ )
				{
					CCopyEntity *pChild = pCopyEnt_Self->m_aChild[i].GetRawPtr();
					m_pStage->TerminateEntity( pChild );
				}
				pCopyEnt_Self->iNumChildren = 0;

				int base_entity_index[2] = { EBE_SMOKE_TRAIL,EBE_FLAME_TRAIL };
				for( int i=0; i<2; i++ )
				{
					CCopyEntityDesc desc;
					desc.SetDefault();
					desc.SetWorldPose( pCopyEnt_Self->GetWorldPose() );
					desc.v1 = pCopyEnt_Self->Position();
					desc.pBaseEntityHandle = &m_aExtraBaseEntity[base_entity_index[i]].Handle;
					desc.pParent = pCopyEnt_Self;

					m_pStage->CreateEntity( desc );
				}

			}

//			m_pStage->SetState( CStage::MISSION_FAILED );

//			m_pStage->GetScreenEffectManager()->FadeOutTo( 0xFFFFFFFF, 2.5f, D3DBLEND_ONE );
		}

//		if( rGameMessage.s1 == DMG_BLAST )
//		{
//			m_pShockWaveEffect->AddForce( rGameMessage.vParam * rGameMessage.fParam1 * 80.0f
//				+ Vec3RandDir() * RangedRand( 0.05f, 0.25f ) );
//		}
		break;

	case GM_IMPACT:
		m_pShockWaveEffect->AddVelocity( rGameMessage.vParam );
		break;

	case GM_HEALING:
		if( 120 <= rfLife )
			break;	// 'rLife' is already max

		rfLife += rGameMessage.fParam1;
		if( 120 < rfLife )
			rfLife = 120;

		// notify the item copy-entity that player has consumed it
		msg.iEffect = GM_EFFECTACCEPTED;
		msg.pSenderEntity = pCopyEnt_Self;
		SendGameMessageTo( msg, rGameMessage.pSenderEntity );
		break;

	case GM_MISSILE_TARGET:
		m_State = STATE_MISSILE_APPROACHING;
		break;

/*	case GM_AMMOSUPPLY:
		if( SinglePlayerInfo().SupplyItem( rGameMessage.pcStrParam, (int)(rGameMessage.fParam1) ) )
		{
			// the item was accepted by the player
			msg.iEffect = GM_EFFECTACCEPTED;
			msg.pSenderEntity = pCopyEnt_Self;
			SendGameMessageTo( msg, rGameMessage.pSenderEntity );
		}
		break;*/

	default:
		CBE_Player::MessageProcedure( rGameMessage, pCopyEnt_Self );
		break;
	}

}


void CBE_PlayerPseudoAircraft::OnEntityDestroyed(CCopyEntity* pCopyEnt)
{
	// detach player base entity from the SinglePlayerInfo()
	CBE_Player::OnEntityDestroyed( pCopyEnt );

	if( m_pAircraft )
		m_pAircraft->WeaponSystem().Disarm();

	// m_pAircraft->m_vecpMeshController has borrowed reference to mesh object
	m_pAircraft->ResetMeshController();
	m_pAircraft = NULL;
}


void CBE_PlayerPseudoAircraft::Draw( CCopyEntity *pCopyEnt )
{
	if( m_Viewpoint == THIRD_PERSON_VIEW
	 || m_pStage->GetEntitySet()->GetCameraEntity() != pCopyEnt )
	{
		if( m_pAircraft )
			m_pAircraft->UpdateTargetMeshTransforms();

		Draw3DModel(pCopyEnt);
	}
}


#define VIEW_CONTROL

void CBE_PlayerPseudoAircraft::UpdateCamera( CCopyEntity* pCopyEnt )
{

#ifdef VIEW_CONTROL

//	Vector3 vViewOffset = m_Viewpoint==THIRD_PERSON_VIEW ? Vector3(0,0,-16) : Vector3(0,0,0);
	float view_offset = - (pCopyEnt->fRadius * 2.5f);// -45.0f;

	Matrix33 matHeading, matPitch;
	matHeading.SetRotationY( m_CamHeading.current );
	Vector3 vPitchAxis = matHeading * Vector3(1,0,0);
	matPitch = Matrix33RotationAxis( m_CamPitch.current, vPitchAxis );

	const Matrix34& entity_world_pose = pCopyEnt->GetWorldPose();
	if( m_Viewpoint == THIRD_PERSON_VIEW )
	{
		m_Camera.SetOrientation( m_CameraOrient.current );
//		m_Camera.SetPosition( entity_world_pose * Vector3(0,1.5f,0) + m_Camera.GetFrontDirection() * view_offset  );

		float height_shift = m_pAircraft ? m_pAircraft->GetThirdPersonCameraOffset().y : 1.5f;
		float depth_shift  = m_pAircraft ? m_pAircraft->GetThirdPersonCameraOffset().z : - (pCopyEnt->fRadius * 2.5f);
		m_Camera.SetPosition( entity_world_pose * Vector3(0,height_shift,0) + m_Camera.GetFrontDirection() * depth_shift  );

//		Vector3 vViewOffset = m_pAircraft ? m_pAircraft->GetThirdPersonCameraOffset() : Vector3(0,1.5f,-45.0f);
//		m_Camera.SetPosition( entity_world_pose * vViewOffset );
	}
	else
	{
		m_Camera.SetOrientation( pCopyEnt->GetWorldPose().matOrient * matPitch * matHeading );
		m_Camera.SetPosition( entity_world_pose * m_CameraLocalPose.vPosition );
	}

#else // VIEW_CONTROL

	m_Camera.SetPose( pCopyEnt->GetWorldPose() );

#endif // VIEW_CONTROL

	m_Camera.UpdateVFTreeForWorldSpace();

	m_CameraOrient.target = pCopyEnt->GetWorldPose().matOrient * matPitch * matHeading;

//	UpdateCameraEffect();
}


void CBE_PlayerPseudoAircraft::UpdatePhysics( CCopyEntity *pCopyEnt, float dt )
{
//	if( !pCopyEnt->pPhysicsActor )
	if( pCopyEnt->m_vecpPhysicsActor.size() == 0
	 || pCopyEnt->m_vecpPhysicsActor[0] == NULL )
	{
//		assert( pCopyEnt->pPhysicsActor );
		ONCE( LOG_PRINT_ERROR( " No physics actor" ) );
		return;
	}

//	CActor& rPhysicsActor = *pCopyEnt->pPhysicsActor;
	CActor& rPhysicsActor = *(pCopyEnt->m_vecpPhysicsActor[0]);

	// wake up the actor if it's sleeping
//	if( rPhysicsActor.GetActivityState() == CJL_PhysicsActor::FROZEN )
//		rPhysicsActor.ApplyWorldImpulse( pCopyEnt->GetUpDirection() * 0.0001f );
	if( rPhysicsActor.IsSleeping() )
		rPhysicsActor.WakeUp();

	if( pCopyEnt->fLife <= 0 && GetMissionState() == MSTATE_CRASHED )
		return;

	Vector3 vOrigPos = pCopyEnt->Position();

	CPseudoAircraftSimulator& pseudo_simulator = PseudoSimulator();
	pseudo_simulator.Update( dt );

	// udpate world poses & velocities of weapon slots
	// - world poses & velocities of pseudo simulator is used
	//   to determinte those of weapon slots
	// call Update() of weapon items used by 'm_pAircraft'
    m_pAircraft->Update( dt );

	// check collision
	Vector3 vUpdatedPos = pseudo_simulator.GetWorldPose().vPosition;
	STrace trace;
	vOrigPos = vOrigPos - (vUpdatedPos - vOrigPos) * 20.0f;
	trace.pvStart = &vOrigPos;
	trace.pvGoal = &vUpdatedPos;
	trace.aabb = AABB3(-Vector3(0.1f,0.1f,0.1f), Vector3(0.1f,0.1f,0.1f) );
	trace.bvType = BVTYPE_SMALLSPHERE;//BVTYPE_DOT;
	trace.fRadius = 0.1f;
	trace.pSourceEntity = pCopyEnt;
	trace.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;
//	trace.SetAABB();
//	trace.GroupIndex = CE_GROUP_PLAYER_PROJECTILE;	// avoid hitting the player
	m_pStage->ClipTrace( trace );

//	PERIODICAL( 1500, MsgBoxFmt( "start: %s, goal: %s", to_string(vOrigPos).c_str(), to_string(vUpdatedPos).c_str() ) )

	if( trace.fFraction < 1.0f )
	{
		m_pStage->GetScreenEffectManager()->FadeInFrom( 0xFFF60606, 1.0f, AlphaBlend::One );
		SGameMessage msg( GM_DAMAGE );
		SetMissionState( MSTATE_CRASHED );
		m_WorldPoseOnMissionFailed.vPosition = trace.vEnd;
		m_WorldPoseOnMissionFailed.matOrient = pseudo_simulator.GetWorldPose().matOrient;
		pseudo_simulator.SetForwardVelocity( Vector3(0,0,0) );
		pseudo_simulator.SetAccel( 0 );
		msg.fParam1 = 100000.0f;
		SendGameMessageTo( msg, pCopyEnt );
//		MsgBox( "player hit the static geometry" );
	}

	// update the properties of copy entity & physics actor
	pCopyEnt->SetWorldPose( pseudo_simulator.GetWorldPose() );
	pCopyEnt->SetVelocity( pseudo_simulator.GetVelocity() );
	pCopyEnt->fSpeed = Vec3Length( pCopyEnt->Velocity() );

//	physics::CActor *pPhysicsActor = pCopyEnt->pPhysicsActor;
	physics::CActor *pPhysicsActor = pCopyEnt->m_vecpPhysicsActor[0];

	pPhysicsActor->SetWorldPose( pseudo_simulator.GetWorldPose() );
	pPhysicsActor->SetLinearVelocity( pseudo_simulator.GetVelocity() );

	// update camera state - not a physics, but it requires dt
	// should be placed somewhere else
	m_CamHeading.Update( dt );
	m_CamPitch.Update( dt );

	m_CameraOrient.Update( dt );
	m_CameraOrient.current.Orthonormalize();
}


bool CBE_PlayerPseudoAircraft::HandleInput( SPlayerEntityAction& input )
{
	if( !m_pAircraft )
		return false;

	if( CBE_Player::HandleInput( input ) )
		return true;	// input has been handled by the parent class 'CBE_Player'


	int input_type = input.type == SPlayerEntityAction::KEY_PRESSED ? ITYPE_KEY_PRESSED : ITYPE_KEY_RELEASED;

	// let command menu handle the input
	// - secondary action is sent to command menu
	if( CActionCode::IsMenuCode( input.SecondaryActionCode )
	 && m_pCmdMenuDialogManager )
	{
		CGM_InputData ui_input;
		ui_input.code = g_GMInputCodeForActionCode[input.SecondaryActionCode];
		ui_input.type = input.type == SPlayerEntityAction::KEY_PRESSED ? CGM_InputData::TYPE_PRESSED : CGM_InputData::TYPE_RELEASED;
		ui_input.fParam = input.fParam;
		bool handled = m_pCmdMenuDialogManager->HandleInput( ui_input );

		if( handled )
			return true;

		// the switch/case block below handles opening & closing of root dialogs
		// - do not return if the input was not handled
	}

	// update aircraft simulator and weapon controls
	// - accel, brake, yaw, pitch, and roll
	// - cycling weapons
	if( m_pAircraft->HandleInput( input.ActionCode, input_type, input.fParam ) )
		return true;

	static const int cmdmenu_misc = 0;
	static const int cmdmenu_order = 1;

	switch( input.ActionCode )
	{
	case ACTION_MOV_LOOK_RIGHT:
		m_CamHeading.target = input.fParam * (float)PI;
		break;

	case ACTION_MOV_LOOK_UP:
		m_CamPitch.target = input.fParam * (float)PI * 0.5f;
		break;

	case ACTION_MISC_CYCLE_VIEWPOINTS:
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
		{
			CycleViewpoint();
			return true;
		}
		break;

	case ACTION_MISC_CYCLE_TARGET_FOCUS:
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
		{
			CycleTargetFocus();
			return true;
		}
		break;

	case ACTION_MISC_TOGGLE_NIGHTVISION:
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
		{
//			ToggleNightVision();
			return true;
		}
		break;

	case ACTION_MISC_HOLD_RADAR:

		if(	m_pPlayerAircraftHUD )
		{
			if( input.type == SPlayerEntityAction::KEY_PRESSED )
			{
				m_pPlayerAircraftHUD->DisplayGlobalRadar( true );
			}
			else
			{
				m_pPlayerAircraftHUD->DisplayGlobalRadar( false );
			}
		}
		break;

	case ACTION_MISC_TOGGLE_HUD:
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
		{
			if( m_pPlayerAircraftHUD )
				m_pPlayerAircraftHUD->ToggleShowHide();
		}
		break;

	case ACTION_MENU_OPEN_MENU_0:
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
		{
			// open misc command menu by down key
			if( m_pCmdMenuDialogManager
			 && cmdmenu_misc < m_vecCmdMenuRootDialogID.size()
			 && !m_pCmdMenuDialogManager->IsThereAnyOpenRootDialog() )
			{
				if( !m_pCmdMenuDialogManager->GetDialog(m_vecCmdMenuRootDialogID[cmdmenu_misc])->IsOpen() )
                    m_pCmdMenuDialogManager->OpenRootDialog( m_vecCmdMenuRootDialogID[cmdmenu_misc] );
			}
		}
        break;

	case ACTION_MENU_OPEN_MENU_1:
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
		{
			// open order command menu by up key
			if( m_pCmdMenuDialogManager
			 && cmdmenu_order < m_vecCmdMenuRootDialogID.size()
			 && !m_pCmdMenuDialogManager->IsThereAnyOpenRootDialog() )
			{
				if( !m_pCmdMenuDialogManager->GetDialog(m_vecCmdMenuRootDialogID[cmdmenu_order])->IsOpen() )
					m_pCmdMenuDialogManager->OpenRootDialog( m_vecCmdMenuRootDialogID[cmdmenu_order] );
			}
		}
		break;

	default:
		break;
	}


/*	if( ACTION_MENU_OPEN_MENU_0 <= input.ActionCode
	 && input.ActionCode <= ACTION_MENU_OPEN_MENU_7 )
	{
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
		{
			int root_dlg_index = input.ActionCode - ACTION_MENU_OPEN_MENU_0;
			if( m_pCmdMenuDialogManager.get()
			 && root_dlg_index < (int)m_vecCmdMenuRootDialogID.size() )
			{
				m_pCmdMenuDialogManager->OpenRootDialog( m_vecCmdMenuRootDialogID[root_dlg_index] );
				return true;
			}
		}
	}*/

	return false;
}


void CBE_PlayerPseudoAircraft::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_Player::Serialize( ar, version );

	if( ar.GetMode() == IArchive::MODE_INPUT )
	{
		SetAircraftState( STATE_NORMAL );
		SetMissionState( MSTATE_NOT_IN_MISSION );
	}

	ar & m_NozzleFlames.m_NozzleFlame;

	for( int i=0; i<NUM_EXTRA_BASE_ENTITIES; i++ )
		ar & m_aExtraBaseEntity[i];
}
