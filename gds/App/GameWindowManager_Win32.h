#ifndef  __GAMEWINDOWMANAGER_WIN32_H__
#define  __GAMEWINDOWMANAGER_WIN32_H__

#include <string>
#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>


#define GAMEWINDOWMANAGER ( CGameWindowManager_Win32::ms_SingletonInstance_ )


#define SMD_WINDOWED	0
#define SMD_FULLSCREEN	1

extern LRESULT (WINAPI *g_pMessageProcedureForGameWindow)( HWND, UINT, WPARAM, LPARAM );


class CGameWindowManager
{
public:

	CGameWindowManager() {}
	virtual ~CGameWindowManager() {}
};


class CGameWindowManager_Win32 : public CGameWindowManager
{
private:
	HWND m_hWnd;
	WNDCLASSEX m_WindowClassEx;

	int m_iCurrentScreenWidth;
	int m_iCurrentScreenHeight;
	int m_iCurrentScreenMode;

	std::string m_ApplicationClassName;

//	LRESULT WINAPI (*m_pMsgProc)( HWND, UINT, WPARAM, LPARAM );	// this function pointer cannot be used as a pointer to a message procedure


protected:

	CGameWindowManager_Win32();		//singleton

public:

	static CGameWindowManager_Win32 ms_SingletonInstance_;	//single instance of 'CGameWindowManager_Win32'

	~CGameWindowManager_Win32();

	bool CreateGameWindow( int iScreenWidth, int iScreenHeight, int screen_mode = SMD_WINDOWED, const std::string& app_title = "Application" );

	void ChangeScreenSize( int iNewScreenWidth, int iNewScreenHeight, bool bFullScreen );

	inline int GetScreenWidth() { return m_iCurrentScreenWidth; }
	inline int GetScreenHeight() { return m_iCurrentScreenHeight; }

	inline int IsFullscreen() { return m_iCurrentScreenMode == SMD_FULLSCREEN ? true : false; }

	HWND GetWindowHandle() { return m_hWnd; }

	bool IsMouseCursorInClientArea();

	/// do nothing in full screen mode
	/// \param top-left corner of the window
	void SetWindowLeftTopCornerPosition( int left, int top );
};


inline CGameWindowManager_Win32& GameWindowManager()
{
	return CGameWindowManager_Win32::ms_SingletonInstance_;
}


#endif		/*  __GAMEWINDOWMANAGER_WIN32_H__  */
