#ifndef  __GameWindowManager_Win32_D3D_H__
#define  __GameWindowManager_Win32_D3D_H__


#include "GameWindowManager_Win32.hpp"


extern LRESULT (WINAPI *g_pMessageProcedureForGameWindow)( HWND, UINT, WPARAM, LPARAM );


class CGameWindowManager_Win32_D3D : public CGameWindowManager_Win32
{
private:

//	LRESULT WINAPI (*m_pMsgProc)( HWND, UINT, WPARAM, LPARAM );	// this function pointer cannot be used as a pointer to a message procedure


protected:

	CGameWindowManager_Win32_D3D();		//singleton

public:

	static CGameWindowManager_Win32_D3D ms_SingletonInstance_;	//single instance of 'CGameWindowManager_Win32'

	~CGameWindowManager_Win32_D3D();

	bool CreateGameWindow( int iScreenWidth, int iScreenHeight, GameWindow::ScreenMode screen_mode, const std::string& app_title );

	void ChangeScreenSize( int iNewScreenWidth, int iNewScreenHeight, bool bFullScreen );
};


inline CGameWindowManager_Win32_D3D& GameWindowManager_Win32_D3D()
{
	return CGameWindowManager_Win32_D3D::ms_SingletonInstance_;
}


#endif		/*  __GameWindowManager_Win32_D3D_H__  */
