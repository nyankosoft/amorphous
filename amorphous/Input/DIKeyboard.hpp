#ifndef  __DIKEYBOARD_H__
#define  __DIKEYBOARD_H__

#include "DirectInput.hpp"
#include "InputHandler.hpp"
#include "KeyboardInputDevice.hpp"
#include "../base.hpp"

#include <dinput.h>
#include <windows.h>


namespace amorphous
{

#pragma comment( lib, "dinput8.lib" )


class DIKeyboard : public KeyboardInputDevice
{
	LPDIRECTINPUTDEVICE8 m_pKeyboard; ///< The DirectX Input keyboard device

	/// Stores key states. Refreshed by RefreshKeyState(). Used to detect missed release events
	BYTE m_DIKs[256];

	enum param
	{
		NUM_SUPPORTED_DIK_CODES = 512
	};

	int m_aiGICodeForDIKCode[NUM_SUPPORTED_DIK_CODES];

private:

	HRESULT InitDIKeyboard( HWND hWnd );

protected:

	void RefreshKeyStates();

	bool IsReleventInput( int gi_code ) { return IsKeyboardInputCode( gi_code ); }

	bool IsKeyPressed( int gi_code );

public:

	DIKeyboard();

	~DIKeyboard();

	Result::Name Init();

	void InitKeyCodeMap();

	HRESULT ReadBufferedData();

	Result::Name SendBufferedInputToInputHandlers();

	void Acquire();

	void Release();
};


} // namespace amorphous



#endif		/*  __DIKEYBOARD_H__  */
