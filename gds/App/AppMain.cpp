
#include "ApplicationBase.h"

#include <windows.h>
#include <mmsystem.h>

#include <vld.h>

#include "GameWindowManager_Win32.h"


/*
class CUserDefinedApp : public CApplicationBase
{
public:
};

extern CApplicationBase *CreateApplicationInstance() { return new CUserDefinedApp(); }

*/


extern CApplicationBase *CreateApplicationInstance();



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


INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	// set the message procedure for the game window
	g_pMessageProcedureForGameWindow = MsgProc;

	StartApp();

	return 0;
}



#if 0

AppMain_X.cpp

int main()
{
	StartApp();

	return 0;
}


#endif