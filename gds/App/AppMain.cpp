#include "ApplicationBase.hpp"
#include "Support/fnop.hpp"
#include "Support/Profile.hpp"

// Windows headers
#include <windows.h>
#include <mmsystem.h>

// Visual Leak Detector
// - Works only if you run the App in 1)Debug Mode (default: F5) of 2)Debug build
// - Commented out: Xerces 3.0.1 (XML parser) crashes when the vld is used.
//#include <vld.h>

#include "GameWindowManager_Win32.hpp"

using namespace std;


// draft
extern void SetCurrentThreadAsRenderThread();


#define APPBASE_TIMER_RESOLUTION	1


/*
class CUserDefinedApp : public CApplicationBase
{
public:
};

extern CApplicationBase *CreateApplicationInstance() { return new CUserDefinedApp(); }

*/


extern CApplicationBase *CreateApplicationInstance();


void MainLoop( CApplicationBase *pApp )
{
    // Enter the message loop
    MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    while( msg.message!=WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
			CScopeProfile sp( "Windows message translation and dispatch" );

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
		{
			pApp->UpdateFrame();
		}
	}
}



// since we are using Direct Input, any pressing & releasing of keys will not be notified to this
// message procedure
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
				
        case WM_ACTIVATE:
            if( WA_INACTIVE != wParam )
            {
                // Make sure the device is acquired, if we are gaining focus.
				if( g_pAppBase )
					g_pAppBase->AcquireInputDevices();
            }
            break;

		case WM_KEYDOWN:
			break;

		case WM_KEYUP:
			break;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


void StartApp()
{
	// Create the instance of application implemented by the user
	g_pAppBase = CreateApplicationInstance();

	g_pAppBase->Run();

	delete g_pAppBase;
	g_pAppBase = NULL;
}


string GetExeFilepath()
{
	char exe_filepath[512];
	memset( exe_filepath, 0, sizeof(exe_filepath) );
	GetModuleFileName( NULL, exe_filepath, sizeof(exe_filepath)-1 );
	return string(exe_filepath);
}


INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, INT )
{
	SetCurrentThreadAsRenderThread();

	// timer resolution for timeGetTime()
	timeBeginPeriod( APPBASE_TIMER_RESOLUTION );

	// set the message procedure for the game window
	g_pMessageProcedureForGameWindow = MsgProc;

	fnop::set_wd( fnop::get_path(GetExeFilepath()) );

	StartApp();

	timeEndPeriod( APPBASE_TIMER_RESOLUTION );

	return 0;
}



#if 0

AppMain_Gen.cpp


// in AppMain_Gen.cpp
void MainLoop( CApplicationBase *pApp )
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