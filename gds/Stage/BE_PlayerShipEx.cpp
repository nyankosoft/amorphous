#include "BE_PlayerShip.h"
#include "EntitySet.h"
#include "Stage.h"
//#include "../Game_Common/WeaponSystem.h"
//#include "../Stage/ScreenEffectManager.h"
//#include "GameMessage.h"

//#include "../Stage/BE_LaserDot.h"

#include "../3DCommon/Direct3D9.h"
//#include "../3DCommon/D3DXMeshObject.h"
//#include "../Support/memory_helpers.h"
#include "trace.h"

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

	D3DXMATRIX matRot;
	D3DXVECTOR3 vDir   = pEntity->GetDirection();
	D3DXVECTOR3 vUp    = pEntity->GetUpDirection();
	D3DXVECTOR3 vRight = pEntity->GetRightDirection();

	D3DXMatrixRotationAxis(&matRot, &vUp, fYaw);
	D3DXVec3TransformCoord(&vDir,   &vDir,   &matRot);
	D3DXVec3TransformCoord(&vRight, &vRight, &matRot);

	D3DXMatrixRotationAxis(&matRot, &vRight, fPitch);
	D3DXVec3TransformCoord(&vDir, &vDir, &matRot);
	D3DXVec3TransformCoord(&vUp,  &vUp,  &matRot);

	pEntity->SetDirection_Right( vRight );
	pEntity->SetDirection_Up( vUp );
	pEntity->SetDirection( vDir );

	// translation
	D3DXVECTOR3 vWishvel, vWishdir;
	float fWishspeed;

	vWishvel = pEntity->GetDirection() * ( m_afThrust[THRUST_FORWARD] * ( 1.0f + m_fBoost ) - m_afThrust[THRUST_BACKWARD] ) * 10.0f
		     + pEntity->GetRightDirection()     * ( m_afThrust[THRUST_RIGHT] - m_afThrust[THRUST_LEFT] ) * 10.0f
		     + pEntity->GetUpDirection()        * ( m_afThrust[THRUST_UP] - m_afThrust[THRUST_DOWN] ) * 10.0f;

	fWishspeed = D3DXVec3Length(&vWishvel);
	D3DXVec3Normalize(&vWishdir, &vWishvel);


	if(50 < fWishspeed)
	{
		fWishspeed = 50;
	}

	Accelerate( pEntity ,vWishdir, fWishspeed, 5.0f );

	// update world aabb here so that collision check would not be performed between the player and his own bullets
	// when he is stepping back while firing 
	pEntity->world_aabb.TransformCoord( pEntity->local_aabb, pEntity->Position() );

}
