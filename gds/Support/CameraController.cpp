#include "CameraController.hpp"
#include "Input/InputDevice.hpp"


namespace amorphous
{


CCameraController::CCameraController( int input_handler_index )
:
m_InputHandlerIndex(input_handler_index)
{
	m_pInputDataDelagate.reset( new CInputDataDelegate<CCameraController>(this) );

	if( InputHub().GetInputHandler(input_handler_index) )
		InputHub().GetInputHandler(input_handler_index)->AddChild( m_pInputDataDelagate.get() );
	else
		InputHub().PushInputHandler( input_handler_index, m_pInputDataDelagate.get() );
}


CCameraController::CCameraController( CInputHandler *pParentInputHandler )
{
	m_pInputDataDelagate.reset( new CInputDataDelegate<CCameraController>(this) );

	if( !pParentInputHandler )
		return;

	pParentInputHandler->AddChild( m_pInputDataDelagate.get() );
}


CCameraController::~CCameraController()
{
	InputHub().RemoveInputHandler( m_InputHandlerIndex, m_pInputDataDelagate.get() );
}


bool CCameraController::IsKeyPressed( int general_input_code )
{
	bool res = CCameraControllerBase::IsKeyPressed( general_input_code );
	if( res )
		return true;

	if( !IsValidGeneralInputCode( general_input_code ) )
		return false;

//	return ( InputHub().GetInputState(general_input_code) == CInputState::PRESSED );
	return ( InputDeviceHub().GetInputDeviceGroup(0)->GetInputState(general_input_code) == CInputState::PRESSED );
}


// Need to avoid calling this when mouse operation is notified by Win32 message
// - See CameraController_Win32.cpp
//void CCameraControllerInputHandler::HandleInput( const SInputData& input )
//{
//}


} // namespace amorphous
