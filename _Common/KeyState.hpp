#ifndef  __KeyState_H__
#define  __KeyState_H__


class KeyCode
{
public:

	enum Code
	{
		Enter,
		Space,
		Down,
		Up,
		Left,
		Right,
		Delete,
		Home,
		End,
		PageUp,
		PageDown,
		Break,
		BackSpace,
		Esc,
		Tab,
		Control,
		Shift,
		Alt,
//		RightAlt,
//		LeftAlt,
		NumKeyCodes
	};
};


#define PLATFORM_IS_WINDOWS

#ifdef PLATFORM_IS_WINDOWS

#include "KeyState_Win32.hpp"

#endif /* PLATFORM_IS_WINDOWS */


#ifdef PLATFORM_IS_LINUX

// ...

#endif /* PLATFORM_IS_LINUX */



#endif /* __KeyState_H__ */
