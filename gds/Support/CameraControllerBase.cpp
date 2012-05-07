#include "CameraControllerBase.hpp"
#include "Input/InputHandler.hpp"


CCameraControllerBase::CCameraControllerBase()
:
m_Pose( Matrix34Identity() )
{
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
//	if( GetAsyncKeyState('X') & 0x8000 )	m_fYaw += 3.141592f * dt;
//	if( GetAsyncKeyState('Z') & 0x8000	)	m_fYaw -= 3.141592f * dt;
//	if( GetAsyncKeyState('Q') & 0x8000 )	m_fPitch += 3.141592f * dt;
//	if( GetAsyncKeyState('A') & 0x8000	)	m_fPitch -= 3.141592f * dt;
//	if( GetAsyncKeyState('Q') & 0x8000 )	m_vCameraPosition.y += 2.0f * dt;
//	if( GetAsyncKeyState('A') & 0x8000	)	m_vCameraPosition.y -= 2.0f * dt;
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


bool CCameraControllerBase::IsKeyPressed( int general_input_code )
{
	switch( general_input_code )
	{
	case GIC_MOUSE_BUTTON_L: return (m_IsMouseButtonPressed[MBTN_LEFT ] == 1);
	case GIC_MOUSE_BUTTON_R: return (m_IsMouseButtonPressed[MBTN_RIGHT] == 1);
	default:
		return false;
	}

	return false;
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
	switch( input.iGICode )
    {
	case GIC_MOUSE_BUTTON_L:
	case GIC_MOUSE_BUTTON_R:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
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
		if( IsKeyPressed( GIC_MOUSE_BUTTON_R ) )
			AddYaw( input.fParam1 / 500.0f ); // rotation
		else if( IsKeyPressed( GIC_MOUSE_BUTTON_L ) )
			Move( input.fParam1 / 200.0f, 0.0f, 0.0f ); // translation
		break;

	case GIC_MOUSE_AXIS_Y:
		if( IsKeyPressed( GIC_MOUSE_BUTTON_R ) )
			AddPitch( input.fParam1 / 500.0f * (-1.0f) ); // rotation
		else if( IsKeyPressed( GIC_MOUSE_BUTTON_L ) )
			Move( 0.0f, input.fParam1 / 200.0f, 0.0f ); // translation
		break;

	default:
		break;
	}
}
