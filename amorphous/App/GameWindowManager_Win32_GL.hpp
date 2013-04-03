#ifndef  __GameWindowManager_Win32_GL_H__
#define  __GameWindowManager_Win32_GL_H__


#include "GameWindowManager_Win32.hpp"
#include "../base.hpp"


namespace amorphous
{


extern LRESULT (WINAPI *g_pMessageProcedureForGameWindow)( HWND, UINT, WPARAM, LPARAM );


class GameWindowManager_Win32_GL : public GameWindowManager_Win32
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

	GameWindowManager_Win32_GL();		//singleton

public:

	static GameWindowManager_Win32_GL ms_SingletonInstance_;	//single instance of 'GameWindowManager_Win32_GL'

	~GameWindowManager_Win32_GL();

	bool CreateGameWindow( int iScreenWidth, int iScreenHeight, GameWindow::ScreenMode screen_mode, const std::string& app_title );

	void ChangeScreenSize( int iNewScreenWidth, int iNewScreenHeight, bool bFullScreen );

	void OnMainLoopFinished();
};


inline GameWindowManager_Win32_GL& GetGameWindowManager_Win32_GL()
{
	return GameWindowManager_Win32_GL::ms_SingletonInstance_;
}

} // namespace amorphous



#endif		/*  __GameWindowManager_Win32_GL_H__  */
