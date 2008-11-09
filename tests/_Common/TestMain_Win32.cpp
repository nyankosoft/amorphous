//-----------------------------------------------------------------------------
// File: D3DAppTestMain.cpp
//-----------------------------------------------------------------------------

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include "3DCommon/all.h"
#include "GameCommon/Timer.h"
#include "Support.h"
#include "GameInput.h"

#include "GraphicsTestBase.h"
#include "GameInput.h"
#include "XML/XMLDocumentLoader.h"
#include "Support/MiscAux.h"

#include <vld.h> // Visual Leak Detector - available only in the debugging mode of the debug build


extern CGraphicsTestBase *CreateTestInstance();
extern const std::string GetAppTitle();


/**
  Using testing framework for graphics modules

  1. Create a class that inherits CGraphicsTestBase

  2. Define a function named "CreateTestInstance()" that creates an instance
     of the test class and returns its instance

	 template:
	 CGraphicsTestBase *CreateTestInstance()
	 {
		return new CYourGraphicsTestClass();
	 }

  3. Define a function "GetAppTitle()" that returns a string of application name

	 template:
     const std::string GetAppTitle() { return string("YourGraphicsTestApp"); }
*/


boost::shared_ptr<CGraphicsTestBase> g_pTest;

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

CStdKeyboard g_StdKeyboard;

boost::shared_ptr<CInputHandler> g_pInputHandler;

CFontSharedPtr g_pFont;

CCameraController_Win32 g_CameraController;

CCamera g_Camera;

float g_FOV = D3DX_PI / 4.0f;

const int g_WindowWidth  = 800;
const int g_WindowHeight = 600;


void ReleaseGraphicsResources()
{
	// Detach the GraphicsResourceManager(GRM) from the list of graphics components
	// managed by CGraphicsComponentCollector(GCC)
	// - Failure to do this may result in memory leak,
	//   and here is how it happens.
	//   1. CGraphicsComponentCollector (singleton) is released
	//      - the graphics components list is also released
	//   2. GRM is released
	//      - GRM tries to detach itself from the graphics components list,
	//        which has been already released in 1.
	//      - So it has to create the singleton instance of GCC at this step
	//      - The newly created GCC instance is never released and causes memory leak
	GraphicsResourceManager().ReleaseSingleton();
}


//-----------------------------------------------------------------------------
// Name: Update( float frametime )
// Desc: 
//-----------------------------------------------------------------------------
void Update( float frametime )
{
	PROFILE_FUNCTION();

	g_pTest->Update( frametime );
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	PROFILE_FUNCTION();

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

//	g_Camera.SetPose( g_CameraController.GetPose() );
	ShaderManagerHub.PushViewAndProjectionMatrices( g_Camera );

	D3DXMATRIX mat;
	g_Camera.GetCameraMatrix( mat );
	g_pTest->UpdateViewTransform( mat );
	g_Camera.GetProjectionMatrix( mat );
	g_pTest->UpdateProjectionTransform( mat );


    // clear the backbuffer to a blue color
    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, g_pTest->GetBackgroundColor().GetARGB32(), 1.0f, 0 );

	g_pTest->RenderScene();

    // begin the scene
    pd3dDevice->BeginScene();

	g_pTest->Render();

	// display fps
	g_pFont->DrawText( to_string(TIMER.GetFPS()).c_str(), D3DXVECTOR2(20,20), 0xFFFFFFFF );

	int i=0;
	const vector<string>& vecProfileResults = GetProfileText();
	BOOST_FOREACH( const string& text, vecProfileResults )
	{
		g_pFont->DrawText( text.c_str(), Vector2( 20, 40 + i*16 ), 0xF0F0F0FF );
		i++;
	}

    // end the scene
    pd3dDevice->EndScene();

    // present the backbuffer contents to the display
    pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	g_CameraController.HandleMessage( hWnd, msg, wParam, lParam );

	D3DXMATRIX matWorld, matProj;

	switch( msg )
	{
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;

	case WM_KEYDOWN:
		StdWin32Keyboard().NotifyKeyDown( (int)wParam );
		break;

	case WM_KEYUP:
		StdWin32Keyboard().NotifyKeyUp( (int)wParam );
		break;

	default:
		break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


bool Init()
{
	g_pTest->Init();

	g_pInputHandler = boost::shared_ptr<CInputHandler>( new CGraphicsTestInputHandler(g_pTest) );

	INPUTHUB.SetInputHandler( g_pInputHandler.get() );

	return true;
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------



INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, INT )
{
	const std::string app_title = GetAppTitle();
	const std::string app_class_name = app_title;

	const string iwd = fnop::get_cwd(); // initial working directory
	if( iwd.substr( iwd.length() - 4 ) != "/_App"
	 && iwd.substr( iwd.length() - 4 ) != "\\_App" )
	{
		// working directory is not set to the directory which contains the application binary
		chdir( "../../../_App" );
	}

    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      app_class_name.c_str(), NULL };

    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( app_class_name.c_str(), app_title.c_str(),
		                      WS_OVERLAPPEDWINDOW, 100, 100, g_WindowWidth, g_WindowHeight,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

	// fix client area size of the window
//	ChagneClientAreaSize( hWnd, g_WindowWidth, g_WindowHeight );

	// init xml parser
	// - released when leaving WinMain()
	CXMLParserInitReleaseManager parser_mgr;

    // Show the window
    ShowWindow( hWnd, SW_SHOWDEFAULT );
    UpdateWindow( hWnd );

	// Initialize Direct3D
	if( !DIRECT3D9.InitD3D( hWnd ) )
		return 0;

	CLogOutput_HTML html_log( GetAppTitle() + "_" + string(GetBuildInfo()) + "_Log.html" );
	g_Log.AddLogOutput( &html_log );

	// Create the instance of the test class
	g_pTest = boost::shared_ptr<CGraphicsTestBase>( CreateTestInstance() );

	if( !Init() )
		return 0;

	ChagneClientAreaSize( hWnd, g_pTest->GetWindowWidth(), g_pTest->GetWindowHeight() );

	ProfileInit();

	// init font
	g_pFont = CFontSharedPtr( new CFont( "Lucida Console", 8, 16 ) );

	// Enter the message loop
	MSG msg;
	ZeroMemory( &msg, sizeof(msg) );

	TIMER.Start();

	while( msg.message!=WM_QUIT )
	{
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
		{
			TIMER.UpdateFrameTime();

			g_CameraController.UpdateCameraPosition( TIMER.GetFrameTime() );
			g_CameraController.SetCameraMatrix();

			float frametime = TIMER.GetFrameTime();
			if( 0.1f < frametime )
				frametime = 0.1f;

			Update( frametime );

			g_Camera.SetPose( g_CameraController.GetPose() );

			if( g_pTest->UseRenderBase() )
				g_pTest->RenderBase();
			else
				Render();

			ProfileDumpOutputToBuffer();

			Sleep( 5 );
		}
    }

	g_pTest.reset();

	ReleaseGraphicsResources();

    // Clean up everything and exit the app
    UnregisterClass( app_class_name.c_str(), wc.hInstance );
    return 0;
}
