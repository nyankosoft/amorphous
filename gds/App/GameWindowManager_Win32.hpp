#ifndef  __GameWindowManager_Win32_H__
#define  __GameWindowManager_Win32_H__


#include "GameWindowManager.hpp"
#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>


extern LRESULT (WINAPI *g_pMessageProcedureForGameWindow)( HWND, UINT, WPARAM, LPARAM );


class CGameWindowManager_Win32 : public CGameWindowManager
{
private:

	HWND m_hWnd;
	WNDCLASSEX m_WindowClassEx;

//	LRESULT WINAPI (*m_pMsgProc)( HWND, UINT, WPARAM, LPARAM );	// this function pointer cannot be used as a pointer to a message procedure


protected:

	CGameWindowManager_Win32();		//singleton

public:

	static CGameWindowManager_Win32 ms_SingletonInstance_;	//single instance of 'CGameWindowManager_Win32'

	~CGameWindowManager_Win32();

	bool CreateGameWindow( int iScreenWidth, int iScreenHeight, GameWindow::ScreenMode screen_mode, const std::string& app_title );

	void ChangeScreenSize( int iNewScreenWidth, int iNewScreenHeight, bool bFullScreen );

	HWND GetWindowHandle() { return m_hWnd; }

	bool IsMouseCursorInClientArea();

	/// do nothing in full screen mode
	/// \param top-left corner of the window
	void SetWindowLeftTopCornerPosition( int left, int top );
};


inline CGameWindowManager_Win32& GameWindowManager_Win32()
{
	return CGameWindowManager_Win32::ms_SingletonInstance_;
}


inline CGameWindowManager& GameWindowManager()
{
	return GameWindowManager_Win32();
}


#endif		/*  __GameWindowManager_Win32_H__  */
