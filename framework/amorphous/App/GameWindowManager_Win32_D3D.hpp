#ifndef  __GameWindowManager_Win32_D3D_H__
#define  __GameWindowManager_Win32_D3D_H__


#include "GameWindowManager_Win32.hpp"


namespace amorphous
{


extern LRESULT (WINAPI *g_pMessageProcedureForGameWindow)( HWND, UINT, WPARAM, LPARAM );


class GameWindowManager_Win32_D3D : public GameWindowManager_Win32
{
private:

//	LRESULT WINAPI (*m_pMsgProc)( HWND, UINT, WPARAM, LPARAM );	// this function pointer cannot be used as a pointer to a message procedure


protected:

	GameWindowManager_Win32_D3D();		//singleton

public:

	static GameWindowManager_Win32_D3D ms_SingletonInstance_;	//single instance of 'GameWindowManager_Win32'

	~GameWindowManager_Win32_D3D();

	void Release();

	bool CreateGameWindow( int iScreenWidth, int iScreenHeight, GameWindow::ScreenMode screen_mode, const std::string& app_title );

	void ChangeScreenSize( int iNewScreenWidth, int iNewScreenHeight, bool bFullScreen );
};


inline GameWindowManager_Win32_D3D& GetGameWindowManager_Win32_D3D()
{
	return GameWindowManager_Win32_D3D::ms_SingletonInstance_;
}

} // namespace amorphous



#endif		/*  __GameWindowManager_Win32_D3D_H__  */
