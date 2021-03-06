#include "CameraControllerBase.hpp"
#include "amorphous/Input/InputHub.hpp"
#include "amorphous/Input/InputDevice.hpp"
#include "amorphous/Support/Log/DefaultLogAux.hpp"
#include "amorphous/Support/Macro.h"


namespace amorphous
{


CameraControllerBase::CameraControllerBase()
:
m_Pose( Matrix34Identity() ),
m_Active(true)
{
	m_fYaw = 0.0f;
	m_fPitch = 0.0f;

	m_fTranslationSpeed = 2.0f;//8.0f;

	m_iPrevMousePosX = 0;
	m_iPrevMousePosY = 0;

	// set up standard keybinds
	m_CameraControlCode[CameraControl::Forward]  = 'W';
	m_CameraControlCode[CameraControl::Backward] = 'S';
	m_CameraControlCode[CameraControl::Right]    = 'D';
	m_CameraControlCode[CameraControl::Left]     = 'A';
	m_CameraControlCode[CameraControl::Up]       = 'Q';
	m_CameraControlCode[CameraControl::Down]     = 'C';

	memset( m_IsMouseButtonPressed, 0, sizeof(m_IsMouseButtonPressed) );
}


CameraControllerBase::~CameraControllerBase()
{
}


void CameraControllerBase::UpdateCameraPose( float dt )
{
	if( !m_Active )
		return;

	float forward=0, right=0, up=0, spd;
//	Matrix33 matRot;

	if( IsKeyPressed(GIC_LSHIFT) )
		spd = m_fTranslationSpeed * 2.0f;
	else
		spd = m_fTranslationSpeed * 1.0f;

	int *kb = m_CameraControlCode;

	if( IsKeyPressed(kb[CameraControl::Forward]) )  forward =  spd * dt;
	if( IsKeyPressed(kb[CameraControl::Backward]) ) forward = -spd * dt;
	if( IsKeyPressed(kb[CameraControl::Right]) )    right   =  spd * dt;
	if( IsKeyPressed(kb[CameraControl::Left]) )     right   = -spd * dt;
	if( IsKeyPressed(kb[CameraControl::Up]) )       up      =  spd * dt * 3.0f;
	if( IsKeyPressed(kb[CameraControl::Down]) )     up      = -spd * dt * 3.0f;

	bool fwd_key_pressed   = IsKeyPressed(kb[CameraControl::Forward]);
	bool right_key_pressed = IsKeyPressed(kb[CameraControl::Right]);
	//LOG_PRINTF(("fwd_key_pressed: %d",fwd_key_pressed ? 1 : 0));
	//LOG_PRINTF(("right_key_pressed: %d",right_key_pressed ? 1 : 0));


	if( IsKeyPressed('X') )	m_fYaw   += PI * 0.5f * dt;
	if( IsKeyPressed('Z') ) m_fYaw   -= PI * 0.5f * dt;
	if( IsKeyPressed('R') )	m_fPitch += PI * 0.5f * dt;
	if( IsKeyPressed('F') ) m_fPitch -= PI * 0.5f * dt;
//	if( IsKeyPressed('Q') ) m_vCameraPosition.y += 2.0f * dt;
//	if( IsKeyPressed('A') ) m_vCameraPosition.y -= 2.0f * dt;

	PERIODICAL( 500, LOG_PRINTF(("yaw: %f, pitch: %f",m_fYaw,m_fPitch)) );

	Matrix34 pose = GetPose();

	// Rotation: m_fYaw and m_fPitch represent the absolute angles of rotation
	pose.matOrient = Matrix33RotationY(m_fYaw) * Matrix33RotationX(m_fPitch);

	// Position: Unlike rotation, positions are updated by adding traveled amount to the current location
	pose.vPosition
		+= pose.matOrient.GetColumn(2) * forward
		+  pose.matOrient.GetColumn(0) * right
		+  pose.matOrient.GetColumn(1) * up;

	SetPose( pose );

//	m_Pose.vPosition += m_vRight * m_fMouseMoveRight + m_vUp * m_fMouseMoveUp;
//	m_fMouseMoveRight = 0; m_fMouseMoveUp = 0;
}


bool CameraControllerBase::IsKeyPressed( int general_input_code )
{
	switch( general_input_code )
	{
	case GIC_MOUSE_BUTTON_L: return (m_IsMouseButtonPressed[MBTN_LEFT ] == 1);
	case GIC_MOUSE_BUTTON_R: return (m_IsMouseButtonPressed[MBTN_RIGHT] == 1);
	default:
		return ( GetInputDeviceHub().GetInputDeviceGroup(0)->GetInputState(general_input_code) == CInputState::PRESSED );
	}

	return false;
}


void CameraControllerBase::AddYaw( float fYaw )
{
	m_fYaw += fYaw;
}


void CameraControllerBase::AddPitch( float fPitch )
{
	m_fPitch += fPitch;
}


void CameraControllerBase::AssignKeyForCameraControl( int general_input_code, CameraControl::Operation cam_control_op )
{
	if( NUM_GENERAL_INPUT_CODES <= general_input_code )
		return;

	m_CameraControlCode[cam_control_op] = general_input_code;
}


void CameraControllerBase::HandleInput( const InputData& input )
{
	LOG_PRINTF(("code: %d",input.iGICode));

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


} // namespace amorphous
