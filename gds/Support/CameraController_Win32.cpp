
#include "CameraController_Win32.h"

#include "GameInput/InputHandler.h"
#include "3DCommon/Direct3D9.h"


int ConvertGICodeToWin32VKCode( int general_input_code )
{
	static int s_Initialized = 0;
	static int s_GICtoVKC[NUM_GENERAL_INPUT_CODES];
	if( s_Initialized == 0 )
	{
		s_GICtoVKC[GIC_SPACE]   = VK_SPACE;
		s_GICtoVKC[GIC_TAB]     = VK_TAB;
		s_GICtoVKC[GIC_UP]      = VK_UP;
		s_GICtoVKC[GIC_DOWN]    = VK_DOWN;
		s_GICtoVKC[GIC_RIGHT]   = VK_RIGHT;
		s_GICtoVKC[GIC_LEFT]    = VK_LEFT;
		s_GICtoVKC[GIC_DELETE]  = VK_DELETE;
		s_GICtoVKC[GIC_BACK]    = VK_BACK;
		s_GICtoVKC[GIC_ESC]     = VK_ESCAPE;

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


inline static bool IsKeyPressed( int general_input_code )
{
	if( !IsValidGeneralInputCode( general_input_code ) )
		return false;

	return ( ( GetAsyncKeyState( ConvertGICodeToWin32VKCode(general_input_code) ) & 0x8000 ) != 0 );
}



CCameraController_Win32::CCameraController_Win32()
{
	m_Pose = Matrix34Identity();

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


CCameraController_Win32::~CCameraController_Win32()
{
}


void CCameraController_Win32::UpdateCameraPosition( float dt )
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

	D3DXMatrixRotationYawPitchRoll( &matRot, m_fYaw, m_fPitch, 0 );
	D3DXVec3TransformCoord( &vRight, &vRight, &matRot);
	D3DXVec3TransformCoord( &vUp,    &vUp,    &matRot);
	D3DXVec3TransformCoord( &vDir,   &vDir,   &matRot);

	m_Pose.matOrient.SetColumn( 0, vRight );
	m_Pose.matOrient.SetColumn( 1, vUp );
	m_Pose.matOrient.SetColumn( 2, vDir );

	m_Pose.vPosition
		+= m_Pose.matOrient.GetColumn(2) * forward
		+ m_Pose.matOrient.GetColumn(0) * right
		+ m_Pose.matOrient.GetColumn(1) * up;
//	m_Pose.vPosition += m_vRight * m_fMouseMoveRight + m_vUp * m_fMouseMoveUp;
//	m_fMouseMoveRight = 0; m_fMouseMoveUp = 0;

}


void CCameraController_Win32::AddYaw( float fYaw )
{
	m_fYaw += fYaw;
}


void CCameraController_Win32::AddPitch( float fPitch )
{
	m_fPitch += fPitch;
}


void CCameraController_Win32::SetCameraMatrix()
{
	D3DXMATRIX matCamera;

	GetCameraMatrix( matCamera );

	DIRECT3D9.GetDevice()->SetTransform( D3DTS_VIEW, &matCamera );
}


void CCameraController_Win32::HandleMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
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


void CCameraController_Win32::AssignKeyForCameraControl( int general_input_code, CameraControl::Operation cam_control_op )
{
	if( NUM_GENERAL_INPUT_CODES <= general_input_code )
		return;

	m_CameraControlCode[cam_control_op] = general_input_code;
}


/*
	add the following codes to the message handler to enable mouse operations


*/
