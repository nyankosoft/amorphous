#ifndef  __KeyState_H__
#define  __KeyState_H__


class KeyCode
{
public:

	enum Code
	{
		Enter = 0xFF,
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
		NumKeyCodes = Alt - Enter + 1
	};
};


#ifdef _MSC_VER

#include "KeyState_Win32.hpp"

#endif // _MSC_VER


#endif /* __KeyState_H__ */
