#ifndef  __GameWindowManager_Win32_GL_H__
#define  __GameWindowManager_Win32_GL_H__


#include "Graphics/GLGraphicsDevice.hpp"
#include "GameWindowManager_Win32.hpp"
#include <gl/gl.h>			// Header File For The OpenGL32 Library
#include <gl/glu.h>			// Header File For The GLu32 Library
#include "../base.hpp"


extern LRESULT (WINAPI *g_pMessageProcedureForGameWindow)( HWND, UINT, WPARAM, LPARAM );


class CGameWindowManager_Win32_GL : public CGameWindowManager_Win32
{
private:

	WNDCLASSEX m_WindowClassEx;

	HDC			m_hDC;    /// Private GDI Device Context
	HGLRC		m_hRC;    /// Permanent Rendering Context
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

	void OnMainLoopFinished();
};


inline CGameWindowManager_Win32_GL& GameWindowManager_Win32_GL()
{
	return CGameWindowManager_Win32_GL::ms_SingletonInstance_;
}


#endif		/*  __GameWindowManager_Win32_GL_H__  */
