#include "ApplicationBase.hpp"
#include "amorphous/Graphics/GraphicsResourceManager.hpp"
#include "amorphous/Support/lfs.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Input/Win32/StdKeyboard.hpp"
#include "amorphous/Input/Win32/StdMouseInput.hpp"

// Windows headers
#include <windows.h>
//#include <mmsystem.h>


// Visual Leak Detector
// - Works only if you run the App in 1)Debug Mode (default: F5) of 2)Debug build
// - Commented out: Xerces 3.0.1 (XML parser) crashes when the vld is used.
//#include <vld.h>

#include "GameWindowManager_Win32.hpp"


namespace amorphous
{

using namespace std;


#define APPBASE_TIMER_RESOLUTION	1


/*
class UserDefinedApp : public ApplicationBase
{
public:
};

extern ApplicationBase *CreateApplicationInstance() { return new CUserDefinedApp(); }

*/


// since we are using Direct Input, any pressing & releasing of keys will not be notified to this
// message procedure
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	static Win32StdKeyboard s_StdKeyboard;
	static Win32StdMouseInput s_StdMouse;

	if( ApplicationBase::GetInstance()->UseDefaultMouse() )
		s_StdMouse.UpdateInput( msg, wParam, lParam );

    switch( msg )
    {
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
				
        case WM_ACTIVATE:
            if( WA_INACTIVE != wParam )
            {
                // Make sure the device is acquired, if we are gaining focus.
				if( ApplicationBase::GetInstance() )
					ApplicationBase::GetInstance()->AcquireInputDevices();
            }
            break;

		case WM_KEYDOWN:
			if( ApplicationBase::GetInstance()->UseDefaultKeyboard() )
				s_StdKeyboard.NotifyKeyDown( (int)wParam );
			break;

		case WM_KEYUP:
			if( ApplicationBase::GetInstance()->UseDefaultKeyboard() )
				s_StdKeyboard.NotifyKeyUp( (int)wParam );
			break;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


string GetExeFilepath()
{
	char exe_filepath[512];
	memset( exe_filepath, 0, sizeof(exe_filepath) );
	GetModuleFileName( NULL, exe_filepath, sizeof(exe_filepath)-1 );
	return string(exe_filepath);
}


static void SetCommandLineArguments( LPSTR lpCmdLine )
{
	string cmd_line(lpCmdLine);
	if( 2 <= cmd_line.length() )
	{
		// Remove the first and last double quotes
		if( cmd_line[0] == '"' )
			cmd_line = cmd_line.substr( 1 );

		if( cmd_line[ cmd_line.length()-1 ] == '"' )
			cmd_line = cmd_line.substr( 0, cmd_line.length()-1 );
	}

	ApplicationBase::ms_CommandLineArguments.resize( 1 );
	ApplicationBase::ms_CommandLineArguments[0] = cmd_line;
}

} // namespace amorphous


// This function executes code specific to Windows platform,
// except the call of amorphous::StartApp().
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, INT )
{
	// set the message procedure for the game window
	amorphous::g_pMessageProcedureForGameWindow = amorphous::MsgProc;

	// timer resolution for timeGetTime()
	timeBeginPeriod( APPBASE_TIMER_RESOLUTION );

	// set the message procedure for the game window
	amorphous::g_pMessageProcedureForGameWindow = amorphous::MsgProc;

	amorphous::lfs::set_wd( amorphous::lfs::get_parent_path(amorphous::GetExeFilepath()) );

	amorphous::SetCommandLineArguments( lpCmdLine );

	ApplicationBase::StartApp();

	timeEndPeriod( APPBASE_TIMER_RESOLUTION );

	return 0;
}



#if 0

AppMain_Gen.cpp


// in AppMain_Gen.cpp
void MainLoop( ApplicationBase *pApp )
{
	while( !g_QuitApp )
	{
		pApp->UpdateFrame();
	}
}


int main()
{
	StartApp();

	return 0;
}


#endif
