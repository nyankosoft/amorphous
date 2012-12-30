#include "GameWindowManager_Win32.hpp"
#include "Support/WindowMisc_Win32.hpp"


namespace amorphous
{


CGameWindowManager_Win32 *g_pGameWindowManager_Win32 = NULL;


CGameWindowManager_Win32::CGameWindowManager_Win32()
:
m_hWnd(NULL)
{
	memset( &m_WindowClassEx, 0, sizeof(WNDCLASSEX) );
}


bool CGameWindowManager_Win32::IsMouseCursorInClientArea()
{
	long frame_w = 0, frame_h = 0;
	GetNonClientAreaSize( m_hWnd, frame_w, frame_h );

	RECT window_rect;
	GetWindowRect( m_hWnd, &window_rect );

	// TODO: accurate client rect position
	RECT offset_window_rect = window_rect;
	offset_window_rect.top   += frame_h;
	offset_window_rect.left  += frame_w;
	offset_window_rect.right -= frame_w;

	POINT pt;
	GetCursorPos( &pt );

	if( offset_window_rect.top <= pt.y
	 && pt.y <= offset_window_rect.bottom
	 && offset_window_rect.left <= pt.x
	 && pt.x <= offset_window_rect.right )
	{
		return true;
	}
	else
		return false;
}


void CGameWindowManager_Win32::SetWindowLeftTopCornerPosition( int left, int top )
{
	// set the position
	// - use SWP_NOSIZE flag to ignore window size parameters
	::SetWindowPos( m_hWnd, HWND_TOP, left, top, 0, 0, SWP_NOSIZE );
}


} // namespace amorphous
