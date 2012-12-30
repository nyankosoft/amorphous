#ifndef  __STDMOUSEINPUT_H__
#define  __STDMOUSEINPUT_H__

#include "MouseInputDevice.hpp"
#include <windows.h>


namespace amorphous
{


class CStdMouseInput : public MouseInputDevice
{
	int m_iPosX, m_iPosY;	// current mouse position

public:

	CStdMouseInput();
	~CStdMouseInput() {}

	Result::Name SendBufferedInputToInputHandlers();

//	HRESULT Init( HWND hWnd );
//	void Release();
//	void AcquireMouse();
//	void UnacquireMouse() { m_pDIMouse->Unacquire(); }

	inline void GetCurrentPosition( int& x, int& y ) { x = m_iPosX; y = m_iPosY; }

	void UpdateInput( UINT msg, WPARAM wParam, LPARAM lParam );

};
} // namespace amorphous



#endif		/*  __STDMOUSEINPUT_H__  */
