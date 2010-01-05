#include "CameraControllerBase.hpp"
#include <d3dx9.h>


CCameraControllerBase::CCameraControllerBase()
{
//s	m_Pose = Matrix34Identity();

	m_fYaw = 0.0f;
	m_fPitch = 0.0f;

	m_fTranslationSpeed = 8.0f;

	m_iPrevMousePosX = 0;
	m_iPrevMousePosY = 0;

	// set up standard keybinds
	m_CameraControlCode[CameraControl::Forward]  = 'E';
	m_CameraControlCode[CameraControl::Backward] = 'D';
	m_CameraControlCode[CameraControl::Right]    = 'F';
	m_CameraControlCode[CameraControl::Left]     = 'S';
	m_CameraControlCode[CameraControl::Up]       = 'Q';
	m_CameraControlCode[CameraControl::Down]     = 'A';
}


CCameraControllerBase::~CCameraControllerBase()
{
}


void CCameraControllerBase::UpdateCameraPose( float dt )
{
	Vector3 vRight = Vector3(1, 0, 0);
	Vector3 vUp =	 Vector3(0, 1, 0);
	Vector3 vDir =	 Vector3(0, 0, 1);

	float forward=0, right=0, up=0, spd;
	D3DXMATRIX matRot;

	if( (GetAsyncKeyState(VK_SHIFT) & 0x8000) )
		spd = m_fTranslationSpeed * 2.0f;
	else
		spd = m_fTranslationSpeed * 1.0f;

	int *kb = m_CameraControlCode;

	if( IsKeyPressed(kb[CameraControl::Forward]) )     forward =  spd * dt;
	if( IsKeyPressed(kb[CameraControl::Backward]) )    forward = -spd * dt;
	if( IsKeyPressed(kb[CameraControl::Right]) )       right   =  spd * dt;
	if( IsKeyPressed(kb[CameraControl::Left]) )        right   = -spd * dt;
	if( IsKeyPressed(kb[CameraControl::Up]) )          up      =  spd * dt * 3.0f;
	if( IsKeyPressed(kb[CameraControl::Down]) )        up      = -spd * dt * 3.0f;

/*
	if( (GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState('E') & 0x8000) )	forward =  spd * dt;
	if( (GetAsyncKeyState(VK_DOWN) & 0x8000) || (GetAsyncKeyState('D') & 0x8000) )	forward = -spd * dt;
	if( GetAsyncKeyState(VK_RIGHT) & 0x8000 || (GetAsyncKeyState('F') & 0x8000)  )	right   =  spd * dt;
	if( GetAsyncKeyState(VK_LEFT) & 0x8000 || (GetAsyncKeyState('S') & 0x8000)  )	right   = -spd * dt;
//	if( GetAsyncKeyState('X') & 0x8000 )	m_fYaw += 3.141592f * dt;
//	if( GetAsyncKeyState('Z') & 0x8000	)	m_fYaw -= 3.141592f * dt;
//	if( GetAsyncKeyState('Q') & 0x8000 )	m_fPitch += 3.141592f * dt;
//	if( GetAsyncKeyState('A') & 0x8000	)	m_fPitch -= 3.141592f * dt;
//	if( GetAsyncKeyState('Q') & 0x8000 )	m_vCameraPosition.y += 2.0f * dt;
//	if( GetAsyncKeyState('A') & 0x8000	)	m_vCameraPosition.y -= 2.0f * dt;
	if( GetAsyncKeyState('Q') & 0x8000 )	up =  5.0f * spd * dt;
	if( GetAsyncKeyState('A') & 0x8000 )	up = -5.0f * spd * dt;
*/

//	m_Pose.matOrient = Matrix33RotationY(m_fYaw) * Matrix33RotationY(m_fPitch);

	D3DXMatrixRotationYawPitchRoll( &matRot, m_fYaw, m_fPitch, 0 );
	D3DXVec3TransformCoord( &vRight, &vRight, &matRot);
	D3DXVec3TransformCoord( &vUp,    &vUp,    &matRot);
	D3DXVec3TransformCoord( &vDir,   &vDir,   &matRot);

	Matrix34 pose = GetPose();

	pose.matOrient.SetColumn( 0, vRight );
	pose.matOrient.SetColumn( 1, vUp );
	pose.matOrient.SetColumn( 2, vDir );

	pose.vPosition
		+= pose.matOrient.GetColumn(2) * forward
		+  pose.matOrient.GetColumn(0) * right
		+  pose.matOrient.GetColumn(1) * up;

	SetPose( pose );

//	m_Pose.vPosition += m_vRight * m_fMouseMoveRight + m_vUp * m_fMouseMoveUp;
//	m_fMouseMoveRight = 0; m_fMouseMoveUp = 0;

}


void CCameraControllerBase::AddYaw( float fYaw )
{
	m_fYaw += fYaw;
}


void CCameraControllerBase::AddPitch( float fPitch )
{
	m_fPitch += fPitch;
}


void CCameraControllerBase::AssignKeyForCameraControl( int general_input_code, CameraControl::Operation cam_control_op )
{
	if( NUM_GENERAL_INPUT_CODES <= general_input_code )
		return;

	m_CameraControlCode[cam_control_op] = general_input_code;
}
