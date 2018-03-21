#include "GameWindowManager_Generic.hpp"
#include "amorphous/Graphics/OpenGL/GLGraphicsDevice.hpp"
#include "amorphous/Graphics/OpenGL/GLInitialization.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
//#include "amorphous/Support/WindowMisc_Win32.hpp"
#include "amorphous/Input/InputHandler.hpp"


namespace amorphous
{


GameWindowManager_Generic GameWindowManager_Generic::ms_SingletonInstance_;


GameWindowManager_Generic& GetGameWindowManager_Generic()
{
	return GameWindowManager_Generic::GetSingletonInstance();
}

Result::Name SelectGraphicsLibrary_Generic( const std::string& graphics_library_name, GameWindowManager*& pGameWindowManager )
{
	if( graphics_library_name == "OpenGL" )
	{
		InitializeOpenGLClasses();
	}
	else
	{
		LOG_PRINT_ERROR( "Unsupported graphics library: " + graphics_library_name );

		return Result::INVALID_ARGS;
	}

	pGameWindowManager = &GetGameWindowManager_Generic();

	return Result::SUCCESS;
}

bool GameWindowManager_Generic::CreateGameWindow(
    int iScreenWidth,
    int iScreenHeight,
    GameWindow::ScreenMode screen_mode,
    const std::string& app_title
    )
{
	// Will it take effect if we run 'export MESA_GL_VERSION_OVERRIDE=4.5' from here
	system("export MESA_GL_VERSION_OVERRIDE=4.5");

#ifdef BUILD_WITH_X11_LIBS
   
	LOG_PRINT("Creating an X11 window.");

    m_pX11GLWindow.reset( new X11_GLWindow );

    m_pX11GLWindow->Init( iScreenWidth, iScreenHeight, app_title.c_str() );

#endif // BUILD_WITH_X11_LIBS

	if( !GLGraphicsDevice().Init( iScreenWidth, iScreenHeight, (screen_mode == GameWindow::ScreenMode::WINDOWED) ? ScreenMode::WINDOWED : ScreenMode::FULLSCREEN ) )
	{
		LOG_PRINT_ERROR( "GLGraphicsDevice::Init() failed." );
		return false;								// Return FALSE
	}

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		LOG_PRINTF_ERROR(( "glewInit error: %s", glewGetErrorString(err)));
		return false;
	}

	return Init( iScreenWidth, iScreenHeight, screen_mode );
}


void GameWindowManager_Generic::MainLoop( ApplicationCore& app )
{
#ifdef BUILD_WITH_X11_LIBS

	if(m_pX11GLWindow)
	{
		m_pX11GLWindow->MainLoop(app);
	}

#endif // BUILD_WITH_X11_LIBS
}

#ifdef BUILD_WITH_X11_LIBS
void GetX11KeyMaps(std::map<unsigned int,int>& x11_to_gic)
{
	x11_to_gic[24] = 'Q';
	x11_to_gic[25] = 'W';
	x11_to_gic[26] = 'E';
	x11_to_gic[27] = 'R';
	x11_to_gic[28] = 'T';
	x11_to_gic[29] = 'Y';
	x11_to_gic[30] = 'U';
	x11_to_gic[31] = 'I';
	x11_to_gic[31] = 'Q';
	x11_to_gic[32] = 'P';
//	x11_to_gic[33] = '';
//	x11_to_gic[34] = '';
//	x11_to_gic[35] = '';
//	x11_to_gic[36] = '';
//	x11_to_gic[37] = '';
	x11_to_gic[38] = 'A';
	x11_to_gic[39] = 'S';
	x11_to_gic[40] = 'D';
	x11_to_gic[41] = 'F';
	x11_to_gic[41] = 'G';
	x11_to_gic[42] = 'H';
	x11_to_gic[43] = 'J';
	x11_to_gic[44] = 'K';
	x11_to_gic[45] = 'L';
//	x11_to_gic[46] = '';
//	x11_to_gic[47] = '';
//	x11_to_gic[48] = '';
//	x11_to_gic[49] = '';
//	x11_to_gic[50] = '';
//	x11_to_gic[51] = '';
	x11_to_gic[52] = 'Z';
	x11_to_gic[53] = 'X';
	x11_to_gic[54] = 'C';
	x11_to_gic[55] = 'V';
	x11_to_gic[56] = 'B';
	x11_to_gic[57] = 'N';
	x11_to_gic[58] = 'M';
//	x11_to_gic[59] = '';
	x11_to_gic[110] = GIC_HOME;
	x11_to_gic[111] = GIC_UP;
	x11_to_gic[112] = GIC_PAGE_UP;
	x11_to_gic[113] = GIC_LEFT;
	x11_to_gic[114] = GIC_RIGHT;
	x11_to_gic[115] = GIC_END;
	x11_to_gic[116] = GIC_DOWN;
	x11_to_gic[117] = GIC_PAGE_DOWN;
}
#endif // BUILD_WITH_X11_LIBS

void GetKeyMaps(std::map<unsigned int,int>& keymaps)
{
#ifdef BUILD_WITH_X11_LIBS
	GetX11KeyMaps(keymaps);
#endif // BUILD_WITH_X11_LIBS
}

void GameWindowManager_Generic::SetWindowLeftTopCornerPosition( int left, int top )
{
	LOG_PRINT_ERROR("Not implemented.");
}

} // namespace amorphous
