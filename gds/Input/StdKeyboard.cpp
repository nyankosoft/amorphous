
#include "StdKeyboard.hpp"

#include "InputHub.hpp"

#include <windows.h>


CStdKeyboard::CStdKeyboard()
{
	for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
		m_aiGICodeForVKCode[i] = GIC_INVALID;

	m_aiGICodeForVKCode[VK_F1]    = GIC_F1;
	m_aiGICodeForVKCode[VK_F2]    = GIC_F2;
	m_aiGICodeForVKCode[VK_F3]    = GIC_F3;
	m_aiGICodeForVKCode[VK_F4]    = GIC_F4;
	m_aiGICodeForVKCode[VK_F5]    = GIC_F5;
	m_aiGICodeForVKCode[VK_F6]    = GIC_F6;
	m_aiGICodeForVKCode[VK_F7]    = GIC_F7;
	m_aiGICodeForVKCode[VK_F8]    = GIC_F8;
	m_aiGICodeForVKCode[VK_F9]    = GIC_F9;
	m_aiGICodeForVKCode[VK_F10]   = GIC_F10;
	m_aiGICodeForVKCode[VK_F11]   = GIC_F11;
	m_aiGICodeForVKCode[VK_F12]   = GIC_F12;

	m_aiGICodeForVKCode[VK_RETURN]   = GIC_ENTER;
	m_aiGICodeForVKCode[VK_SPACE]    = GIC_SPACE;
	m_aiGICodeForVKCode[VK_LSHIFT]   = GIC_LSHIFT;
	m_aiGICodeForVKCode[VK_RSHIFT]   = GIC_RSHIFT;
	m_aiGICodeForVKCode[VK_RCONTROL] = GIC_RCONTROL,
	m_aiGICodeForVKCode[VK_LCONTROL] = GIC_LCONTROL;

	m_aiGICodeForVKCode[VK_CONTROL]  = GIC_LCONTROL;

//	m_aiGICodeForVKCode[VK_RALT]     = GIC_RALT,
//	m_aiGICodeForVKCode[VK_LALT]     = GIC_LALT;
	m_aiGICodeForVKCode[VK_TAB]      = GIC_TAB;
	m_aiGICodeForVKCode[VK_BACK]     = GIC_BACK;
	m_aiGICodeForVKCode[VK_ESCAPE]   = GIC_ESC;
	m_aiGICodeForVKCode[VK_UP]       = GIC_UP;
	m_aiGICodeForVKCode[VK_DOWN]     = GIC_DOWN;
	m_aiGICodeForVKCode[VK_RIGHT]    = GIC_RIGHT;
	m_aiGICodeForVKCode[VK_LEFT]     = GIC_LEFT;

	m_aiGICodeForVKCode[VK_INSERT]   = GIC_INSERT;
	m_aiGICodeForVKCode[VK_DELETE]   = GIC_DELETE;

	m_aiGICodeForVKCode[VK_HOME]     = GIC_HOME;
	m_aiGICodeForVKCode[VK_END]      = GIC_END;

	m_aiGICodeForVKCode[VK_PRIOR]    = GIC_PAGE_UP;
	m_aiGICodeForVKCode[VK_NEXT]     = GIC_PAGE_DOWN;
//	m_aiGICodeForVKCode[VK_NOCONVERT] = GIC_NOCONVERT;

	// char values 'A' to 'Z' are directly used as general input codes
	// just like virtual key codes
	for( char c='A'; c<='Z'; c++ )
		m_aiGICodeForVKCode[c] = c;

	// This is also true for '0' to '9'
	for( char c='0'; c<='9'; c++ )
		m_aiGICodeForVKCode[c] = c;

	m_aiGICodeForVKCode[VK_NUMPAD0]  = GIC_NUMPAD0;
	m_aiGICodeForVKCode[VK_NUMPAD1]  = GIC_NUMPAD1;
	m_aiGICodeForVKCode[VK_NUMPAD2]  = GIC_NUMPAD2;
	m_aiGICodeForVKCode[VK_NUMPAD3]  = GIC_NUMPAD3;
	m_aiGICodeForVKCode[VK_NUMPAD4]  = GIC_NUMPAD4;
	m_aiGICodeForVKCode[VK_NUMPAD5]  = GIC_NUMPAD5;
	m_aiGICodeForVKCode[VK_NUMPAD6]  = GIC_NUMPAD6;
	m_aiGICodeForVKCode[VK_NUMPAD7]  = GIC_NUMPAD7;
	m_aiGICodeForVKCode[VK_NUMPAD8]  = GIC_NUMPAD8;
	m_aiGICodeForVKCode[VK_NUMPAD9]  = GIC_NUMPAD9;
	
	m_aiGICodeForVKCode[VK_MULTIPLY] = GIC_MULTIPLY;
	m_aiGICodeForVKCode[VK_DIVIDE]   = GIC_DIVIDE;
	m_aiGICodeForVKCode[VK_ADD]      = GIC_NUMPAD_PLUS;
	m_aiGICodeForVKCode[VK_SUBTRACT] = GIC_NUMPAD_MINUS;
	m_aiGICodeForVKCode[VK_DECIMAL]  = GIC_NUMPAD_DECIMAL;
	m_aiGICodeForVKCode[VK_SEPARATOR] = GIC_NUMPAD_ENTER;

//	m_aiGICodeForVKCode[13] = GIC_NUMPAD_ENTER;

/*	m_aiGICodeForVKCode[VK_]    = '';
	m_aiGICodeForVKCode[VK_]    = '';
	m_aiGICodeForVKCode[VK_]    = '';*/
}


void CStdKeyboard::NotifyKeyDown( int iVK_Code )
{
	SInputData input;

	input.iGICode= m_aiGICodeForVKCode[iVK_Code];

	if( input.iGICode == GIC_INVALID )
		return;	// no GIC for this virtual key code

	input.iType = ITYPE_KEY_PRESSED;
	input.fParam1 = 1.0f;

	// send input to input hub
	InputHub().UpdateInput(input);
}


void CStdKeyboard::NotifyKeyUp( int iVK_Code )
{
	SInputData input;

	input.iGICode= m_aiGICodeForVKCode[iVK_Code];

	if( input.iGICode == GIC_INVALID )
		return;	// no GIC for this virtual key code

	input.iType = ITYPE_KEY_RELEASED;
	input.fParam1 = 0.0f;

	// send input to input hub
	InputHub().UpdateInput(input);
}


