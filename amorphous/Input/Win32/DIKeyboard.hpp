#ifndef  __DIKEYBOARD_H__
#define  __DIKEYBOARD_H__

#include "amorphous/base.hpp"
#include "amorphous/Input/InputHandler.hpp"
#include "amorphous/Input/KeyboardInputDevice.hpp"
#include "DirectInput.hpp"


namespace amorphous
{


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
