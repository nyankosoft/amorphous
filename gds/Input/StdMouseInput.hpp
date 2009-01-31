
#ifndef  __STDMOUSEINPUT_H__
#define  __STDMOUSEINPUT_H__

#include <windows.h>


class CStdMouseInput
{
	int m_iPosX, m_iPosY;	// current mouse position
	
	enum eMouseAction
	{
		MOUSE_BUTTON_L = 0,
		MOUSE_BUTTON_R,
		MOUSE_BUTTON_M,
		MOUSE_AXIS_X,
		MOUSE_AXIS_Y,
		MOUSE_AXIS_Z

	};

public:

	CStdMouseInput();
	~CStdMouseInput() {}

//	HRESULT Init( HWND hWnd );
//	void Release();
//	void AcquireMouse();
//	void UnacquireMouse() { m_pDIMouse->Unacquire(); }

	inline void GetCurrentPosition( int& x, int& y ) { x = m_iPosX; y = m_iPosY; }

	void UpdateInput( UINT msg, WPARAM wParam, LPARAM lParam );

};

#endif		/*  __STDMOUSEINPUT_H__  */
