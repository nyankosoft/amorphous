#ifndef  __MOUSEINPUT_H__
#define  __MOUSEINPUT_H__


#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include "../base.hpp"
#include "MouseInputDevice.hpp"

#pragma comment( lib, "dinput8.lib" )


/// - Send mouse positions in reference screen coordinates
/// - When the screen resolution is changed, the mouse device object needs to be notified
///   by CDirectInputMouse::UpdateScreenSize()
/// - Internally, CDirectInputMouse stores the cursor position in non-scaled coordinates
class CDirectInputMouse : public MouseInputDevice
{
	LPDIRECTINPUTDEVICE8 m_pDIMouse;

	float m_fPrevMove_X;
	float m_fPrevMove_Y;

	/// current mouse position
	int m_iPosX, m_iPosY;

	int m_ScreenWidth;
	int m_ScreenHeight;

	enum Params
	{
		DIMOUSE_AXIS_X      =  0, ///< ??? (some deprecated param?)
		DIMOUSE_AXIS_Y      =  1, ///< ??? (some deprecated param?)
		NUM_MOUSE_BUTTONS   =  8, ///< ??? (some deprecated param?)
		DIMOUSE_BUFFER_SIZE = 32, ///< size of the buffer to hold input data from mouse (DirectInput)
	};

private:

	HRESULT InitDIMouse( HWND hWnd );

public:

	CDirectInputMouse();

	~CDirectInputMouse();

	Result::Name Init();

	void Release();

	void AcquireMouse();

	void UnacquireMouse() { m_pDIMouse->Unacquire(); }

	/// updates the rect area for the mouse cursor
	void UpdateScreenSize( int width, int height ) { m_ScreenWidth = width, m_ScreenHeight = height; }

/*	float GetSensitivity( int iMouseAxis ) { return m_afMouseSensitivity[iMouseAxis]; }
	void SetSensitivity( int iMouseAxis, float fSensitivity ) { m_afMouseSensitivity[iMouseAxis] = fSensitivity; }
	bool InvertMouse();
	void SetInvertMouse( bool bEnableInvertMouse );
*/
	/// returns the current mouse cursor position in the current screen resolution
	inline void GetCurrentPosition( int& x, int& y ) const;

	inline int GetCurrentPositionX() const { return m_iPosX; }
	inline int GetCurrentPositionY() const { return m_iPosY; }

	HRESULT UpdateInput();

	Result::Name SendBufferedInputToInputHandlers();
};

//----------------------- inline implementation -----------------------

inline void CDirectInputMouse::GetCurrentPosition( int& x, int& y ) const
{
	x = m_iPosX;
	y = m_iPosY;
}



#endif		/*  __MOUSEINPUT_H__  */
