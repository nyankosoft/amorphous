#ifndef  __DIKEYBOARD_H__
#define  __DIKEYBOARD_H__

#include "DirectInput.h"
#include "InputHandler.h"

#include <dinput.h>
#include <windows.h>

#pragma comment( lib, "dinput8.lib" )


class CDIKeyboard// : public CInputDevice
{
	LPDIRECTINPUTDEVICE8 m_pKeyboard; // The keyboard device

	enum param
	{
		NUM_SUPPORTED_DIK_CODES = 512
	};

	int m_aiGICodeForDIKCode[NUM_SUPPORTED_DIK_CODES];

public:

	CDIKeyboard();
	~CDIKeyboard();

	HRESULT Init( HWND hWnd );
	void InitKeyCodeMap();
	HRESULT GetKeyState( BYTE *pacKeyboardStateBuffer );
	HRESULT ReadBufferedData();
	void Acquire();

	void Release();
};



#endif		/*  __DIKEYBOARD_H__  */