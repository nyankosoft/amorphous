#ifndef  __GameWindowManager_Win32_GL_H__
#define  __GameWindowManager_Win32_GL_H__


#include "GameWindowManager.hpp"
#include <windows.h>		// Header File For Windows
#include <mmsystem.h>
#include <gl/gl.h>			// Header File For The OpenGL32 Library
#include <gl/glu.h>			// Header File For The GLu32 Library
#include "../base.hpp"


extern LRESULT (WINAPI *g_pMessageProcedureForGameWindow)( HWND, UINT, WPARAM, LPARAM );


class CGameWindowManager_Win32_GL : public CGameWindowManager
{
private:

	WNDCLASSEX m_WindowClassEx;

	HDC			m_hDC;    /// Private GDI Device Context
	HGLRC		m_hRC;    /// Permanent Rendering Context
	HWND		m_hWnd;   /// Holds Our Window Handle
	HINSTANCE	m_hInstance;   /// Holds The Instance Of The Application

	std::string m_ClassName;

//	LRESULT WINAPI (*m_pMsgProc)( HWND, UINT, WPARAM, LPARAM );	// this function pointer cannot be used as a pointer to a message procedure

private:

	Result::Name KillGLWindow();

protected:

	CGameWindowManager_Win32_GL();		//singleton

public:

	static CGameWindowManager_Win32_GL ms_SingletonInstance_;	//single instance of 'CGameWindowManager_Win32_GL'

	~CGameWindowManager_Win32_GL();

	bool CreateGameWindow( int iScreenWidth, int iScreenHeight, GameWindow::ScreenMode screen_mode, const std::string& app_title );

	void ChangeScreenSize( int iNewScreenWidth, int iNewScreenHeight, bool bFullScreen );

	HWND GetWindowHandle() { return m_hWnd; }

	bool IsMouseCursorInClientArea();

	/// do nothing in full screen mode
	/// \param top-left corner of the window
	void SetWindowLeftTopCornerPosition( int left, int top );
};


inline CGameWindowManager_Win32_GL& GameWindowManager_Win32_GL()
{
	return CGameWindowManager_Win32_GL::ms_SingletonInstance_;
}


inline CGameWindowManager& GameWindowManager()
{
	return GameWindowManager_Win32_GL();
}


#endif		/*  __GameWindowManager_Win32_GL_H__  */
