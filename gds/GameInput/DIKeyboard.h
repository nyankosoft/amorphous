#ifndef  __DIKEYBOARD_H__
#define  __DIKEYBOARD_H__

#include "DirectInput.h"
#include "InputHandler.h"
#include "InputDevice.h"
#include "../base.h"

#include <dinput.h>
#include <windows.h>

#pragma comment( lib, "dinput8.lib" )


class CDIKeyboard : public CInputDevice
{
	LPDIRECTINPUTDEVICE8 m_pKeyboard; // The keyboard device

	enum param
	{
		NUM_SUPPORTED_DIK_CODES = 512
	};

	int m_aiGICodeForDIKCode[NUM_SUPPORTED_DIK_CODES];

private:

	HRESULT InitDIKeyboard( HWND hWnd );

public:

	CDIKeyboard();
	~CDIKeyboard();

	Result::Name Init();

	void InitKeyCodeMap();

	HRESULT GetKeyState( BYTE *pacKeyboardStateBuffer );

	HRESULT ReadBufferedData();

	Result::Name SendBufferedInputToInputHandlers();

	void Acquire();

	void Release();
};



#endif		/*  __DIKEYBOARD_H__  */
