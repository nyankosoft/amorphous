#ifndef  __GameWindowManager_Win32_H__
#define  __GameWindowManager_Win32_H__


#include "GameWindowManager.hpp"
#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>


extern LRESULT (WINAPI *g_pMessageProcedureForGameWindow)( HWND, UINT, WPARAM, LPARAM );


class CGameWindowManager_Win32 : public CGameWindowManager
{
protected:

	HWND m_hWnd; /// Holds our window handle

	WNDCLASSEX m_WindowClassEx;

//	LRESULT WINAPI (*m_pMsgProc)( HWND, UINT, WPARAM, LPARAM );	// this function pointer cannot be used as a pointer to a message procedure


public:

	CGameWindowManager_Win32();

	virtual ~CGameWindowManager_Win32() {}

//	bool CreateGameWindow( int iScreenWidth, int iScreenHeight, GameWindow::ScreenMode screen_mode, const std::string& app_title );

//	void ChangeScreenSize( int iNewScreenWidth, int iNewScreenHeight, bool bFullScreen );

	HWND GetWindowHandle() { return m_hWnd; }

	virtual void OnMainLoopFinished() {}

	bool IsMouseCursorInClientArea();

	/// do nothing in full screen mode
	/// \param top-left corner of the window
	void SetWindowLeftTopCornerPosition( int left, int top );
};


extern CGameWindowManager_Win32 *g_pGameWindowManager_Win32;


inline CGameWindowManager_Win32& GameWindowManager_Win32()
{
//	return CGameWindowManager_Win32::ms_SingletonInstance_;
	return *g_pGameWindowManager_Win32;
}


#endif		/*  __GameWindowManager_Win32_H__  */
