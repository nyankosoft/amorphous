#include "BE_PlayerPseudoLeggedVehicle.hpp"
#include "PlayerInfo.hpp"
#include "EntitySet.hpp"
#include "CopyEntityDesc.hpp"
#include "Stage.hpp"
#include "Input/InputHandler_PlayerShip.hpp"
#include "Item/WeaponSystem.hpp"
#include "Stage/ScreenEffectManager.hpp"
#include "GameMessage.hpp"

#include "GameCommon/GameMathMisc.hpp"
#include "Support/MTRand.hpp"
#include "GameCommon/RandomDirectionTable.hpp"
#include "GameCommon/ShockWaveCameraEffect.hpp"

#include "Stage/bsptree.hpp"
#include "Stage/BE_LaserDot.hpp"

// added for laser dot casting test
#include "Graphics/3DGameMath.hpp"
#include "Graphics/Direct3D9.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/Log/StateLog.hpp"
#include "Support/StringAux.hpp"
#include "trace.hpp"

//#include "JigLib/JL_PhysicsActor.hpp"
#include "JigLib/JL_LineSegment.hpp"

#include "Physics/Actor.hpp"


using namespace physics;


//================================================================================
// CVehicleLeg
//================================================================================

CVehicleLeg::CVehicleLeg( Vector3 vLocalPos, Vector3 vLocalDir, float fLength )
: m_vLocalPos(vLocalPos), m_vLocalDir(vLocalDir), m_fLength(fLength), m_pStage(NULL)
{
}


CBE_PlayerPseudoLeggedVehicle::~CBE_PlayerPseudoLeggedVehicle()
{
}


void CVehicleLeg::Update( CCopyEntity *pEntity, float dt )
{
//	if( !pEntity->pPhysicsActor )
//		return;

	if( pEntity->m_vecpPhysicsActor.size() == 0
	 || pEntity->m_vecpPhysicsActor[0] == NULL )
		return;

	physics::CActor *pPhysicsActor = pEntity->m_vecpPhysicsActor[0];

//	const Matrix34& rWorldPose = pEntity->GetWorldPose();

	Matrix34 world_pose;
	pPhysicsActor->GetWorldPose( world_pose );

	Vector3 vWorldPos, vWorldDir;

	world_pose.Transform( vWorldPos, m_vLocalPos );
	vWorldDir = world_pose.matOrient * m_vLocalDir;

	CJL_LineSegment segment;
	segment.SetDefault();
	segment.vStart = vWorldPos;

	Vector3 vGoal = vWorldPos + vWorldDir * m_fLength;
	segment.vGoal = vGoal;

	segment.vEnd = vGoal;
	segment.fFraction = 1.0f;

	CStage *pStage = m_pStage;

//	pStage->ClipTrace( segment );

	if( segment.fFraction < 1.0f && 0.1f < -Vec3Dot( segment.plane.normal, vWorldDir ) )
	{
		m_OnGround = true;

//		float depth = Vec3Length( vGoal - segment.vEnd );
		float depth = - Vec3Dot( vGoal - segment.vEnd, segment.plane.normal );

//		assert( 0 <= depth );
//		assert( 0 <= segment.fFraction );

		if( 0 < depth )
		{
//			Vector3 vVelLegDir = Vec3Dot( vWorldDir, pEntity->Velocity() ) * vWorldDir;
			Vector3 vVelLegDir = Vec3Dot( vWorldDir, pPhysicsActor->GetLinearVelocity() ) * vWorldDir;

			// reduce force when the leg is perpendicular to the contacted surface
			float slope = fabsf( Vec3Dot( vWorldDir, segment.plane.normal ) );

			// apply spring
			Vector3 vForce;
			vForce = - vWorldDir * depth * m_fSpring * slope;

			// apply damping
			vForce -= vVelLegDir * m_fDamper;

			pPhysicsActor->AddWorldForceAtWorldPos( vForce, vWorldPos );
//			pPhysicsActor->ApplyWorldImpulse( vForce, vWorldPos );
		}
	}
	else
		m_OnGround = false;
}



//================================================================================
// CBE_PlayerPseudoLeggedVehicle
//================================================================================

CBE_PlayerPseudoLeggedVehicle::CBE_PlayerPseudoLeggedVehicle()
{
//	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_PLAYER;

//	m_ActorDesc.ActorFlag |= JL_ACTOR_APPLY_NO_ANGULARIMPULSE;
	m_EntityFlag |= BETYPE_RIGIDBODY;

//	m_fStaticFriction = 0.8f;
//	m_fDynamicFriction = 0.8f;

}

void CBE_PlayerPseudoLeggedVehicle::Init()
{
	CBE_Player::Init();

	m_EntityFlag |= BETYPE_PLAYER | BETYPE_FLOATING;

	for( int i=0; i<6; i++ )
		m_afThrust[i] = 0.0f;

	m_fBoost = 0.0f;

	m_vJumpThrust = Vector3(0,0,0);

//	float fLegTopPos = -0.2f;
	float fLegTopPos = 0.1f;

	// front left
	m_aVehicleLeg[0].m_vLocalPos = Vector3(-1.2f, fLegTopPos,  1.4f);

	// front right
	m_aVehicleLeg[1].m_vLocalPos = Vector3( 1.2f, fLegTopPos,  1.4f);

	// rear left
	m_aVehicleLeg[2].m_vLocalPos = Vector3(-1.2f, fLegTopPos, -1.4f);

	// rear right
	m_aVehicleLeg[3].m_vLocalPos = Vector3( 1.2f, fLegTopPos, -1.4f);

	for( int i=0; i<4; i++ )
	{
		m_aVehicleLeg[i].m_vLocalDir = Vector3(0,-1,0);
//		m_aVehicleLeg[i].m_fLength   = 1.0f;
		m_aVehicleLeg[i].m_fLength   = 1.3f;

///		m_aVehicleLeg[i].m_fSpring   = 1.0f;
		m_aVehicleLeg[i].m_fSpring   = 150.0f;

		m_aVehicleLeg[i].m_fDamper   = 20.0f;
	}

	// set stage to legs
	for( int i=0; i<4; i++ )
		m_aVehicleLeg[i].SetStage( m_pStage );

	m_vLocalMuzzlePos = Vector3(0.0f, -0.35f, 1.65f);

}


#define MAX_PITCH	60
#define MAX_BANK	60

void CBE_PlayerPseudoLeggedVehicle::LimitOrientation( CCopyEntity *pCopyEnt )
{
//	CActor& rPhysicsActor = *pCopyEnt->pPhysicsActor;
	CActor& rPhysicsActor = *(pCopyEnt->m_vecpPhysicsActor[0]);

	float dp = Vec3Dot( pCopyEnt->GetUpDirection(), Vector3(0,1,0) );
	float max_pitch_cos = (float)cos(rad_to_deg(MAX_PITCH));

	if( 0.98f < dp )
	{
		// not need to correct orientation
		return;
	}
	else
	{
		// apply torque to modify the orientation
		Vector3 vAxis;
		Vec3Cross( vAxis, pCopyEnt->GetUpDirection(), Vector3(0,1,0) );
		Vec3Normalize( vAxis, vAxis );
		vAxis *= ( 1.0f - dp ) * 15.0f;
		rPhysicsActor.AddWorldTorque( vAxis );

		if( dp < 0.85f )
		{
			// if the angle between y-axis and current up direction is
			// over 60 degrees, modify the orientation directly
			Matrix33 matRot, matOrigOrient;
			matRot = Matrix33RotationAxis( (float)acos(dp), vAxis );

			matOrigOrient = rPhysicsActor.GetWorldOrientation();

//			rPhysicsActor.SetOrientation( matRot * matOrigOrient );
		}
	}

}


void CBE_PlayerPseudoLeggedVehicle::Move( CCopyEntity *pCopyEnt )
{
//	if( !pCopyEnt->pPhysicsActor )
	if( pCopyEnt->m_vecpPhysicsActor.size() == 0
	 || pCopyEnt->m_vecpPhysicsActor[0] == NULL )
	{
//		assert( pCopyEnt->pPhysicsActor );
		ONCE( LOG_PRINT_ERROR( "No physics actor" ) );
		return;
	}

//	CActor& rPhysicsActor = *pCopyEnt->pPhysicsActor;
	CActor& rPhysicsActor = *(pCopyEnt->m_vecpPhysicsActor[0]);

	// If player is in solid, try to get out of there.
//	NudgePosition( pCopyEnt );

	float fFrameTime = m_pStage->GetFrameTime();
	if( 0.03125f < fFrameTime )
		fFrameTime = 0.03125f;	// don't use frametime directly when if it's too large ( 32 < FPS )

	// there used to be some jump-related code here

	Matrix33 matCameraOrient;
	matCameraOrient.SetColumn( 0, m_Camera.GetRightDirection() );
	matCameraOrient.SetColumn( 1, m_Camera.GetUpDirection() );
	matCameraOrient.SetColumn( 2, m_Camera.GetFrontDirection() );

	// update world muzzle pose
	m_WorldMuzzlePose.vPosition	= pCopyEnt->GetWorldPosition()
		                        + matCameraOrient.GetColumn(1) * m_vLocalMuzzlePos.y
		                        + matCameraOrient.GetColumn(2) * m_vLocalMuzzlePos.z;

	m_WorldMuzzlePose.matOrient = matCameraOrient;

	// update world aabb here so that collision check would not be performed between the player and his own bullets
	// when he is stepping back while firing
	pCopyEnt->world_aabb.TransformCoord( pCopyEnt->local_aabb, pCopyEnt->GetWorldPosition() );
}


void CBE_PlayerPseudoLeggedVehicle::UpdateCamera(CCopyEntity* pCopyEnt)
{
	Vector3 vNewCameraPos;
    UpdateCameraEffect( vNewCameraPos, pCopyEnt );

	// update camera position
	// orientation is set in Move()

	m_Camera.SetPosition( vNewCameraPos );

	m_Camera.UpdateVFTreeForWorldSpace();
}


void CBE_PlayerPseudoLeggedVehicle::UpdatePhysics( CCopyEntity *pCopyEnt, float dt )
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
	CActor& rPhysicsActor = *pCopyEnt->m_vecpPhysicsActor[0];

	// wake up the actor if it's sleeping
//	if( rPhysicsActor.GetActivityState() == CJL_PhysicsActor::FROZEN )
//		rPhysicsActor.ApplyWorldImpulse( pCopyEnt->GetUpDirection() * 0.0001f );

	if( rPhysicsActor.IsSleeping() )
		rPhysicsActor.WakeUp();

	int i, num_legs = NUM_VEHICLE_LEGS;
	for( i=0; i<num_legs; i++ )
	{
		m_aVehicleLeg[i].Update( pCopyEnt, dt );
	}


	Vector3 vRight, vUp, vDir;

	vRight	= pCopyEnt->GetRightDirection();
	vUp		= pCopyEnt->GetUpDirection();
	vDir	= pCopyEnt->GetDirection();

	if( 1.0f - fabsf(vDir.y) < 0.005f )
	{
		// looking vertically upward
		// cannot determine the horizontal direction
		return;
	}
	else
	{
		vDir.y = 0;
		Vec3Normalize( vDir, vDir );
		vUp = Vector3(0,1,0);
		vRight = Vec3Cross( vUp, vDir );
	}


	float fYawSpeedGain;
//	float fAbsYawSpeed, fAbsPitchSpeed, fSign;
	static float fYawSpeed   = 0;
	static float fPitchSpeed = 0;
	static float fRollSpeed = 0;
	static float fYaw = 0.0f;	// turn right / left	(unit: radian)
	static float fPitch = 0.0f;	// look up / down		(unit: radian)
	static float fRoll = 0.0f;	// bank right /left		(unit: radian)


//	fYawSpeedGain = m_afRotationSpeedGain[PLAYERPLV_ROTATION_YAW] / 40.0f;
	fYawSpeedGain = m_afRotationSpeedGain[PLAYERPLV_ROTATION_YAW] / 6.8f;
	m_afRotationSpeedGain[PLAYERPLV_ROTATION_YAW] = 0.0f;

	fYawSpeed += fYawSpeedGain;
	ApplySQFriction( fYawSpeed, dt, fabsf(fYawSpeed) * 2.45f, 3.125f );

	fYaw += fYawSpeed * dt; 


	if( 2.0f * D3DX_PI < fYaw )			fYaw -= 2.0f * D3DX_PI;
	else if( fYaw < -2.0f * D3DX_PI )	fYaw += 2.0f * D3DX_PI;

	// change the direction of the vehicle
	rPhysicsActor.AddWorldTorque( pCopyEnt->GetUpDirection() * fYawSpeed * 2.0f );

	// measure roll - tilt a little when player turn right /left
/*	fRollSpeed -= fYawSpeedGain / 4.0f;
	float fAbsRollSpeed = fabsf(fRollSpeed);
	fSign = 0 < fRollSpeed ? 1 : -1;
	ApplyFriction( fAbsRollSpeed, fAbsRollSpeed * 2.45f );
	fRollSpeed = fAbsRollSpeed * fSign;
	
	fRoll += fRollSpeed * fFrameTime;

	float fAbsRoll = fabsf(fRoll);
	fSign = 0 < fRoll ? 1 : -1;
	if( fAbsYawSpeed < D3DX_PI / 3.0f )
	{	// back to the horizontal position
		ApplyFriction( fAbsRoll, fAbsRoll * 2.2f );
		fRoll = fSign * fAbsRoll;
	}

	float fMaxRollAngle = D3DX_PI / 9.0f;
	if( fMaxRollAngle < fRoll )
		fRoll = fMaxRollAngle;
	else if( fRoll < -fMaxRollAngle )
		fRoll = -fMaxRollAngle;

*/

	// save the current camera pose
	m_Camera.GetPose( m_PrevCameraPose );


	fPitchSpeed = fPitchSpeed - m_afRotationSpeedGain[PLAYERPLV_ROTATION_PITCH] / 20.0f;
	m_afRotationSpeedGain[PLAYERPLV_ROTATION_PITCH] = 0.0f;

	ApplySQFriction( fPitchSpeed, dt, fabsf(fPitchSpeed) * 2.45f, 3.125f );

	fPitch += fPitchSpeed * dt; 


	if( D3DX_PI / 2.1f < fPitch )	 fPitch = D3DX_PI / 2.1f;
	if( fPitch < -(D3DX_PI / 2.1f) ) fPitch = -( D3DX_PI / 2.1f );


	// apply angular friction
	Vector3 vCounterAngForce = - rPhysicsActor.GetAngularVelocity() * 5.0f;
	rPhysicsActor.AddWorldTorque( vCounterAngForce );

	// update pitch
	Matrix33 matRot;
	Vector3 vCamRight, vCamUp, vCamDir;
	matRot	= Matrix33RotationAxis( fPitch, vRight );
//	vCamRight = vRight;
//	vCamUp	= matRot * vUp;
//	vCamDir	= matRot * vDir;
	vCamRight = pCopyEnt->GetRightDirection();
	vCamUp	  = matRot * pCopyEnt->GetUpDirection();
	vCamDir	  = matRot * pCopyEnt->GetDirection();

	// update camera orientation
	// position is updated in UpdateCamera()
	Matrix33 matCameraOrient;
	matCameraOrient.SetColumn( 0, vCamRight );
	matCameraOrient.SetColumn( 1, vCamUp );
	matCameraOrient.SetColumn( 2, vCamDir );

	m_Camera.SetOrientation( matCameraOrient );

	// calculate 'fWishspeed' & 'vWishdir', which are the desired speed & direction
	float fWishspeed;
	Vector3 vWishvel, vWishdir;

	vWishvel = vDir   * ( m_afThrust[THRUST_FORWARD] * ( 1.0f + m_fBoost ) - m_afThrust[THRUST_BACKWARD] ) * 30.0f
		     + vRight * ( m_afThrust[THRUST_RIGHT] - m_afThrust[THRUST_LEFT] ) * 30.0f
		     + vUp    * ( m_afThrust[THRUST_UP] - m_afThrust[THRUST_DOWN] ) * 30.0f;

	fWishspeed = Vec3Length(vWishvel);
	Vec3Normalize( vWishdir, vWishvel );

	if(80 < fWishspeed)
	{
		fWishspeed = 80;
		//vWishvel *= g_PlayerMotionVariables.fMaxSpeed / fWishspeed;  //Is this necessary?
	}

///	Accelerate( pCopyEnt, vWishdir, fWishspeed, 10.0f );

//	rPhysicsActor.SetVelocity( pCopyEnt->Velocity() );
//	rPhysicsActor.SetVelocity( vWishdir );
	rPhysicsActor.AddWorldForce( vWishdir * 800.0f );

	Vector3 vCounterForce;
	vCounterForce = - rPhysicsActor.GetLinearVelocity() * 30.0f;
	rPhysicsActor.AddWorldForce( vCounterForce );

//	pCopyEnt->pPhysicsActor->ApplyWorldImpulse( m_vJumpThrust );
	if( m_fJumpThrustTime < 0.2f )
	{
//		rPhysicsActor.AddWorldForce( m_vJumpThrust * 10.0f );
		rPhysicsActor.AddWorldForce( m_vJumpThrust * 3.0f );
		m_fJumpThrustTime += dt;
	}
	else
        m_vJumpThrust = Vector3(0,0,0);

	// make sure that the vehicle does not turn upside down
	LimitOrientation( pCopyEnt );
}


bool CBE_PlayerPseudoLeggedVehicle::HandleInput( SPlayerEntityAction& input )
{

	if( CBE_Player::HandleInput( input ) )
		return true;	// input has been handled by the parent class 'CBE_Player'

	switch( input.ActionCode )
	{
	case ACTION_MOV_FORWARD:
//		StateLog.Update( 15, "fwd: " + to_string(input.type) );
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
		{
			SetThrust( THRUST_FORWARD, input.fParam );
			DWORD dwCurrentTime = timeGetTime();
			if( dwCurrentTime - m_adwLastActionInputTime[input.ActionCode] < 300 )
				SetBoost( 1.0f );
			m_adwLastActionInputTime[input.ActionCode] = dwCurrentTime;
			return true;
		}
		else if( input.type == SPlayerEntityAction::KEY_RELEASED )
		{
			SetThrust( THRUST_FORWARD, 0 );
			SetBoost( 0.0f );
			return true;
		}
		break;

	case ACTION_MOV_BOOST:
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
			SetBoost( input.fParam );
		else if( input.type == SPlayerEntityAction::KEY_RELEASED )
			SetBoost( 0 );
		return true;

	case ACTION_MOV_BACKWARD:
//		StateLog.Update( 16, "bk: " + to_string(input.type) );
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
			SetThrust( THRUST_BACKWARD, input.fParam );
		else if( input.type == SPlayerEntityAction::KEY_RELEASED )
			SetThrust( THRUST_BACKWARD, 0 );
		return true;

	case ACTION_MOV_SLIDE_R:
//		StateLog.Update( 17, "right: " + to_string(input.type) );
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
			SetThrust( THRUST_RIGHT, input.fParam );
		else if( input.type == SPlayerEntityAction::KEY_RELEASED )
			SetThrust( THRUST_RIGHT, 0 );
		return true;

	case ACTION_MOV_SLIDE_L:
//		StateLog.Update( 18, "left: " + to_string(input.type) );
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
			SetThrust( THRUST_LEFT, input.fParam );
		else if( input.type == SPlayerEntityAction::KEY_RELEASED )
			SetThrust( THRUST_LEFT, 0 );
		return true;

	case ACTION_MOV_SLIDE_UP:
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
//			AddJumpThrust( Vector3(0,10,0) );
			AddJumpThrust( Vector3(0,1000,0) );
/*		if( input.type == SPlayerEntityAction::KEY_PRESSED )
			SetThrust( THRUST_UP, input.fParam );
		else if( input.type == SPlayerEntityAction::KEY_RELEASED )
			SetThrust( THRUST_UP, 0 );*/
		return true;

	case ACTION_MOV_SLIDE_DOWN:
		if( input.type == SPlayerEntityAction::KEY_PRESSED )
			SetThrust( THRUST_DOWN, input.fParam );
		else if( input.type == SPlayerEntityAction::KEY_RELEASED )
			SetThrust( THRUST_DOWN, 0 );
		return true;

/*	case ACTION_MOV_ABS_YAW:
		SetRotationSpeedGain( PLAYERPLV_ROTATION_YAW, input.fParam );
		break;
	case ACTION_MOV_ABS_PITCH:
		SetRotationSpeedGain( PLAYERPLV_ROTATION_PITCH, input.fParam );
		break;*/

	case ACTION_MOV_YAW:
		AddRotationSpeedGain( PLAYERPLV_ROTATION_YAW, input.fParam );
		return true;

	case ACTION_MOV_PITCH:
		AddRotationSpeedGain( PLAYERPLV_ROTATION_PITCH, input.fParam );
		return true;
	}

	return false;
}

void CBE_PlayerPseudoLeggedVehicle::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_Player::Serialize( ar, version );

//	ar & m_aVehicleLeg
}


CInputHandler_PlayerBase *CBE_PlayerPseudoLeggedVehicle::CreatePlayerInputHandler()
{
	return new CInputHandler_PlayerShip();
}
