#ifndef  __GameWindowManager_HPP__
#define  __GameWindowManager_HPP__

#include <string>
#include <stdio.h>
#include <stdlib.h>


class GameWindow
{
public:
	enum ScreenMode
	{
		WINDOWED,
		FULLSCREEN,
		NUM_SCREEN_MODES,
	};
};


class CGameWindowManager
{
protected:

	std::string m_ApplicationClassName;

	int m_iCurrentScreenWidth;
	int m_iCurrentScreenHeight;
	GameWindow::ScreenMode m_CurrentScreenMode;

public:

	CGameWindowManager() {}

	virtual ~CGameWindowManager() {}

	virtual bool CreateGameWindow( int iScreenWidth, int iScreenHeight, GameWindow::ScreenMode screen_mode = GameWindow::WINDOWED, const std::string& app_title = "Application" ) = 0;

	virtual void ChangeScreenSize( int iNewScreenWidth, int iNewScreenHeight, bool bFullScreen ) {}

	inline int GetScreenWidth()  { return m_iCurrentScreenWidth; }
	inline int GetScreenHeight() { return m_iCurrentScreenHeight; }

	inline bool IsFullscreen()   { return m_CurrentScreenMode == GameWindow::FULLSCREEN ? true : false; }

	virtual void SetWindowLeftTopCornerPosition( int left, int top ) {}

	virtual bool IsMouseCursorInClientArea() { return false; }
};



#endif // __GameWindowManager_HPP__
