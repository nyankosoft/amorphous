#include "StdMouseInput.hpp"
#include "Input/InputHub.hpp"


namespace amorphous
{


Win32StdMouseInput::Win32StdMouseInput()
{
	m_iPosX = 0;
	m_iPosY = 0;

	GetInputDeviceHub().RegisterInputDeviceToGroup( this );
}


Result::Name Win32StdMouseInput::SendBufferedInputToInputHandlers()
{
	return Result::UNKNOWN_ERROR;
}


void Win32StdMouseInput::UpdateInput( UINT msg, WPARAM wParam, LPARAM lParam )
{
	InputData input;

	int iMouseMoveX, iMouseMoveY;
	int iNewPosX, iNewPosY;
	switch( msg )
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		if( msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN )
			input.iType = ITYPE_KEY_PRESSED;
		else if( msg == WM_LBUTTONUP || msg == WM_RBUTTONUP )
			input.iType = ITYPE_KEY_RELEASED;

		// update positions
		m_iPosX = LOWORD(lParam);
		m_iPosY = HIWORD(lParam);

		input.SetParamH16( (short)m_iPosX );
		input.SetParamL16( (short)m_iPosY );

		if( msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP )
		{
			input.iGICode = GIC_MOUSE_BUTTON_L;
			UpdateInputState( input );
			GetInputHub().UpdateInput( input );
		}
		else if( msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP )
		{
			input.iGICode = GIC_MOUSE_BUTTON_R;
			UpdateInputState( input );
			GetInputHub().UpdateInput( input );
		}
		else if( msg == WM_MBUTTONDOWN )
		{
			input.iGICode = GIC_MOUSE_BUTTON_M;
			UpdateInputState( input );
			GetInputHub().UpdateInput( input );
		}

		break;

	case WM_MOUSEMOVE:
		input.iType = ITYPE_KEY_PRESSED;

		// update position
		iNewPosX = LOWORD(lParam);
		iNewPosY = HIWORD(lParam);
		iMouseMoveX = iNewPosX - m_iPosX;
		iMouseMoveY = iNewPosY - m_iPosY;

		input.SetParamH16( (short)iNewPosX );
		input.SetParamL16( (short)iNewPosY );

		if( iMouseMoveX != 0 )
		{
			input.iGICode = GIC_MOUSE_AXIS_X;
			input.fParam1 = (float)iMouseMoveX;
			GetInputHub().UpdateInput( input );
		}

		if( iMouseMoveY != 0 )
		{
			input.iGICode = GIC_MOUSE_AXIS_Y;
			input.fParam1 = (float)iMouseMoveY;
			GetInputHub().UpdateInput( input );
		}

		m_iPosX = iNewPosX;
		m_iPosY = iNewPosY;
		break;

	case WM_MOUSEWHEEL:
		signed short rotated_dist = (signed short)HIWORD(wParam);
//		input.iGICode = GIC_MOUSE_WHEEL_UP;
		input.iGICode = ( 0 < rotated_dist ) ? GIC_MOUSE_WHEEL_UP : GIC_MOUSE_WHEEL_DOWN;
		UpdateInputState( input );
		GetInputHub().UpdateInput( input );
		break;
	}

}


} // namespace amorphous
