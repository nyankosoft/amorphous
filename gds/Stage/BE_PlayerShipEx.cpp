#include "BE_PlayerShip.hpp"
#include "EntitySet.hpp"
#include "Stage.hpp"
#include "trace.hpp"


namespace amorphous
{


using namespace std;


void CBE_PlayerShip::MoveEx(CCopyEntity *pEntity)
{
	float fFrameTime = m_pStage->GetFrameTime();

	ApplyFriction( pEntity, 3.2f );

	// rotation
	static float s_fYawSpeed   = 0;
	static float s_fPitchSpeed = 0;
	static float s_fRollSpeed = 0;

	pEntity->AngularVelocity() = Vector3(0,0,0);

	float fYaw, fYawSpeedGain, fAbsYawSpeed, fPitch, fAbsPitchSpeed, fSign;
	fYawSpeedGain = m_afRotationSpeedGain[PLAYERSHIP_ROTATION_YAW] / 40.0f;

//	pEntity->AngularVelocity() = pEntity->Up() * fYawSpeedGain;
//	pEntity->vTorque += pEntity->Up() * fYawSpeedGain * 10.0f;

	s_fYawSpeed += fYawSpeedGain;
	fAbsYawSpeed = fabsf(s_fYawSpeed);
	fSign = 0 < s_fYawSpeed ? 1 : -1;
	ApplyFriction( fAbsYawSpeed, fAbsYawSpeed * 2.45f );
	s_fYawSpeed = fAbsYawSpeed * fSign;

	fYaw = s_fYawSpeed * fFrameTime;


	s_fPitchSpeed = s_fPitchSpeed - m_afRotationSpeedGain[PLAYERSHIP_ROTATION_PITCH] / 40.0f;

	fAbsPitchSpeed = fabsf(s_fPitchSpeed);
	fSign = 0 < s_fPitchSpeed ? 1 : -1;
	ApplyFriction( fAbsPitchSpeed, fAbsPitchSpeed * 2.45f );
	s_fPitchSpeed = fAbsPitchSpeed * fSign;

	fPitch = s_fPitchSpeed * fFrameTime; 

	Matrix33 matRot( Matrix33Identity() );
	Vector3 vDir   = pEntity->GetDirection();
	Vector3 vUp    = pEntity->GetUpDirection();
	Vector3 vRight = pEntity->GetRightDirection();

	matRot = Matrix33RotationAxis( fYaw, vUp );
	vDir   = matRot * vDir;
	vRight = matRot * vRight;

	matRot = Matrix33RotationAxis( fPitch, vRight );
	vDir = matRot * vDir;
	vUp  = matRot * vUp;

	pEntity->SetDirection_Right( vRight );
	pEntity->SetDirection_Up( vUp );
	pEntity->SetDirection( vDir );

	// translation
	Vector3 vWishvel, vWishdir;
	float fWishspeed;

	vWishvel = pEntity->GetDirection() * ( m_afThrust[THRUST_FORWARD] * ( 1.0f + m_fBoost ) - m_afThrust[THRUST_BACKWARD] ) * 10.0f
		     + pEntity->GetRightDirection()     * ( m_afThrust[THRUST_RIGHT] - m_afThrust[THRUST_LEFT] ) * 10.0f
		     + pEntity->GetUpDirection()        * ( m_afThrust[THRUST_UP] - m_afThrust[THRUST_DOWN] ) * 10.0f;

	fWishspeed = Vec3Length(vWishvel);
	Vec3Normalize(vWishdir, vWishvel);


	if(50 < fWishspeed)
	{
		fWishspeed = 50;
	}

	Accelerate( pEntity ,vWishdir, fWishspeed, 5.0f );

	// update world aabb here so that collision check would not be performed between the player and his own bullets
	// when he is stepping back while firing 
	pEntity->world_aabb.TransformCoord( pEntity->local_aabb, pEntity->GetWorldPosition() );

}


} // namespace amorphous
