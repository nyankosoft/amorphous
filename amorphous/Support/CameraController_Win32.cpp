#include "CameraController_Win32.hpp"
#include "Input/InputHandler.hpp"


namespace amorphous
{


int ConvertGICodeToWin32VKCode( int general_input_code )
{
	static int s_Initialized = 0;
	static int s_GICtoVKC[NUM_GENERAL_INPUT_CODES];
	if( s_Initialized == 0 )
	{
		s_GICtoVKC[GIC_SPACE]    = VK_SPACE;
		s_GICtoVKC[GIC_TAB]      = VK_TAB;
		s_GICtoVKC[GIC_UP]       = VK_UP;
		s_GICtoVKC[GIC_DOWN]     = VK_DOWN;
		s_GICtoVKC[GIC_RIGHT]    = VK_RIGHT;
		s_GICtoVKC[GIC_LEFT]     = VK_LEFT;
		s_GICtoVKC[GIC_DELETE]   = VK_DELETE;
		s_GICtoVKC[GIC_BACK]     = VK_BACK;
		s_GICtoVKC[GIC_ESC]      = VK_ESCAPE;
		s_GICtoVKC[GIC_ENTER]    = VK_RETURN;
		s_GICtoVKC[GIC_LSHIFT]   = VK_SHIFT;
		s_GICtoVKC[GIC_RSHIFT]   = VK_SHIFT;
		s_GICtoVKC[GIC_LCONTROL] = VK_CONTROL;
		s_GICtoVKC[GIC_RCONTROL] = VK_CONTROL;

		s_Initialized = 1;
	}

	if( general_input_code < 0 || NUM_GENERAL_INPUT_CODES <= general_input_code )
	{
		// invalid general input code
		return VK_ESCAPE;
	}
	else if( 'A' <= general_input_code && general_input_code <= 'Z' )
	{
		// alphabet key - no need to convert
		return general_input_code;
	}
	else
	{
		// use the conversion table
		return s_GICtoVKC[general_input_code];
	}
}



//================================================================================
// CPlatformDependentCameraController
//================================================================================

bool CPlatformDependentCameraController::IsKeyPressed( int general_input_code )
{
	if( !IsValidGeneralInputCode( general_input_code ) )
		return false;

	return ( ( GetAsyncKeyState( ConvertGICodeToWin32VKCode(general_input_code) ) & 0x8000 ) != 0 );
}


void CPlatformDependentCameraController::HandleMessage( UINT msg, WPARAM wParam, LPARAM lParam )
{
	int iMousePosX, iMousePosY, iMouseMoveX, iMouseMoveY;

    switch( msg )
    {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			m_iPrevMousePosX = LOWORD(lParam);
			m_iPrevMousePosY = HIWORD(lParam);
			break;

		case WM_MOUSEMOVE:
			if( !(wParam & MK_RBUTTON) && !(wParam & MK_LBUTTON) )
				break;
			
			iMousePosX = LOWORD(lParam);
			iMousePosY = HIWORD(lParam);
			iMouseMoveX = iMousePosX - m_iPrevMousePosX;
			iMouseMoveY = iMousePosY - m_iPrevMousePosY;

			if( wParam & MK_RBUTTON )
			{
				// camera roration
				AddYaw( iMouseMoveX / 450.0f );
				AddPitch( - iMouseMoveY / 450.0f );
			}
			else
			{
				// camera translation
				Move( iMouseMoveX / 200.0f, iMouseMoveY / 200.0f, 0.0f );
			}

			m_iPrevMousePosX = iMousePosX;
			m_iPrevMousePosY = iMousePosY;
			break;
	}
}


} // namespace amorphous
