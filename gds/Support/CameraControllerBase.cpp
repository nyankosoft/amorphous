#include "CameraControllerBase.hpp"
#include "Input/InputHandler.hpp"


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

	memset( m_IsMouseButtonPressed, 0, sizeof(m_IsMouseButtonPressed) );
}


CCameraControllerBase::~CCameraControllerBase()
{
}


void CCameraControllerBase::UpdateCameraPose( float dt )
{
	float forward=0, right=0, up=0, spd;
//	Matrix33 matRot;

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
	Matrix34 pose = GetPose();

	pose.matOrient = Matrix33RotationY(m_fYaw) * Matrix33RotationX(m_fPitch);

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


void CCameraControllerBase::HandleInput( const SInputData& input )
{
	int iMousePosX = 0, iMousePosY = 0, iMouseMoveX = 0, iMouseMoveY = 0;

	switch( input.iGICode )
    {
	case GIC_MOUSE_BUTTON_L:
	case GIC_MOUSE_BUTTON_R:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_iPrevMousePosX = input.GetParamH16();
			m_iPrevMousePosY = input.GetParamL16();
			switch( input.iGICode )
			{
			case GIC_MOUSE_BUTTON_L: m_IsMouseButtonPressed[MBTN_LEFT]  = 1; break;
			case GIC_MOUSE_BUTTON_R: m_IsMouseButtonPressed[MBTN_RIGHT] = 1; break;
			default: break;
			}
		}
		else if( input.iType == ITYPE_KEY_RELEASED )
		{
			switch( input.iGICode )
			{
			case GIC_MOUSE_BUTTON_L: m_IsMouseButtonPressed[MBTN_LEFT]  = 0; break;
			case GIC_MOUSE_BUTTON_R: m_IsMouseButtonPressed[MBTN_RIGHT] = 0; break;
			default: break;
			}
		}
		break;
	case GIC_MOUSE_AXIS_X:
	case GIC_MOUSE_AXIS_Y:
		if( m_IsMouseButtonPressed[MBTN_LEFT] == 0
		 && m_IsMouseButtonPressed[MBTN_RIGHT] == 0 )
			break;

		iMousePosX = input.GetParamH16();
		iMousePosY = input.GetParamL16();
		iMouseMoveX = iMousePosX - m_iPrevMousePosX;
		iMouseMoveY = iMousePosY - m_iPrevMousePosY;

		if( m_IsMouseButtonPressed[MBTN_RIGHT] == 1 )
		{
			// camera roration
			AddYaw( (float)iMouseMoveX / 450.0f );
			AddPitch( - (float)iMouseMoveY / 450.0f );
		}
		else
		{
			// camera translation
			Move( (float)iMouseMoveX / 200.0f, (float)iMouseMoveY / 200.0f, 0.0f );
		}

		m_iPrevMousePosX = iMousePosX;
		m_iPrevMousePosY = iMousePosY;
		break;
	}
}
