#ifndef  __MOUSEINPUT_H__
#define  __MOUSEINPUT_H__


#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>

#pragma comment( lib, "dinput8.lib" )

enum eMouseAction
{
	MOUSE_BUTTON_L = 0,
	MOUSE_BUTTON_R,
	MOUSE_BUTTON_M,
	MOUSE_AXIS_X,
	MOUSE_AXIS_Y,
	MOUSE_AXIS_Z

};

#define DIMOUSE_AXIS_X 0
#define DIMOUSE_AXIS_Y 1

#define NUM_MOUSE_BUTTONS 8
#define DIMOUSE_BUFFER_SIZE 32	//size of the buffer to hold input data from mouse (DirectInput)

class CDirectInputMouse// : public CInputDevice
{
	LPDIRECTINPUTDEVICE8 m_pDIMouse;

	float m_fPrevMove_X;
	float m_fPrevMove_Y;

	/// current mouse position
	int m_iPosX, m_iPosY;

	int m_ScreenWidth;
	int m_ScreenHeight;

public:

	CDirectInputMouse();

	~CDirectInputMouse();

	HRESULT Init( HWND hWnd );

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
	inline void GetCurrentPosition( int& x, int& y ) const { x = m_iPosX; y = m_iPosY; }

	inline int GetCurrentPositionX() const { return m_iPosX; }
	inline int GetCurrentPositionY() const { return m_iPosY; }

	HRESULT UpdateInput();

};

#endif		/*  __MOUSEINPUT_H__  */
