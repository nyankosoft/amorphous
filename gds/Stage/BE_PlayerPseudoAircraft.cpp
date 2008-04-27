
#include "BE_PlayerPseudoAircraft.h"
#include <Stage/PlayerInfo.h>
#include <Stage/EntitySet.h>
#include <Stage/CopyEntityDesc.h>
#include <Stage/Stage.h>
#include <Stage/ScreenEffectManager.h>
#include <Stage/HUD_PlayerAircraft.h>
#include <Stage/GameMessage.h>
#include "Input/InputHandler_PlayerPAC.h"
#include "Item/WeaponSystem.h"
#include "Item/GI_MissileLauncher.h"
#include "ViewFrustumTest.h"

#include "3DMath/Vector3.h"
#include "3DMath/MathMisc.h"

#include "GameCommon/BasicGameMath.h"
#include "GameCommon/GameMathMisc.h"
#include "GameCommon/MTRand.h"
#include "GameCommon/RandomDirectionTable.h"
#include "GameCommon/ShockWaveCameraEffect.h"
#include "GameCommon/Timer.h"
#include "GameCommon/PseudoAircraftSimulator.h"
#include "GameCommon/MeshBoneController_Aircraft.h"

// added for laser dot casting test
#include "3DCommon/3DGameMath.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/D3DXMeshObject.h"
#include "3DCommon/D3DXSMeshObject.h"
#include "3DCommon/Shader/Shader.h"
#include "3DCommon/Shader/ShaderManager.h"
#include "Support/memory_helpers.h"
#include "trace.h"
#include "OverlapTestAABB.h"

#include "JigLib/JL_PhysicsActor.h"
#include "JigLib/JL_LineSegment.h"

#include "UI/ui.h"

#include "Support/Log/DefaultLog.h"
#include "Support/Macro.h"
#include "Support/Vec3_StringAux.h"

#include "../../Project3/FlightGame/src/Stage/FG_AIAircraftEntityDesc.h"
#include "../../Project3/FlightGame/src/EntityGroups.h"


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

CBE_PlayerPseudoAircraft::CBE_PlayerPseudoAircraft()
:
m_pAircraft(NULL)
{
//	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_PLAYER;

//	m_ActorDesc.ActorFlag |= JL_ACTOR_APPLY_NO_ANGULARIMPULSE;

	// pseudo aircraft will not be affected by the gravity
	// since it does not simulate the physics
	m_ActorDesc.ActorFlag |= JL_ACTOR_KINEMATIC;

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

	m_pFocusedTarget = NULL;

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
}


void CBE_PlayerPseudoAircraft::Init()
{
	CBE_Player::Init();

	RaiseEntityFlag( BETYPE_PLAYER | BETYPE_FLOATING );

	float near_clip	= 1.0f;
	float far_clip	= 50000.0f;

	m_Camera.SetFOV( D3DX_PI / 4.0f );
//	m_Camera.SetFOV( D3DX_PI / 8.0f );
	m_Camera.SetNearClip( near_clip );
	m_Camera.SetFarClip( far_clip );
	m_Camera.SetAspectRatio( 640.0f / 480.0f );

//	for( int i=0; i<CWeaponSystem::NUM_WEAPONSLOTS; i++ )
//		PLAYERINFO.GetWeaponSystem()->GetWeaponSlot(i).LocalPose.vPosition = Vector3( 0.0f, -1.2f, 12.0f );

	Init3DModel();

	// TODO: set base entity names from text file
	m_aExtraBaseEntity[EBE_SMOKE_TRAIL].Handle.SetBaseEntityName( "bsmkt_L" );
	m_aExtraBaseEntity[EBE_FLAME_TRAIL].Handle.SetBaseEntityName( "FlameTraceL2" );
//	m_aExtraBaseEntity[EBE_EXPLOSION].Handle.SetBaseEntityName( "" );

	LoadBaseEntity( m_aExtraBaseEntity[EBE_SMOKE_TRAIL].Handle );
	LoadBaseEntity( m_aExtraBaseEntity[EBE_FLAME_TRAIL].Handle );
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


//void CBE_PlayerPseudoAircraft::EnableNightVision( bool enable )
void CBE_PlayerPseudoAircraft::SetVisionMode( int vision_mode )
{
	CScreenEffectManager* pScreenEffectManager = m_pStage->GetScreenEffectManager();

	switch( vision_mode )
	{
	case CPlayerVisionMode::Normal:
	default:
		pScreenEffectManager->ClearEffectFlag( ScreenEffect::PseudoNightVision );
		pScreenEffectManager->ClearEffectFlag( ScreenEffect::MonochromeColor );
		pScreenEffectManager->SetMonochromeColorOffset( 0.0f, 0.0f, 0.0f );
		break;

	case CPlayerVisionMode::NightVision:
		pScreenEffectManager->FadeInFrom( 0xFF000000, 0.6f, AlphaBlend::One );
		pScreenEffectManager->RaiseEffectFlag( ScreenEffect::PseudoNightVision );
		pScreenEffectManager->RaiseEffectFlag( ScreenEffect::MonochromeColor );
		pScreenEffectManager->SetMonochromeColorOffset( -0.1f, 0.3f, -0.1f );
		break;

	case CPlayerVisionMode::Thermal:
		// NOT IMPLEMENTED
		break;
	}
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

	m_pAircraft->ResetAircraftControls();

//	Vector3 vInitVelocity = Vector3(0,0,5);
//	pCopyEnt->SetVelocity( vInitVelocity );

//	Matrix34 init_pose( Vector3( 0,1200, -100 ), Matrix33Identity() );
//	pCopyEnt->SetWorldPose( init_pose );

	if( pCopyEnt->pPhysicsActor )
	{
		pCopyEnt->pPhysicsActor->SetWorldPose( pCopyEnt->GetWorldPose() );
		pCopyEnt->pPhysicsActor->SetVelocity( pCopyEnt->Velocity() );
//		pCopyEnt->pPhysicsActor->SetPosition( init_pose.vPosition );
//		pCopyEnt->pPhysicsActor->SetOrientation( init_pose.matOrient );
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

	// mission started - set the mission state
	SetMissionState( MSTATE_IN_MISSION );

	LOG_PRINT( " - end" );
}


void CBE_PlayerPseudoAircraft::Move( CCopyEntity *pCopyEnt )
{
	if( !pCopyEnt->pPhysicsActor )
	{
		assert( pCopyEnt->pPhysicsActor );
		return;
	}

	CJL_PhysicsActor& rPhysicsActor = *pCopyEnt->pPhysicsActor;

/*	// update world aabb here so that collision check would not be performed between the player and his own bullets
	// when he is stepping back while firing
	pCopyEnt->world_aabb.TransformCoord( pCopyEnt->local_aabb, pCopyEnt->Position() );
*/

}

/*
void CBE_PlayerPseudoAircraft::UpdateObjectsInSight( CCopyEntity* pCopyEnt )
{
    static vector<CCopyEntity *> s_vecpVisibleEntity;

	// clear any previous data
	s_vecpVisibleEntity.resize(0);
	m_vecpVisibleEntity.resize(0);

	// check visible entities
	CViewFrustumTest vf_test;
//	vf_test.m_Flag = VFT_IGNORE_NOCLIP_ENTITIES;
	vf_test.SetCamera( &m_Camera );
	vf_test.SetBuffer( s_vecpVisibleEntity );

	// collect entities that are in the view frustum volume of the sensor camera
	m_pStage->GetVisibleEntities( vf_test );

	int i, iNumVisibleEntities = vf_test.GetNumVisibleEntities();
	for( i=0; i<iNumVisibleEntities; i++ )
	{
		CCopyEntity *pTarget = vf_test.GetEntity(i);

		if( pTarget == pCopyEnt )
			continue;

//		if( pTarget->pBaseEntity->GetArchiveObjectID() == CBaseEntity::BE_ENEMYAIRCRAFT )
		if( !pTarget->bNoClip )
			m_vecpVisibleEntity.push_back( pTarget );

//			if( pTarget->bNoClip )	continue;	// don't target a no-clip entity
//		if( pTarget->sGroupID == CE_GROUP_INVALID )	continue;	// don't target an entity that has an invalid group ID
//		if( pTarget->sGroupID != pCopyEnt->sGroupID )
//		{
//			pCopyEnt->pTarget = vf_test.GetEntity(i);
//			break;
//		}
	}
}
*/


bool CBE_PlayerPseudoAircraft::SetAircraft()
{
	CGI_Aircraft* pAircraft = PLAYERINFO.GetAircraft();

	if( !pAircraft )
	{
		LOG_PRINT_WARNING( " - an aircraft item is not selected." );
		return false;
	}

	m_pAircraft = pAircraft;

	m_CameraLocalPose = m_pAircraft->GetCockpitLocalPose();

	// set release positions for missile launchers
	m_pAircraft->InitWeaponSystem();

//	m_pAircraft->WeaponSystem().SetProjectileGroup( CE_GROUP_PLAYER_PROJECTILE );
	m_pAircraft->WeaponSystem().SetProjectileGroup( GetEntityGroupID( m_ProjectileEntityGroup ) );

	// get mesh filename from the aircraft item and load the mesh
	m_MeshProperty.Release();

	m_MeshProperty.m_MeshObjectHandle = pAircraft->GetMeshObjectContainer().m_MeshObjectHandle;
///	m_MeshProperty.m_str3DModelFileName	= pAircraft->GetMeshObjectContainer().m_MeshObjectHandle.filename;
//	m_MeshProperty.m_SpecTex.filename	= pAircraft->GetMeshObjectHolder().m_SpecTex.filename;

	m_MeshProperty.LoadMeshObject();

	if( m_MeshProperty.m_MeshObjectHandle.GetMeshObject()
	 && m_MeshProperty.m_MeshObjectHandle.GetMeshType() == CD3DXMeshObjectBase::TYPE_SMESH )
	{
		m_pAircraft->ResetMeshController();
		m_pAircraft->InitMeshController( (CD3DXSMeshObject *)m_MeshProperty.m_MeshObjectHandle.GetMeshObject() );
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
//		CGI_Weapon* pWeapon = m_pAircraft->WeaponSystem().GetPrimaryWeapon();
//		if( pWeapon && pWeapon->GetArchiveObjectID() == CGameItem::ID_MISSILELAUNCHER )
//			effective_range = ((CGI_MissileLauncher *)pWeapon)
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

		if( vecpEntityBuffer[i]->GroupIndex == CE_GROUP_ENEMY )
		{
            m_vecFocusCandidate.push_back( FocusCandidate( vecpEntityBuffer[i], 1.0f ) );
		}

//		score = target.pos
//		m_RadarInfo.m_vec
	}

	if( m_vecFocusCandidate.size() == 0 )
	{
		// no candidate entities to focus on
		m_CurrentTargetFocusIndex = -1;
		m_pFocusedTarget = NULL;
		return;
	}
	else
	{
		// if there is at least one target on radar and no focus is set on any target
		// right now, set the focus on the first target in m_vecFocusCandidate
		if( !IsValidEntity(m_pFocusedTarget) /*&& 0 < m_vecFocusCandidate.size()*/ )
		{
			m_pFocusedTarget = m_vecFocusCandidate[0].pEntity;
			m_CurrentTargetFocusIndex = 0;
		}
		else
		{
			size_t num_focus_candidates = m_vecFocusCandidate.size();
			for( i=0; i<num_focus_candidates; i++ )
			{
				if( m_pFocusedTarget == m_vecFocusCandidate[i].pEntity )
				{
					// keep the current focused target
					m_CurrentTargetFocusIndex = i;
					break;
				}
			}
			if( num_focus_candidates <= i )
			{
//				MsgBoxFmt( "changing target focus (entities: %d, valid: %d, candidates: %d)", num_entities, num_valid_entities, num_focus_candidates );
				// set a new target focus
				m_pFocusedTarget = m_vecFocusCandidate[0].pEntity;
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

		CGI_MissileLauncher *pLauncher = (CGI_MissileLauncher *)pWeapon;

		// set primary target to lock-on
		pLauncher->SetPrimaryTarget( m_pFocusedTarget );
	}
}


void CBE_PlayerPseudoAircraft::UpdateRadarInfo( CCopyEntity* pCopyEnt )
{
	static vector<CCopyEntity *> s_vecpEntityBuffer;
	s_vecpEntityBuffer.resize( 0 );

	float effective_radar_radius = 100000.0f;
	const float r = effective_radar_radius;//200000.0f;

	// cube with each edge 400[km]
	// TODO: use a proper bounding-box that contains the entire stage
	AABB3 aabb = AABB3(
		Vector3(-1,-1,-1) * effective_radar_radius + pCopyEnt->Position(),
		Vector3( 1, 1, 1) * effective_radar_radius + pCopyEnt->Position() );	// m_pStage->GetAABB();

	COverlapTestAABB overlap_test( aabb, &s_vecpEntityBuffer, ENTITY_GROUP_MIN_ID /*CE_GROUP_GENERAL*/ );

	m_pStage->GetEntitySet()->GetOverlappingEntities( overlap_test );

	UpdateFocusCandidateTargets( s_vecpEntityBuffer );

	// clear all the previous target info
	m_RadarInfo.ClearTargetInfo();

	Vector3 vCamFwdDir = m_Camera.GetFrontDirection();// pCopyEnt->GetDirection();

	// get missile laucnher on the current aircraft
	CGI_Weapon *pPrimaryWeapon = m_pAircraft->WeaponSystem().GetPrimaryWeapon();
	CGI_MissileLauncher* pLauncher = NULL;
	if( pPrimaryWeapon && pPrimaryWeapon->GetArchiveObjectID() == CGameItem::ID_MISSILELAUNCHER )
		pLauncher = (CGI_MissileLauncher *)pPrimaryWeapon;

	// set primary target to lock-on
	if( pLauncher && IsValidEntity(m_pFocusedTarget) )
		pLauncher->SetPrimaryTarget( m_pFocusedTarget );

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

		default:

			switch( pEntity->GroupIndex )
			{
			case CE_GROUP_PLAYER:      tgt_type |= HUD_TargetInfo::PLAYER; break;
			case CE_GROUP_PLAYER_ALLY: tgt_type |= HUD_TargetInfo::ALLY;   break;
			case CE_GROUP_ENEMY:       tgt_type |= HUD_TargetInfo::ENEMY;  break;
			default:
				break;
			}

			switch( pEntity->GetEntityTypeID() )
			{
			case CFG_EntityTypeID::AI_AIRCRAFT:
				tgt_type |= HUD_TargetInfo::TGT_AIR;
				break;
			case CFG_EntityTypeID::AI_VEHICLE:
			case CFG_EntityTypeID::AI_SHIP:
				tgt_type |= HUD_TargetInfo::TGT_SURFACE;
				break;
			default:
				break;
			}

			break;

/*		case BE_ENEMYAIRCRAFT:
			tgt_type = HUD_TargetInfo::ENEMY | HUD_TargetInfo::TGT_AIR;
			break;
		case BE_ENEMYSHIP:
			tgt_type = HUD_TargetInfo::ENEMY | HUD_TargetInfo::TGT_SURFACE;
			break;
		case BE_HOMINGMISSILE:
			tgt_type = HUD_TargetInfo::MISSILE;
			break;
		default:
			tgt_type = 0;
			break;*/
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
	}

	m_State = STATE_NORMAL;
	for( i=0; i<num_entities; i++ )
	{
		CCopyEntity *pEntity = s_vecpEntityBuffer[i];

		if( !IsValidEntity( pEntity ) )
			continue;

		if( pEntity->pBaseEntity->GetArchiveObjectID() == BE_HOMINGMISSILE )
		{
			if( pEntity->pTarget == pCopyEnt )
				m_State = STATE_MISSILE_APPROACHING;
		}
	}
}


void CBE_PlayerPseudoAircraft::Act( CCopyEntity* pCopyEnt )
{
	CBE_Player::Act( pCopyEnt );

	if( !m_pAircraft )
		return;

	if( pCopyEnt->fLife <= 0 )
	{
		// player aircraft is destroyed - call update routines of child entities
		for( int i=0; i<pCopyEnt->GetNumChildren(); i++ )
			pCopyEnt->GetChild(i)->Act();

		return;
	}

	UpdateRadarInfo( pCopyEnt );

	CGI_Weapon::ms_pStage = m_pStage;
///	m_pAircraft->Update( TIMER.GetFrameTime() );

	int nozzle_frame_entity_offset = m_pLaserDotEntity ? 1 : 0;

	m_NozzleFlames.UpdateNozzleFlames( pCopyEnt, nozzle_frame_entity_offset, m_pAircraft );

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

//		GAMESOUNDMANAGER.Play3D( m_aDamageSound[rGameMessage.s1][iVariation], pCopyEnt_Self->Position() );

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
				pCopyEnt_Self->EntityFlag |= BETYPE_USE_PHYSSIM_RESULTS;

				// create flame & smoke trail
				for( int i=0; i<pCopyEnt_Self->GetNumChildren(); i++ )
				{
					m_pStage->TerminateEntity( pCopyEnt_Self->apChild[i] );
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
		if( PLAYERINFO.SupplyItem( rGameMessage.pcStrParam, (int)(rGameMessage.fParam1) ) )
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
	// detach player base entity from the PLAYERINFO
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

#else

	m_Camera.SetPose( pCopyEnt->GetWorldPose() );

#endif

	m_Camera.UpdateVFTreeForWorldSpace();

	m_CameraOrient.target = pCopyEnt->GetWorldPose().matOrient * matPitch * matHeading;

//	UpdateCameraEffect();
}


void CBE_PlayerPseudoAircraft::UpdatePhysics( CCopyEntity *pCopyEnt, float dt )
{
	if( !pCopyEnt->pPhysicsActor )
	{
		assert( pCopyEnt->pPhysicsActor );
		return;
	}

	CJL_PhysicsActor& rPhysicsActor = *pCopyEnt->pPhysicsActor;

	// wake up the actor if it's sleeping
	if( rPhysicsActor.GetActivityState() == CJL_PhysicsActor::FROZEN )
		rPhysicsActor.ApplyWorldImpulse( pCopyEnt->GetUpDirection() * 0.0001f );

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

	pCopyEnt->pPhysicsActor->SetWorldPose( pseudo_simulator.GetWorldPose() );
	pCopyEnt->pPhysicsActor->SetVelocity( pseudo_simulator.GetVelocity() );

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
	 && m_pCmdMenuDialogManager.get() )
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

	CGI_Weapon::ms_pStage = m_pStage;
	if( m_pAircraft->HandleInput( input.ActionCode, input_type, input.fParam ) )
		return true;

//	CPseudoAircraftSimulator& aircraft = m_pAircraft->GetPseudoSimulator();
	HUD_PlayerBase *pHUD = NULL;

	static const int cmdmenu_misc = 0;
	static const int cmdmenu_order = 1;

	switch( input.ActionCode )
	{
/*	case ACTION_MOV_PITCH_ACCEL:
		aircraft.SetPitchAccel( input.fParam * 5.0f );
		break;

	case ACTION_MOV_ROLL_ACCEL:
		aircraft.SetRollAccel( input.fParam * 5.0f );
		break;

	case ACTION_MOV_YAW_ACCEL:
		aircraft.SetYawAccel( input.fParam * 1.5f );
		break;
*/
	case ACTION_MOV_LOOK_RIGHT:
		m_CamHeading.target = input.fParam * PI;
		break;

	case ACTION_MOV_LOOK_UP:
		m_CamPitch.target = input.fParam * PI * 0.5f;
		break;

/*	case ACTION_MOV_BOOST:
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
			SetAccel( m_fBoostAccel );
		else if( input.type == SPlayerEntityAction::KEY_RELEASED )
			SetAccel( m_fAccel );
		return true;

	case ACTION_MOV_BRAKE:
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
			SetAccel( m_fBrakeAccel );
		else if( input.type == SPlayerEntityAction::KEY_RELEASED )
			SetAccel( m_fAccel );
		return true;
*/
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

		if(	(pHUD = PLAYERINFO.GetHUD())
		 && (pHUD->GetType() == HUD_PlayerBase::TYPE_AIRCRAFT) )
		{
            HUD_PlayerAircraft *pAircratHUD = (HUD_PlayerAircraft *)pHUD;
 
			if( input.type == SPlayerEntityAction::KEY_PRESSED )
			{
				pAircratHUD->DisplayGlobalRadar( true );
			}
			else
			{
				pAircratHUD->DisplayGlobalRadar( false );
			}
		}
		break;

	case ACTION_MENU_OPEN_MENU_0:
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
		{
			// open misc command menu by down key
			if( m_pCmdMenuDialogManager.get() && cmdmenu_misc < m_vecCmdMenuRootDialogID.size() )
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
			if( m_pCmdMenuDialogManager.get() && cmdmenu_order < m_vecCmdMenuRootDialogID.size() )
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


void CBE_PlayerPseudoAircraft::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	Init3DModel();
}


void CBE_PlayerPseudoAircraft::ReleaseGraphicsResources()
{
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
