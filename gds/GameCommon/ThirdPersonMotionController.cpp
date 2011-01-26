#include "ThirdPersonMotionController.hpp"
#include "../Item/SkeletalCharacter.hpp"
#include "../Input/InputHandler.hpp"

using namespace boost;


CThirdPersonMotionController::CThirdPersonMotionController()
:
m_fVeritcalCameraSpaceInput(0),
m_fHorizontalCameraSpaceInput(0)
{
}


void CThirdPersonMotionController::Update()
{
	if( !m_pCharacter )
		return;

	CSkeletalCharacter& skeletal_character = *m_pCharacter;

//	if( !skeletal_character.IsCameraDependentMotionControlEnabled() )
//		return;

	if( fabsf( m_fHorizontalCameraSpaceInput ) < 0.1f
	 && fabsf( m_fVeritcalCameraSpaceInput ) < 0.1f )
	{
		skeletal_character.SetFwdSpeed( 0.0f );
		skeletal_character.SetTurnSpeed( 0.0f );
		skeletal_character.SetDesiredHorizontalDirection( Vector3(0,0,0) );
		return;
	}

	Vector3 vInput = Vector3( m_fHorizontalCameraSpaceInput, 0, m_fVeritcalCameraSpaceInput );
	float input_vector_length = vInput.GetLength();
	vInput /= input_vector_length;

//	if( 1.0f < vInput.GetLength() )
//		vInput = vInput / GetLength();

	CEntityHandle<CItemEntity> entity = m_pCharacter->GetItemEntity();
	shared_ptr<CItemEntity> pEntity = entity.Get();
	if( !pEntity )
		return;

	Matrix33 character_orient = pEntity->GetWorldPose().matOrient;
	if( character_orient.GetColumn(1).GetLength() < 0.001f )
		return;

	Horizontalize( character_orient );

	Matrix33 cam_orient = m_CameraPose.matOrient;// Camera().GetPose().matOrient;// m_CameraOrientation.current.ToRotationMatrix();

	Horizontalize( cam_orient );

	// Calculate the desired direction in world space
	Vector3 vWorldDesiredDirection = cam_orient * vInput;

	skeletal_character.SetDesiredHorizontalDirection( vWorldDesiredDirection );

	// gamepad analog axis-x/y: [0,2]
	// keyboad up/down/left/right: [0,1]
	float fwd_speed = input_vector_length * 0.5f;
	if(skeletal_character.GetActionInputState(ACTION_MOV_BOOST) == CInputState::PRESSED)
		fwd_speed *= 2.0f;
	clamp( fwd_speed, 0.0f, 1.0f );

//	float speed = ( 0.1f < GetActionInput(*m_pKeyBind,ACTION_MOV_BOOST) ) ? 1.0f : 0.5f;

	if( 0.75f < fwd_speed )
		int boost_input_detected = 1;

	skeletal_character.SetFwdSpeed( fwd_speed );
}


float GetAnalogMotionInput( const SInputData& input, float sign )
{
	if( input.iType == ITYPE_KEY_PRESSED
	 || input.iType == ITYPE_VALUE_CHANGED )
	{
		return input.fParam1 * sign;
	}
	else if( input.iType == ITYPE_KEY_RELEASED )
	{
		return 0;
	}

	return 0;
}


void CThirdPersonMotionController::HandleInput( int action_code, const SInputData& input )
{
	switch( action_code )
	{
	case ACTION_MOV_FORWARD:
//		if( input.iType == ITYPE_KEY_PRESSED
//		 || input.iType == ITYPE_VALUE_CHANGED )
//		{
//			m_fVerticalMotion = input.fParam1;
//		}
//		else if( input.iType == ITYPE_KEY_RELEASED )
//		{
//			m_fVerticalMotion = 0;
//		}
		LOG_PRINT( " ACTION_MOV_FORWARD: " + to_string(input.fParam1) );
		m_fVeritcalCameraSpaceInput = GetAnalogMotionInput( input, 1.0f );
		break;

	case ACTION_MOV_BACKWARD:
		LOG_PRINT( " ACTION_MOV_BACKWARD: " + to_string(input.fParam1) );
		m_fVeritcalCameraSpaceInput = GetAnalogMotionInput( input, -1.0f );
		break;

	case ACTION_MOV_TURN_R:
		m_fHorizontalCameraSpaceInput = GetAnalogMotionInput( input, 1.0f );
		break;

	case ACTION_MOV_TURN_L:
		m_fHorizontalCameraSpaceInput = GetAnalogMotionInput( input, -1.0f );
		break;

	default:
		break;
	}
}
