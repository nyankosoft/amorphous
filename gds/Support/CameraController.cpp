#include "CameraController.hpp"
#include "Input/InputHub.hpp"

using namespace boost;


void CreateCameraMatrixFromCameraPose( const Matrix34& src_camera_pose, D3DXMATRIX& dest_camera_matrix )
{
}


CCameraController::CCameraController( int input_handler_index )
:
m_InputHandlerIndex(input_handler_index)
{
	m_pInputHandler = shared_ptr<CCameraControllerInputHandler>( new CCameraControllerInputHandler(this) );

	InputHub().PushInputHandler( m_InputHandlerIndex, m_pInputHandler.get() );
}


CCameraController::~CCameraController()
{
//	InputHub().RemoveInputHandler( m_InputHandlerIndex, m_pInputHandler.get() );
	InputHub().SetInputHandler( m_InputHandlerIndex, NULL );
}


bool CCameraController::IsKeyPressed( int general_input_code )
{
	if( !IsValidGeneralInputCode( general_input_code ) )
		return false;

	return ( InputHub().GetInputState(general_input_code) == CInputState::PRESSED );
}


void CCameraControllerInputHandler::ProcessInput(SInputData& input)
{
//	if( IsMouseInput(input.iGICode) )
//		m_pCameraController->
}
