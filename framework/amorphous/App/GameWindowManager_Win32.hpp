#ifndef  __GameWindowManager_Win32_H__
#define  __GameWindowManager_Win32_H__


#include "GameWindowManager.hpp"
#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>


namespace amorphous
{


extern LRESULT (WINAPI *g_pMessageProcedureForGameWindow)( HWND, UINT, WPARAM, LPARAM );


class GameWindowManager_Win32 : public GameWindowManager
{
protected:

	HWND m_hWnd; /// Holds our window handle

	WNDCLASSEX m_WindowClassEx;

//	LRESULT WINAPI (*m_pMsgProc)( HWND, UINT, WPARAM, LPARAM );	// this function pointer cannot be used as a pointer to a message procedure


public:

	GameWindowManager_Win32();

	virtual ~GameWindowManager_Win32() {}

	virtual void Release();

//	bool CreateGameWindow( int iScreenWidth, int iScreenHeight, GameWindow::ScreenMode screen_mode, const std::string& app_title );

//	void ChangeScreenSize( int iNewScreenWidth, int iNewScreenHeight, bool bFullScreen );

	void SetWindowTitleText( const std::string& text );

	HWND GetWindowHandle() { return m_hWnd; }

	bool IsMouseCursorInClientArea();

	/// do nothing in full screen mode
	/// \param top-left corner of the window
	void SetWindowLeftTopCornerPosition( int left, int top );

	void MainLoop( ApplicationCore& app );
};


Result::Name SelectGraphicsLibrary_Win32( const std::string& graphics_library_name, GameWindowManager*& pGameWindowManager );

GameWindowManager_Win32& GetGameWindowManager_Win32();


} // namespace amorphous



#endif		/*  __GameWindowManager_Win32_H__  */
