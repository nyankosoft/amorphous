#ifndef  __amorphous_GameWindowManager_Generic_HPP__
#define  __amorphous_GameWindowManager_Generic_HPP__


#include "GameWindowManager.hpp"
#include <memory>

#ifdef BUILD_WITH_X11_LIBS
#include "X11_GLWindow.hpp"
#endif // BUILD_WITH_X11_LIBS


namespace amorphous
{


class GameWindowManager_Generic : public GameWindowManager
{
private:

	static GameWindowManager_Generic ms_SingletonInstance_;

	GameWindowManager_Generic() {}

protected:

#ifdef BUILD_WITH_X11_LIBS
	std::unique_ptr<X11_GLWindow> m_pX11GLWindow;
#endif // BUILD_WITH_X11_LIBS

public:

	virtual ~GameWindowManager_Generic() {}

	void Release();

	bool CreateGameWindow( int iScreenWidth, int iScreenHeight, GameWindow::ScreenMode screen_mode, const std::string& app_title );

	void MainLoop( ApplicationCore& app );

//	void ChangeScreenSize( int iNewScreenWidth, int iNewScreenHeight, bool bFullScreen );

	//void SetWindowTitleText( const std::string& text );

	//bool IsMouseCursorInClientArea();

	/// do nothing in full screen mode
	/// \param top-left corner of the window
	void SetWindowLeftTopCornerPosition( int left, int top );

	static GameWindowManager_Generic& GetSingletonInstance() { return ms_SingletonInstance_; }
};


Result::Name SelectGraphicsLibrary_Generic( const std::string& graphics_library_name, GameWindowManager*& pGameWindowManager );

GameWindowManager_Generic& GetGameWindowManager_Generic();


} // namespace amorphous


#endif		/*  __amorphous_GameWindowManager_Generic_HPP__  */
