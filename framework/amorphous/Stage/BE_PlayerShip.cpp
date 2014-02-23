#include "BE_PlayerShip.hpp"
#include "PlayerInfo.hpp"
#include "EntitySet.hpp"
#include "CopyEntityDesc.hpp"
#include "Stage.hpp"
#include "Input/InputHandler_PlayerShip.hpp"
#include "Stage/ScreenEffectManager.hpp"
#include "GameMessage.hpp"
#include "Support/MTRand.hpp"
#include "Support/VectorRand.hpp"
//#include "GameCommon/ShockWaveCameraEffect.hpp"
#include "trace.hpp"
//#include "Stage/BE_LaserDot.hpp"
//#include "Sound/SoundManager.hpp"


namespace amorphous
{



//#define APPLY_PHYSICS_TO_PLAYER_SHIP


CBE_PlayerShip::CBE_PlayerShip()
{
	this->m_BoundingVolumeType = BVTYPE_AABB;
//	this->m_BoundingVolumeType = BVTYPE_OBB;
	this->m_bNoClip = false;
//	this->m_fRadius = 0.2f;

//	m_PlayerEntityHandle.SetBaseEntityName( "PlayerShip" );

//	m_LaserDot.SetBaseEntityName( "ldt" );
//	m_HeadLight.SetBaseEntityName( "DynamicLightX" );

//	m_pLaserDotEntity = NULL;
//	m_pHeadLightEntity = NULL;
//	m_bHeadLightOn = false;


#ifdef APPLY_PHYSICS_TO_PLAYER_SHIP 

//	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_PLAYER;

	m_ActorDesc.ActorFlag |= JL_ACTOR_APPLY_NO_ANGULARIMPULSE;
	m_EntityFlag |= BETYPE_RIGIDBODY;

//	m_fStaticFriction = 0.8f;
//	m_fDynamicFriction = 0.8f;

#else

	m_EntityFlag &= ~BETYPE_RIGIDBODY;

#endif

	m_pShockWaveEffect = NULL;

}


CBE_PlayerShip::~CBE_PlayerShip()
{
}


void CBE_PlayerShip::Init()
{
	CBE_Player::Init();

	m_EntityFlag |= BETYPE_PLAYER | BETYPE_FLOATING;

	for( int i=0; i<6; i++ )
		m_afThrust[i] = 0.0f;

	m_fBoost = 0.0f;

	InitDamageSound();

//	m_fDefaultFOV = 3.141592f / 3.0f;

	m_vLocalMuzzlePos = Vector3(0.0f, -0.35f, 1.45f);


//	m_pShockWaveEffect = new ShockWaveCameraEffect;
}


void CBE_PlayerShip::InitCopyEntity(CCopyEntity* pCopyEnt)
{
	CBE_Player::InitCopyEntity( pCopyEnt );

	pCopyEnt->fLife = 100.0f;

	// generate laser dot entity and store its pointer to 'm_pLaserDotEntity'
//	m_pLaserDotEntity = m_pStage->CreateEntity( laser_dot );

	m_Camera.SetFarClip( 1000.0f );
}

/*
void CBE_PlayerShip::Act(CCopyEntity* pCopyEnt)
{
	return;
}*/


void CBE_PlayerShip::Move( CCopyEntity *pCopyEnt )
{

	// If player is in solid, try to get out of there.
	NudgePosition( pCopyEnt );

	float fFrameTime;
	if( 0.03125 < m_pStage->GetFrameTime() )
		fFrameTime = 0.03125;	// Don't use frametime when it's below certain value ( 32 < FPS )
	else
		fFrameTime = m_pStage->GetFrameTime();

	Vector3 vDir   = Vector3(0,0,1);
	Vector3 vRight = Vector3(1,0,0);
	Vector3 vUp    = Vector3(0,1,0);

//	float fForward, fRight, fUp;
	float fYawSpeedGain, fAbsYawSpeed, fAbsPitchSpeed, fSign;
	static float fYawSpeed   = 0;
	static float fPitchSpeed = 0;
	static float fRollSpeed = 0;
	static float fYaw = 0.0f;	// turn right / left	(unit: radian)
	static float fPitch = 0.0f;	// look up / down		(unit: radian)
	static float fRoll = 0.0f;	// bank right /left		(unit: radian)
	const float pi = (float)PI;

	// measure yaw -  difficult for control but seems more appropriate for a space ship
//	fYawSpeedGain
//		= ActionInput.GetAction(ACTION_MOV_TURN_R) * fFrameTime * pi
//		- ActionInput.GetAction(ACTION_MOV_TURN_L) * fFrameTime * pi
//		+ ActionInput.GetAction(ACTION_MOV_ABS_YAW) / 40.0f;

//	fYawSpeedGain = m_pInput->GetActionState(ACTION_MOV_ABS_YAW) / 40.0f;
	fYawSpeedGain = m_afRotationSpeedGain[PLAYERSHIP_ROTATION_YAW] / 40.0f;
	m_afRotationSpeedGain[PLAYERSHIP_ROTATION_YAW] = 0.0f;

	fYawSpeed += fYawSpeedGain;
	fAbsYawSpeed = fabsf(fYawSpeed);
	fSign = 0 < fYawSpeed ? 1.0f : -1.0f;
	ApplyFriction( fAbsYawSpeed, fAbsYawSpeed * 2.45f );
	fYawSpeed = fAbsYawSpeed * fSign;

	fYaw += fYawSpeed * fFrameTime; 


	// measure yaw - easier control but seems less like a space ship
/*	fYaw = fYaw
		+ ActionInput.GetAction(ACTION_MOV_TURN_R) * fFrameTime * pi
		- ActionInput.GetAction(ACTION_MOV_TURN_L) * fFrameTime * pi
		+ ActionInput.GetAction(ACTION_MOV_ABS_YAW) / 750.0f * pi;*/

	if( 2.0f * pi < fYaw )			fYaw -= 2.0f * pi;
	else if( fYaw < -2.0f * pi )	fYaw += 2.0f * pi;

	// measure roll - tilt a little when player turn right /left
	fRollSpeed -= fYawSpeedGain / 4.0f;
	float fAbsRollSpeed = fabsf(fRollSpeed);
	fSign = 0 < fRollSpeed ? 1.0f : -1.0f;
	ApplyFriction( fAbsRollSpeed, fAbsRollSpeed * 2.45f );
	fRollSpeed = fAbsRollSpeed * fSign;
	
	fRoll += fRollSpeed * fFrameTime;

	float fAbsRoll = fabsf(fRoll);
	fSign = 0 < fRoll ? 1.0f : -1.0f;
	if( fAbsYawSpeed < pi / 3.0f )
	{	// back to the horizontal position
		ApplyFriction( fAbsRoll, fAbsRoll * 2.2f );
		fRoll = fSign * fAbsRoll;
	}

	float fMaxRollAngle = pi / 9.0f;
	if( fMaxRollAngle < fRoll )
		fRoll = fMaxRollAngle;
	else if( fRoll < -fMaxRollAngle )
		fRoll = -fMaxRollAngle;


	// measure pitch -  difficult for control but seems more appropriate for a space ship
//	fPitchSpeed = fPitchSpeed
//		- ActionInput.GetAction(ACTION_MOV_LOOK_UP) * fFrameTime * pi
//		+ ActionInput.GetAction(ACTION_MOV_LOOK_DOWN) * fFrameTime * pi
//		- ActionInput.GetAction(ACTION_MOV_ABS_PITCH) / 40.0f;

//	fPitchSpeed = m_pInput->GetActionState(ACTION_MOV_ABS_PITCH) / 40.0f;
	fPitchSpeed = fPitchSpeed - m_afRotationSpeedGain[PLAYERSHIP_ROTATION_PITCH] / 40.0f;
	m_afRotationSpeedGain[PLAYERSHIP_ROTATION_PITCH] = 0.0f;

	fAbsPitchSpeed = fabsf(fPitchSpeed);
	fSign = 0 < fPitchSpeed ? 1.0f : -1.0f;
	ApplyFriction( fAbsPitchSpeed, fAbsPitchSpeed * 2.45f );
	fPitchSpeed = fAbsPitchSpeed * fSign;

	fPitch += fPitchSpeed * fFrameTime; 

	// measure pitch - easier control but seems less like a space ship
/*	fPitch = fPitch
		- ActionInput.GetAction(ACTION_MOV_LOOK_UP  ) * fFrameTime * pi
		+ ActionInput.GetAction(ACTION_MOV_LOOK_DOWN) * fFrameTime * pi
		- ActionInput.GetAction(ACTION_MOV_ABS_PITCH) / 750.0f * pi;	*/

	if( pi / 2.1f < fPitch )	fPitch = pi / 2.1f;
	if( fPitch < -(pi / 2.1f) ) fPitch = -( pi / 2.1f );

	// change the direction of the ship based on the 'fYaw' and 'fPitch' we have just measured

	// update yaw
	Matrix33 matRot( Matrix33RotationY(fYaw) );
	vDir = Vector3(0,0,1);
	vDir   = matRot * vDir;
	vRight = matRot * vRight;

	// update pitch
	matRot = Matrix33RotationAxis( fPitch, vRight );
	vDir = matRot * vDir;
	vUp  = matRot * vUp;

	// update roll
	matRot = Matrix33RotationAxis( fRoll, vDir );
	vRight = matRot * vRight;
	vUp    = matRot * vUp;

	pCopyEnt->SetDirection( vDir );
	pCopyEnt->SetDirection_Right( vRight );
	pCopyEnt->SetDirection_Up( vUp );


	//check whether the player is on or off the ground
	CategorizePosition( pCopyEnt );

	//Crouching movement	pCopyEnt.Crouch();

	//Changes 'pCopyEnt.Velocity' according to friction
	ApplyFriction( pCopyEnt, 3.2f );

	//measure thrust power (forward & right)
/*	fForward
	= ActionInput.GetAction(ACTION_MOV_FORWARD ) * 10.0f * ( 1.0f + 0.6f * ActionInput.GetAction(ACTION_MOV_RUN) )
	- ActionInput.GetAction(ACTION_MOV_BACKWARD) * 10.0f;
	fRight
	= ActionInput.GetAction(ACTION_MOV_SLIDE_R) * 10.0f
	- ActionInput.GetAction(ACTION_MOV_SLIDE_L) * 10.0f;//800.0f * fFrameTime;
	fUp
	= ActionInput.GetAction(ACTION_MOV_SLIDE_UP) * 10.0f
	- ActionInput.GetAction(ACTION_MOV_SLIDE_DOWN) * 10.0f;//800.0f * fFrameTime;*/

	//Calculate 'fWishspeed' & 'vWishdir', which are the desired speed & direction
	float fWishspeed;
	Vector3 vWishvel, vWishdir;

	vWishvel = vDir   * ( m_afThrust[THRUST_FORWARD] * ( 1.0f + m_fBoost ) - m_afThrust[THRUST_BACKWARD] ) * 10.0f
		     + vRight * ( m_afThrust[THRUST_RIGHT] - m_afThrust[THRUST_LEFT] ) * 10.0f
		     + vUp    * ( m_afThrust[THRUST_UP] - m_afThrust[THRUST_DOWN] ) * 10.0f;

//	vWishvel = ( vDir * fForward ) + ( vRight * fRight ) + ( vUp * fUp );
	fWishspeed = Vec3Length(vWishvel);
	Vec3Normalize(vWishdir, vWishvel);

	// In original QUAKE, this code was to clamp to server defined max speed.
	if(50 < fWishspeed)
	{
		fWishspeed = 50;
		//vWishvel *= g_PlayerMotionVariables.fMaxSpeed / fWishspeed;  //Is this necessary?
	}

/*	if( pCopyEnt->sState & CESTATE_ONGROUND )
	{	//player is on the ground
		pCopyEnt->vVelocity.y = 0.0f;
		Accelerate( pCopyEnt ,vWishdir, fWishspeed, 3.0f );
		pCopyEnt->vVelocity.y -= 9.8f * fFrameTime;
		//Move on the ground
		GroundMove( pCopyEnt );
	}
	else
	{	//player is off the ground
		AirAccelerate( pCopyEnt ,vWishdir, fWishspeed, 3.0f );
		//Add gravity
		pCopyEnt->vVelocity.y	-= 9.8f * fFrameTime;
		//Move through the air
		SlideMove( pCopyEnt );
	}*/


	// when the player is a ship 
	Accelerate( pCopyEnt ,vWishdir, fWishspeed, 5.0f );
	SlideMove( pCopyEnt );

	// update world muzzle pose
	Vector3 vMuzzlePosition;
//	Vector3 vLocalMuzzlePos = Vector3(0.0f, -0.35f, 1.45f);
	vMuzzlePosition	= pCopyEnt->GetWorldPosition()
					+ pCopyEnt->GetUpDirection() * m_vLocalMuzzlePos.y
				    + pCopyEnt->GetDirection() * m_vLocalMuzzlePos.z;

	m_WorldMuzzlePose.vPosition = vMuzzlePosition;
	m_WorldMuzzlePose.matOrient.SetColumn( 0, pCopyEnt->GetRightDirection() );
	m_WorldMuzzlePose.matOrient.SetColumn( 1, pCopyEnt->GetUpDirection() );
	m_WorldMuzzlePose.matOrient.SetColumn( 2, pCopyEnt->GetDirection() );

	// update world aabb here so that collision check would not be performed between the player and his own bullets
	// when he is stepping back while firing 
	pCopyEnt->world_aabb.TransformCoord( pCopyEnt->local_aabb, pCopyEnt->GetWorldPosition() );
}


bool CBE_PlayerShip::HandleInput( SPlayerEntityAction& input )
{

	if( CBE_Player::HandleInput( input ) )
		return true;	// input has been handled by the parent class 'CBE_Player'

	switch( input.ActionCode )
	{
	case ACTION_MOV_FORWARD:
		if( input.type == ITYPE_KEY_PRESSED )
		{
			SetThrust( THRUST_FORWARD, input.fParam );
			DWORD dwCurrentTime = timeGetTime();
			if( dwCurrentTime - m_adwLastActionInputTime[input.ActionCode] < 300 )
				SetBoost( 1.0f );
			m_adwLastActionInputTime[input.ActionCode] = dwCurrentTime;
			return true;
		}
		else if( input.type == ITYPE_KEY_RELEASED )
		{
			SetThrust( THRUST_FORWARD, 0 );
			SetBoost( 0.0f );
			return true;
		}
		break;

	case ACTION_MOV_BOOST:
		if( input.type == ITYPE_KEY_PRESSED )
			SetBoost( input.fParam );
		else if( input.type == ITYPE_KEY_RELEASED )
			SetBoost( 0 );
		return true;

	case ACTION_MOV_BACKWARD:
		if( input.type == ITYPE_KEY_PRESSED )
			SetThrust( THRUST_BACKWARD, input.fParam );
		else if( input.type == ITYPE_KEY_RELEASED )
			SetThrust( THRUST_BACKWARD, 0 );
		return true;

	case ACTION_MOV_SLIDE_R:
		if( input.type == ITYPE_KEY_PRESSED )
			SetThrust( THRUST_RIGHT, input.fParam );
		else if( input.type == ITYPE_KEY_RELEASED )
			SetThrust( THRUST_RIGHT, 0 );
		return true;

	case ACTION_MOV_SLIDE_L:
		if( input.type == ITYPE_KEY_PRESSED )
			SetThrust( THRUST_LEFT, input.fParam );
		else if( input.type == ITYPE_KEY_RELEASED )
			SetThrust( THRUST_LEFT, 0 );
		return true;

	case ACTION_MOV_SLIDE_UP:
		if( input.type == ITYPE_KEY_PRESSED )
			SetThrust( THRUST_UP, input.fParam );
		else if( input.type == ITYPE_KEY_RELEASED )
			SetThrust( THRUST_UP, 0 );
		return true;

	case ACTION_MOV_SLIDE_DOWN:
		if( input.type == ITYPE_KEY_PRESSED )
			SetThrust( THRUST_DOWN, input.fParam );
		else if( input.type == ITYPE_KEY_RELEASED )
			SetThrust( THRUST_DOWN, 0 );
		return true;

/*	case ACTION_MOV_ABS_YAW:
		SetRotationSpeedGain( PLAYERSHIP_ROTATION_YAW, input.fParam );
		break;

	case ACTION_MOV_ABS_PITCH:
		SetRotationSpeedGain( PLAYERSHIP_ROTATION_PITCH, input.fParam );
		break;*/

	case ACTION_MOV_YAW:
		AddRotationSpeedGain( PLAYERSHIP_ROTATION_YAW, input.fParam );
		return true;

	case ACTION_MOV_PITCH:
		AddRotationSpeedGain( PLAYERSHIP_ROTATION_PITCH, input.fParam );
		return true;
	}

	return false;
}

/*
void CBE_PlayerShip::ToggleHeadLight()
{
	if( m_bHeadLightOn == false )
	{	// turn on the light
		CCopyEntityDesc light;
		light.pBaseEntityHandle = &m_HeadLight;	// m_pStage->FindBaseEntity( "DynamicLightX" );
		light.vPosition = m_pPlayerCopyEntity->Position()
			            + m_pPlayerCopyEntity->GetDirection()
					    + m_pPlayerCopyEntity->GetUpDirection() * (-0.35f);

		m_pHeadLightEntity = m_pStage->CreateEntity( light );

		m_bHeadLightOn = true;
	}
	else
	{	// turn off the light
		GameMessage msg;
		msg.effect = GM_TERMINATE;
		SendGameMessageTo( msg, m_pHeadLightEntity );
		m_pHeadLightEntity = NULL;
		m_bHeadLightOn = false;
	}
}*/

/*
void CBE_PlayerShip::MessageProcedure(GameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
	float& rfLife = pCopyEnt_Self->fLife;
	float& rfTimeAfterDeath = pCopyEnt_Self->f4;
	GameMessage msg;
	int iVariation;

	switch( rGameMessage.effect )
	{
	case GM_DAMAGE:
		// play a damage sound according to the type of damage
		iVariation = PLYAER_DAMAGESOUND_VARIATION * rand() / RAND_MAX;
		if( PLYAER_DAMAGESOUND_VARIATION <= iVariation ) iVariation = PLYAER_DAMAGESOUND_VARIATION - 1;
		GetSoundManager().PlayAt( m_aiDamageSoundIndex[rGameMessage.s1][iVariation], pCopyEnt_Self->Position() );

		// flash screen with red
		m_pStage->GetScreenEffectManager()->FadeInFrom( 0x40F82000, 0.20f, D3DBLEND_INVSRCALPHA );

///		rfLife -= rGameMessage.fParam1;
		if( rfLife <= 0 && rfTimeAfterDeath < 0 )
		{
			rfLife = 0;
			rfTimeAfterDeath = 0;
			pCopyEnt_Self->vVelocity = Vector3(0,0,0);

			m_pStage->GetScreenEffectManager()->FadeOutTo( 0xFFFFFFFF, 2.5f, D3DBLEND_ONE );
		}

		if( rGameMessage.s1 == DMG_BLAST )
		{
//			Vector3 vImpulse;
//			vImpulse.x = RangedRand(  -1.0f,  1.0f );
//			vImpulse.y = RangedRand( -500.0f, -400.0f );
//			vImpulse.z = 0;
//			m_pShockWaveEffect->AddForce( vImpulse );
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
		SendGameMessageTo( msg, rGameMessage.pSenderEntity );
		break;

	case GM_AMMOSUPPLY:
		if( WEAPONSYSTEM.SupplyAmmunition( rGameMessage.pcStrParam, rGameMessage.fParam1 ) )
		{	// the item was accepted by the player
			msg.effect = GM_EFFECTACCEPTED;
			msg.sender = pCopyEnt_Self->Self();
			SendGameMessageTo( msg, rGameMessage.pSenderEntity );
		}
		break;

	case GM_DOORKEYITEM:
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
		break;
	}

}*/

/*
void CBE_PlayerShip::PlayerDead(CCopyEntity* pCopyEnt)
{
	float& rfTimeAfterDeath = pCopyEnt->f4;
	rfTimeAfterDeath += m_pStage->GetFrameTime();

	if( m_pStage->GetFrameTime() == 0 )
		MessageBox(NULL, "frame time 0", "exception", MB_OK);

	if( 2.5f <= rfTimeAfterDeath )
	{
		rfTimeAfterDeath = -1;
		pCopyEnt->fLife = 100.0f;
		pCopyEnt->GetWorldPosition() = Vector3(0,0,0);
		pCopyEnt->SetDirection( Vector3(0,0,1) );
		pCopyEnt->SetDirection_Right( Vector3(1,0,0) );
		pCopyEnt->SetDirection_Up( Vector3(0,1,0) );
		pCopyEnt->s1 = 0;

		m_pStage->GetScreenEffectManager()->FadeInFrom( 0xFF000000, 1.0f, D3DBLEND_INVSRCALPHA );

		return;
	}

	Vector3 vDir, vRight;
	D3DXMATRIX matRotY;
	float fRotAngle = m_pStage->GetFrameTime() * (2.5f - rfTimeAfterDeath) * (2.5f - rfTimeAfterDeath);
	D3DXMatrixRotationY( &matRotY, fRotAngle * 2.0f );
	D3DXVec3TransformCoord( &vDir, &pCopyEnt->GetDirection(), &matRotY);
	D3DXVec3TransformCoord( &vRight, &pCopyEnt->GetRightDirection(), &matRotY);
	pCopyEnt->SetDirection( vDir );
	pCopyEnt->SetDirection_Right( vRight );
}*/

/*
void CBE_PlayerShip::Draw(CCopyEntity *pCopyEnt)
{
//	if( !m_bTriggerSafetyEnabled )
//		CastLaserDotX( pCopyEnt );
}
*/


void CBE_PlayerShip::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_Player::Serialize( ar, version );
}


InputHandler_PlayerBase *CBE_PlayerShip::CreatePlayerInputHandler()
{
	return new InputHandler_PlayerShip();
}


} // namespace amorphous
