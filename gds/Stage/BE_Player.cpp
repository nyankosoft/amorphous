#include "BE_Player.hpp"
#include "PlayerInfo.hpp"
#include "EntitySet.hpp"
#include "CopyEntityDesc.hpp"
#include "Stage.hpp"
#include "Input/InputHandler_PlayerShip.hpp"
#include "Stage/ScreenEffectManager.hpp"
#include "Stage/BE_LaserDot.hpp"
#include "GameMessage.hpp"
#include "trace.hpp"
#include "HUD_PlayerBase.hpp"
#include "SubDisplay.hpp"

#include "Input/InputHub.hpp"
#include "Support/MTRand.hpp"
#include "GameCommon/ShockWaveCameraEffect.hpp"
#include "Item/WeaponSystem.hpp"

#include "Sound/SoundManager.hpp"

// added for laser dot casting test
#include "Support/VectorRand.hpp"
#include "Support/Timer.hpp"
#include "Support/Log/StateLog.hpp"
#include "Support/Profile.hpp"
#include "Utilities/TextFileScannerExtensions.hpp"
#include "GUI/GM_DialogManager.hpp"
#include "GUI/GM_ControlRendererManager.hpp"
#include "GUI/InputHandler_Dialog.hpp"

#include "JigLib/JL_PhysicsActor.hpp"

using namespace std;
using namespace boost;


//#define APPLY_PHYSICS_TO_PLAYER_SHIP


int gs_InputHandlerIndex = 1;


CBE_Player::CBE_Player()
{
	this->m_BoundingVolumeType = BVTYPE_AABB;
//	this->m_BoundingVolumeType = BVTYPE_OBB;
	this->m_bNoClip = false;
//	this->m_fRadius = 0.2f;

//	m_PlayerEntityHandle.SetBaseEntityName( "PlayerShip" );

	m_LaserDot.SetBaseEntityName( "ldt" );
	m_HeadLight.SetBaseEntityName( "DynamicLightX" );

	m_pLaserDotEntity = NULL;
	m_pHeadLightEntity = NULL;
	m_bHeadLightOn = false;
	
	m_pInputHandler = NULL;

	m_ProjectileEntityGroup.SetID( ENTITY_GROUP_MIN_ID );

	m_pShockWaveEffect = NULL;

	m_CameraLocalPose.Identity();

/*
#ifdef APPLY_PHYSICS_TO_PLAYER_SHIP 

//	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_PLAYER;

	m_ActorDesc.ActorFlag |= JL_ACTOR_APPLY_NO_ANGULARIMPULSE;
	RaiseEntityFlag( BETYPE_RIGIDBODY );

//	m_fStaticFriction = 0.8f;
//	m_fDynamicFriction = 0.8f;

#else

	m_EntityFlag &= ~BETYPE_RIGIDBODY;

#endif
*/
}


CBE_Player::~CBE_Player()
{
	SafeDelete( m_pShockWaveEffect );

	SafeDelete( m_pInputHandler );
}


void CBE_Player::Init()
{
	RaiseEntityFlag( BETYPE_PLAYER | BETYPE_FLOATING );

	InitDamageSound();

//	m_fDefaultFOV = 3.141592f / 3.0f;

	m_pShockWaveEffect = new CShockWaveCameraEffect;

	m_Camera.SetFarClip( 1000.0f );

	for( int i=0; i<CWeaponSystem::NUM_WEAPONSLOTS; i++ )
	{
		SinglePlayerInfo().GetWeaponSystem()->GetWeaponSlot(i).LocalPose.vPosition = Vector3(0,0,2.5f);
	}
}


void CBE_Player::InitCopyEntity(CCopyEntity* pCopyEnt)
{
	m_pPlayerCopyEntity = pCopyEnt;

	pCopyEnt->fLife = 100.0f;
//	pCopyEnt->GroupIndex = CE_GROUP_PLAYER;

	float& rfTimeAfterDeath = pCopyEnt->f4;
	rfTimeAfterDeath = -1;

	CCopyEntityDesc laser_dot;
	laser_dot.pBaseEntityHandle = &m_LaserDot;
	laser_dot.pParent = pCopyEnt;
	laser_dot.SetWorldPosition( pCopyEnt->GetWorldPosition() );
	laser_dot.vVelocity = Vector3(0,0,0);

	// generate laser dot entity and store its pointer to 'm_pLaserDotEntity'
	m_pLaserDotEntity = m_pStage->CreateEntity( laser_dot );

	SinglePlayerInfo().SetPlayerBaseEntity( this );

//	m_pStage->SetState( CStage::PLAYER_IN_STAGE );

	// set the input handler for the player entity operations
	SafeDelete( m_pInputHandler );
	m_pInputHandler = CreatePlayerInputHandler();
	InputHub().PushInputHandler( gs_InputHandlerIndex, m_pInputHandler );

//	SinglePlayerInfo().SetInputHandlerForPlayerShip();
}


void UpdateStateLogs(CCopyEntity *pEntity)
{
	StateLog.Update( 2, "pos" + to_string(pEntity->GetWorldPosition()) );
	StateLog.Update( 3, "dir" + to_string(pEntity->Velocity()) );
//	StateLog.Update(  , "dir" + to_string(pEntity->GetDirection_Right()) );
	StateLog.Update( 4, "speed: " + to_string(pEntity->fSpeed) + "[m/s]" );

}


void CBE_Player::Act(CCopyEntity* pCopyEnt)
{
	float& rfTimeAfterDeath = pCopyEnt->f4;
	if( 0 <= rfTimeAfterDeath )
	{
		PlayerDead(pCopyEnt);
		return;
	}

	m_pPlayerCopyEntity = pCopyEnt;

	float frametime = m_pStage->GetFrameTime();


	// calculate motion during one frame
//#ifdef APPLY_PHYSICS_TO_PLAYER_SHIP 
//	MoveEx(pCopyEnt);
//#else
	Move(pCopyEnt);
//#endif

///	if( !m_bTriggerSafetyEnabled )
///	{
//		LaserAimingDevice( true );

		// update weapon system
		SinglePlayerInfo().GetWeaponSystem()->UpdateWorldProperties( pCopyEnt->GetWorldPose(), pCopyEnt->Velocity(), Vector3(0,0,0) );

		SinglePlayerInfo().GetWeaponSystem()->Update( frametime );

	
///	}
///	else
//		LaserAimingDevice( false );

	CInputHandler_PlayerBase *pInputHandler = m_pInputHandler;
	if( pInputHandler
	 && 0 < pInputHandler->GetActionState(ACTION_ATK_UNLOCK_TRIGGER_SAFETY) )
		LaserAimingDevice( true );
	else
		LaserAimingDevice( false );

	if( m_pLaserDotEntity )
	{
		m_pLaserDotEntity->v1 = m_vLocalMuzzlePos;
		m_pLaserDotEntity->Act();
	}

	if( m_pHeadLightEntity )
	{
		// update head position
		// head light is represented by placing the point light in front of the player
		// make sure the point light for head light does not penetrate geometry
		float fMargin = 1.5f;
		float fForward = 6.0f + fMargin;
		float fUp = -0.35f;
		Vector3 vStart, vGoal, vForward;
		STrace tr;
		tr.pSourceEntity = pCopyEnt;
		tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;//|TRACETYPE_GET_MATERIAL_INFO;
		tr.bvType = BVTYPE_DOT;
		vStart = pCopyEnt->GetWorldPosition() + pCopyEnt->GetUpDirection() * fUp;
		vGoal = vStart + pCopyEnt->GetDirection() * fForward;
		tr.pvStart = &vStart;
		tr.pvGoal = &vGoal;
		m_pStage->ClipTrace( tr );

		if( tr.fFraction < 1 )
		{
			// point light sphere is blocked before it reaches maximum distance
			if( fMargin < tr.fFraction * fForward )
                vForward = pCopyEnt->GetDirection() * ( tr.fFraction * fForward - fMargin );
			else
				vForward = pCopyEnt->GetDirection() * tr.fFraction * fForward * 0.9f;
		}
		else
			vForward = pCopyEnt->GetDirection() * ( fForward - fMargin );

		m_pHeadLightEntity->SetWorldPosition( vStart + vForward );
		m_pHeadLightEntity->Act();
	}

	if( m_pShockWaveEffect )
		m_pShockWaveEffect->Update( frametime );

	if( m_pCmdMenuDialogManager.get() )
		m_pCmdMenuDialogManager->Update( frametime );

	if( GetHUD() )
		GetHUD()->Update( frametime );

	UpdateStateLogs(pCopyEnt);

	return;
}


void CBE_Player::Move( CCopyEntity *pCopyEnt )
{
	// update world aabb here so that collision check would not be performed between the player and his own bullets
	// when he is stepping back while firing 
	pCopyEnt->world_aabb.TransformCoord( pCopyEnt->local_aabb, pCopyEnt->GetWorldPosition() );
}


bool CBE_Player::HandleInput( SPlayerEntityAction& input )
{

	switch( input.ActionCode )
	{
	case ACTION_MISC_TOGGLE_HEADLIGHT:
		if( input.type == ITYPE_KEY_PRESSED )
			ToggleHeadLight();
		return true;
	}

	return false;
}


void CBE_Player::ToggleHeadLight()
{
	if( m_bHeadLightOn == false )
	{
		// turn on the light
		CCopyEntityDesc light;
		light.pBaseEntityHandle = &m_HeadLight;	// m_pStage->FindBaseEntity( "DynamicLightX" );
		Vector3 vWorldPos = m_pPlayerCopyEntity->GetWorldPosition()
			              + m_pPlayerCopyEntity->GetDirection()
					      + m_pPlayerCopyEntity->GetUpDirection() * (-0.35f);

		light.SetWorldPosition( vWorldPos );

		m_pHeadLightEntity = m_pStage->CreateEntity( light );

		m_bHeadLightOn = true;
	}
	else
	{	// turn off the light
		SGameMessage msg;
		msg.effect = GM_TERMINATE;
		SendGameMessageTo( msg, m_pHeadLightEntity );
		m_pHeadLightEntity = NULL;
		m_bHeadLightOn = false;
	}
}


void CBE_Player::OnDestroyingEnemyEntity( SGameMessage& msg )
{
	shared_ptr<CCopyEntity> pSenderEntity = msg.sender.Get();

	if( pSenderEntity )
	{
		CCopyEntity& destroyed_entity = *(pSenderEntity.get());
		KillReport rep;
		rep.base_name    = destroyed_entity.pBaseEntity->GetName();
		rep.entity_name  = destroyed_entity.GetName();
		rep.vWorldPos    = destroyed_entity.GetWorldPosition();
		rep.time         = m_pStage->GetElapsedTimeMS();
		rep.score        = (int)(msg.fParam1);
		m_CombatRecord.m_vecKillReport.push_back( rep );
	}
}


void CBE_Player::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
	float& rfLife = pCopyEnt_Self->fLife;
	float& rfTimeAfterDeath = pCopyEnt_Self->f4;
	SGameMessage msg;
	int iVariation;

	switch( rGameMessage.effect )
	{
	case GM_DAMAGE:
		// play a damage sound according to the type of damage
		iVariation = PLYAER_DAMAGESOUND_VARIATION * rand() / RAND_MAX;
		if( PLYAER_DAMAGESOUND_VARIATION <= iVariation ) iVariation = PLYAER_DAMAGESOUND_VARIATION - 1;
		SoundManager().PlayAt( m_aDamageSound[rGameMessage.s1][iVariation], pCopyEnt_Self->GetWorldPosition() );

		// flash screen with red
		m_pStage->GetScreenEffectManager()->FadeInFrom( 0x40F82000, 0.20f, AlphaBlend::InvSrcAlpha );

///		rfLife -= rGameMessage.fParam1;
		if( rfLife <= 0 && rfTimeAfterDeath < 0 )
		{
			rfLife = 0;
			rfTimeAfterDeath = 0;
			pCopyEnt_Self->vVelocity = Vector3(0,0,0);

			m_pStage->GetScreenEffectManager()->FadeOutTo( 0xFFFFFFFF, 2.5f, AlphaBlend::One );
		}

		if( rGameMessage.s1 == DMG_BLAST )
		{
/*			Vector3 vImpulse;
			vImpulse.x = RangedRand(  -1.0f,  1.0f );
			vImpulse.y = RangedRand( -500.0f, -400.0f );
			vImpulse.z = 0;
			m_pShockWaveEffect->AddForce( vImpulse );*/
			m_pShockWaveEffect->AddForce( rGameMessage.vParam * rGameMessage.fParam1 * 80.0f
				+ Vec3RandDir() * RangedRand( 0.05f, 0.25f ) );
		}
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
		msg.effect = GM_EFFECTACCEPTED;
		msg.sender = pCopyEnt_Self->Self();
		SendGameMessageTo( msg, rGameMessage.sender );
		break;

	case GM_AMMOSUPPLY:
//		if( SinglePlayerInfo().GetWeaponSystem()->SupplyAmmunition( rGameMessage.pcStrParam, (int)(rGameMessage.fParam1) ) )
		if( SinglePlayerInfo().SupplyItem( rGameMessage.pcStrParam, (int)(rGameMessage.fParam1) ) )
		{	// the item was accepted by the player
			msg.effect = GM_EFFECTACCEPTED;
			msg.sender = pCopyEnt_Self->Self();
			SendGameMessageTo( msg, rGameMessage.sender );
		}
		break;

	case GM_DESTROYED:
		OnDestroyingEnemyEntity( rGameMessage );
		break;

/*	case GM_DOORKEYITEM:
		strcpy( m_acDoorKeyCode[m_iNumDoorKeyCodes++], rGameMessage.pcStrParam );
		msg.effect = GM_EFFECTACCEPTED;
		msg.sender = pCopyEnt_Self->Self();
		SendGameMessageTo( msg, rGameMessage.pSenderEntity );
		break;

	case GM_KEYCODE_REQUEST:
		int i;
		for( i=0; i<m_iNumDoorKeyCodes; i++ )
		{
			if( strcmp(m_acDoorKeyCode[i], rGameMessage.pcStrParam) == 0 )
			{
				msg.effect = GM_KEYCODE_INPUT;
				msg.pcStrParam = m_acDoorKeyCode[i];
				msg.sender = pCopyEnt_Self->Self();
				SendGameMessageTo( msg, rGameMessage.pSenderEntity );
			}
		}
		break;*/

	default:
		break;
	}

}


void CBE_Player::OnEntityDestroyed(CCopyEntity* pCopyEnt)
{
	if( SinglePlayerInfo().GetCurrentPlayerBaseEntity() == this )
		SinglePlayerInfo().SetPlayerBaseEntity( NULL );

	if( m_pInputHandler )
	{
		InputHub().RemoveInputHandler( gs_InputHandlerIndex, m_pInputHandler );
		SafeDelete( m_pInputHandler );
	}
}

/*
void CBE_Player::RenderStage(CCopyEntity* pCopyEnt)
{
	// set the camera and draw the scene
	// BeginScene() & EndScene() pairs are called inside the function
//	ProfileBegin( "render stage (player)" );

	CCamera* pCamera = GetCamera();

	if( pCamera )
		m_pStage->Render( *pCamera );

//	ProfileEnd( "render stage (player)" );

	// render player HUD

	SubMonitor::SetStage( m_pStage );

	SinglePlayerInfo().RenderHUD();

	// render command menu UI
	if( m_pCmdMenuDialogManager.get() )
        m_pCmdMenuDialogManager->Render();

	SubMonitor::SetStage( NULL );
}
*/

void CBE_Player::RenderStage(CCopyEntity* pCopyEnt)
{
	CCamera* pCamera = GetCamera();

	if( pCamera )
		m_pStage->Render( *pCamera );


	// render player HUD
	HUD_PlayerBase *pHUD = GetHUD();
	if( pHUD )
	{
		if( pHUD->GetSubDisplay() )
			pHUD->GetSubDisplay()->SetStage( m_pStage->GetWeakPtr() );

		pHUD->Render();
	}


	// render command menu UI
	if( m_pCmdMenuDialogManager )
        m_pCmdMenuDialogManager->Render();
}


void CBE_Player::CreateRenderTasks(CCopyEntity* pCopyEnt)
{
	// let hud create render tasks
	// - if the submonitor is enabled, it will create render tasks
	//   which renders the scene to a texture
//	GetHUD()->GetSubDisplay()->SetStage( m_pStage->GetWeakPtr() );

	if( GetHUD() )
		GetHUD()->CreateRenderTasks();

	// add render tasks necessary to render the stage
	m_pStage->CreateStageRenderTasks( GetCamera() );

//	RenderTaskProcessor.AddRenderTask( new CStageRenderTask( m_pStage->GetWeakPtr() ) );
}


void CBE_Player::UpdateCameraEffect( Vector3& vNewCameraPos, CCopyEntity* pCopyEnt )
{
	float fEyeHeight = 0.5f;

	// set pseudo motion blur effect if the camera is moving fast

	Matrix34 camera_pose;
	m_Camera.GetPose( camera_pose );

	// calc how much the camera has been rotated since the last frame
//	float right_dist = Vec3Dot( m_PrevCameraPose.matOrient.GetColumn(2), pCopyEnt->GetRightDirection() );
//	float up_dist    = Vec3Dot( m_PrevCameraPose.matOrient.GetColumn(2), pCopyEnt->GetUpDirection() );
	float right_dist = Vec3Dot( m_PrevCameraPose.matOrient.GetColumn(2), camera_pose.matOrient.GetColumn(0) );
	float up_dist    = Vec3Dot( m_PrevCameraPose.matOrient.GetColumn(2), camera_pose.matOrient.GetColumn(1) );

	const float frametime = m_pStage->GetFrameTime();

	right_dist = fabsf(right_dist)	/ frametime;
	up_dist    = fabsf(up_dist)		/ frametime;
///	right_dist = fabsf(right_dist);
///	up_dist    = fabsf(up_dist);

	float blur_factor = 5.0f;
	float th = 3.5f;

/*	if( th < right_dist || th < up_dist )
		m_pStage->GetScreenEffectManager()->SetBlurEffect( (right_dist - th) * blur_factor, (up_dist - th) * blur_factor );
	else
		m_pStage->GetScreenEffectManager()->ClearBlurEffect();
*/
	vNewCameraPos = pCopyEnt->GetWorldPosition()
		          + pCopyEnt->GetUpDirection() * fEyeHeight
				  + m_pShockWaveEffect->GetPosition();

	// save the camera pose
	m_Camera.GetPose( m_PrevCameraPose );
}


void CBE_Player::UpdateCamera( CCopyEntity* pCopyEnt )
{
	Vector3 vNewCameraPos;
    UpdateCameraEffect( vNewCameraPos, pCopyEnt );

	// update camera pose
//	m_Camera.UpdatePosition( pCopyEnt->GetWorldPosition() + pCopyEnt->GetUpDirection() * fEyeHeight,
/*	m_Camera.UpdatePosition( pCopyEnt->GetWorldPosition() + pCopyEnt->GetUpDirection() * fEyeHeight + m_pShockWaveEffect->GetPosition(),
		                     pCopyEnt->GetDirection(),
							 pCopyEnt->GetRightDirection(),
							 pCopyEnt->GetUpDirection() );
*/

	m_Camera.UpdatePosition( vNewCameraPos,
		                     pCopyEnt->GetDirection(),
							 pCopyEnt->GetRightDirection(),
							 pCopyEnt->GetUpDirection() );
}


void CBE_Player::PlayerDead(CCopyEntity* pCopyEnt)
{
	float& rfTimeAfterDeath = pCopyEnt->f4;
	rfTimeAfterDeath += m_pStage->GetFrameTime();

	if( m_pStage->GetFrameTime() == 0 )
		MessageBox(NULL, "frame time 0", "exception", MB_OK);

	if( 2.5f <= rfTimeAfterDeath )
	{
		rfTimeAfterDeath = -1;
		pCopyEnt->fLife = 100.0f;
		pCopyEnt->SetWorldPose( Matrix34Identity() );
		pCopyEnt->s1 = 0;

		m_pStage->GetScreenEffectManager()->FadeInFrom( 0xFF000000, 1.0f, AlphaBlend::InvSrcAlpha );

		return;
	}

	D3DXVECTOR3 vDir, vRight;
	D3DXMATRIX matRotY;
	float fRotAngle = m_pStage->GetFrameTime() * (2.5f - rfTimeAfterDeath) * (2.5f - rfTimeAfterDeath);
	D3DXMatrixRotationY( &matRotY, fRotAngle * 2.0f );
	D3DXVec3TransformCoord( &vDir, &pCopyEnt->GetDirection(), &matRotY);
	D3DXVec3TransformCoord( &vRight, &pCopyEnt->GetRightDirection(), &matRotY);
	pCopyEnt->SetDirection( vDir );
	pCopyEnt->SetDirection_Right( vRight );
}


void CBE_Player::InitDamageSound()
{
	m_aDamageSound[DMG_BULLET][0].SetResourceName( "DamageD00" );
	m_aDamageSound[DMG_BULLET][1].SetResourceName( "DamageD00" );
	m_aDamageSound[DMG_BULLET][2].SetResourceName( "DamageD00" );
	m_aDamageSound[DMG_BULLET][3].SetResourceName( "DamageD01" );
	m_aDamageSound[DMG_BULLET][4].SetResourceName( "DamageD01" );
	m_aDamageSound[DMG_BULLET][5].SetResourceName( "DamageH00" );

//	m_aDamageSound[DMG_BLAST][0].SetResourceName( "" );
//	m_aDamageSound[DMG_BLAST][1].SetResourceName( "" );
//	m_aDamageSound[DMG_BLAST][2].SetResourceName( "" );
}


void CBE_Player::Draw(CCopyEntity *pCopyEnt)
{
//	if( !m_bTriggerSafetyEnabled )
//		CastLaserDotX( pCopyEnt );
}


// function to experiment the laser dot effect cast by some aiming module
void CBE_Player::LaserAimingDevice( bool bLaserDot )
{
	if( !IsValidEntity(m_pLaserDotEntity) )
		return;

	if( bLaserDot )
		m_pLaserDotEntity->s1 |= BE_LASERDOT_ON;
	else
		m_pLaserDotEntity->s1 &= ~BE_LASERDOT_ON;
}


void CBE_Player::SetCommandMenuUI( CGM_DialogManagerSharedPtr pDlgMgr,
								   std::vector<int>& vecRootDialogID,
		                           CGM_DialogInputHandlerSharedPtr pDlgInputHandler )
{
	if( !pDlgMgr.get() )
		return;

	m_pCmdMenuDialogManager = pDlgMgr;

	const size_t num_max_root_dialogs = 4;
//	const size_t num_root_dialogs
//		= num_max_root_dialogs < vecRootDialogID.size() ? num_max_root_dialogs : vecRootDialogID.size();
//	m_vecCmdMenuRootDialogID.resize( num_root_dialogs );
//	for( size_t i=0; i<num_root_dialogs; i++ )
//		m_vecCmdMenuRootDialogID[i] = vecRootDialogID[i];

	m_vecCmdMenuRootDialogID = vecRootDialogID;
	if( num_max_root_dialogs < m_vecCmdMenuRootDialogID.size() )
	{
		m_vecCmdMenuRootDialogID.erase(
		m_vecCmdMenuRootDialogID.begin() + num_max_root_dialogs,
		m_vecCmdMenuRootDialogID.end() );
	}

	m_pCmdMenuInputHandler = pDlgInputHandler;
}


void CBE_Player::ReleaseGraphicsResources()
{
	CBaseEntity::ReleaseGraphicsResources();

//	SafeDelete( m_pLaserDotX );

}

void CBE_Player::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	CBaseEntity::LoadGraphicsResources( rParam );
}


bool CBE_Player::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	if( CBE_PhysicsBaseEntity::LoadSpecificPropertiesFromFile( scanner ) )
		return true;

	string group_name;

	if( scanner.TryScanLine( "CAMERA_LOCAL_POS", m_CameraLocalPose.vPosition ) ) return true;

	if( scanner.TryScanLine( "PROJECTILE_GROUP", group_name ) )
	{
		m_ProjectileEntityGroup.SetGroupName( group_name );
		return true;
	}

	return false;
}


void CBE_Player::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_PhysicsBaseEntity::Serialize( ar, version );
//	CBaseEntity::Serialize( ar, version );

	ar & m_LaserDot;
	ar & m_HeadLight;

	ar & m_CameraLocalPose;

	ar & m_ProjectileEntityGroup;

//	ar & ;
}
