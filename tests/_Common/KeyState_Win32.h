#ifndef  __KeyState_Win32_H__
#define  __KeyState_Win32_H__


#include <windows.h>


inline void init_vkcode_table( int* tbl, int array_size )
{
	for( int i=0; i<array_size; i++ )
		tbl[i] = VK_RETURN;

	tbl[KeyCode::Enter]      = VK_RETURN;
	tbl[KeyCode::Space]      = VK_SPACE;
	tbl[KeyCode::Down]       = VK_DOWN;
	tbl[KeyCode::Up]         = VK_UP;
	tbl[KeyCode::Left]       = VK_LEFT;
	tbl[KeyCode::Right]      = VK_RIGHT;
	tbl[KeyCode::Delete]     = VK_DELETE;
	tbl[KeyCode::Home]       = VK_HOME;
	tbl[KeyCode::End]        = VK_END;
//	tbl[KeyCode::PageUp]     = VK_PAGEUP;
//	tbl[KeyCode::PageDown]   = VK_PAGEDOWN;
//	tbl[KeyCode::Break]      = VK_;
	tbl[KeyCode::BackSpace]  = VK_BACK;
	tbl[KeyCode::Esc]        = VK_ESCAPE;
	tbl[KeyCode::Tab]        = VK_TAB;
	tbl[KeyCode::Control]    = VK_CONTROL;
	tbl[KeyCode::Shift]      = VK_SHIFT;
//	tbl[KeyCode::Alt]        = VK_ALT;
}


inline int ToVirtualKeyCode( KeyCode::Code key_code )
{
	static int s_tbl[512];
	static int s_initialized = 0;
	if( !s_initialized )
	{
		init_vkcode_table( s_tbl, sizeof(s_tbl) / sizeof(s_tbl[0]) );
		s_initialized = 1;
	}

	return s_tbl[ key_code ];
}


inline bool IsKeyPressed( KeyCode::Code key_code )
{
	return ( GetAsyncKeyState( ToVirtualKeyCode(key_code) ) & 0x8000 ) != 0;
}


#endif /* __KeyState_Win32_H__ */
