#ifndef  __GameWindowManager_HPP__
#define  __GameWindowManager_HPP__

#include <string>
#include "gds/base.hpp"


namespace amorphous
{


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


class GameWindowManager
{
protected:

	std::string m_ApplicationClassName;

	int m_iCurrentScreenWidth;
	int m_iCurrentScreenHeight;
	GameWindow::ScreenMode m_CurrentScreenMode;

public:

	GameWindowManager() {}

	virtual ~GameWindowManager() {}

	virtual bool CreateGameWindow( int iScreenWidth, int iScreenHeight, GameWindow::ScreenMode screen_mode = GameWindow::WINDOWED, const std::string& app_title = "Application" ) = 0;

	virtual void ChangeScreenSize( int iNewScreenWidth, int iNewScreenHeight, bool bFullScreen ) {}

	virtual void SetWindowTitleText( const std::string& text ) {}

	inline int GetScreenWidth()  { return m_iCurrentScreenWidth; }
	inline int GetScreenHeight() { return m_iCurrentScreenHeight; }

	inline bool IsFullscreen()   { return m_CurrentScreenMode == GameWindow::FULLSCREEN ? true : false; }

	virtual void SetWindowLeftTopCornerPosition( int left, int top ) {}

	virtual bool IsMouseCursorInClientArea() { return false; }

	virtual void OnMainLoopFinished() {}
};


Result::Name SelectGraphicsLibrary( const std::string& graphics_library_name );

GameWindowManager& GetGameWindowManager();


} // namespace amorphous



#endif // __GameWindowManager_HPP__
