//-----------------------------------------------------------------------------
// File: D3DAppTestMain.cpp
//-----------------------------------------------------------------------------

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include <gds/Graphics.hpp>
#include <gds/Graphics/Font/BitstreamVeraSansMono_Bold_256.hpp>
#include <gds/Support/MiscAux.hpp>
#include <gds/Support/WindowMisc_Win32.hpp>
#include <gds/Support/Timer.hpp>
#include <gds/Support/ParamLoader.hpp>
#include <gds/Support.hpp>
#include <gds/Input.hpp>
#include <gds/Input/StdMouseInput.hpp>
#include <gds/XML.hpp>
#include <gds/App/GameWindowManager_Win32.hpp>
//#include <gl/gl.h>

#include "GraphicsTestBase.hpp"

//#include <vld.h> // Visual Leak Detector - available only in the debugging mode of the debug build

using namespace std;
using namespace boost;


extern CGraphicsTestBase *CreateTestInstance();
extern const std::string GetAppTitle();

// draft
extern void SetCurrentThreadAsRenderThread();


// global variable
std::string g_CmdLine;


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

CPlatformDependentCameraController g_CameraController;

CCamera g_Camera;

float g_FOV = (float)PI / 4.0f;

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
// Name: Update( float frametime )
// Desc: 
//-----------------------------------------------------------------------------

void UpdateCameraMatrices()
{
	D3DXMATRIX matView;
	g_CameraController.GetCameraMatrix( matView );
//	DIRECT3D9.GetDevice()->SetTransform( D3DTS_VIEW, &matView );

	Matrix44 view;
	view.SetRowMajorMatrix44( (Scalar *)&matView );
	FixedFunctionPipelineManager().SetViewTransform( view );
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	PROFILE_FUNCTION();

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );

//	g_Camera.SetPose( g_CameraController.GetPose() );
	ShaderManagerHub.PushViewAndProjectionMatrices( g_Camera );
/*
	D3DXMATRIX matView, matProj;
	g_Camera.GetCameraMatrix( matView );
	g_Camera.GetProjectionMatrix( matProj );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
*/
	g_pTest->UpdateViewTransform( g_Camera.GetCameraMatrix() );
	g_pTest->UpdateProjectionTransform( g_Camera.GetProjectionMatrix() );

	GraphicsDevice().SetRenderState( RenderStateType::ALPHA_BLEND, true );
	GraphicsDevice().SetSourceBlendMode( AlphaBlend::SrcAlpha );
	GraphicsDevice().SetDestBlendMode( AlphaBlend::InvSrcAlpha );

    // clear the backbuffer to a blue color
//	GraphicsDevice().SetClearColor( g_pTest->GetBackgroundColor() );
//	GraphicsDevice().SetClearDepth( 1.0f );
//	GraphicsDevice().Clear()
	if( pd3dDevice )
	{
		pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, g_pTest->GetBackgroundColor().GetARGB32(), 1.0f, 0 );
	}
	else
	{
		const SFloatRGBAColor c = g_pTest->GetBackgroundColor();
		glClearColor( c.fRed, c.fGreen, c.fBlue, c.fAlpha );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	}

	g_pTest->RenderScene();

    // begin the scene
	if( pd3dDevice )
		pd3dDevice->BeginScene();

	g_pTest->Render();

	// display fps
	g_pFont->DrawText( to_string(GlobalTimer().GetFPS()).c_str(), D3DXVECTOR2(20,20), 0xFFFFFFFF );

	int i=0;
	const vector<string>& vecProfileResults = GetProfileText();
	BOOST_FOREACH( const string& text, vecProfileResults )
	{
//		g_pFont->DrawText( text.c_str(), Vector2( 20, 40 + i*16 ), 0xF0F0F0FF );
		i++;
	}

	if( pd3dDevice )
	{
		// end the scene
		pd3dDevice->EndScene();

		// present the backbuffer contents to the display
		pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	g_CameraController.HandleMessage( msg, wParam, lParam );

	static CStdMouseInput s_Mouse;

	s_Mouse.UpdateInput( msg, wParam, lParam );

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

    case WM_ACTIVATE:
        if( WA_INACTIVE != wParam )
        {
            // Make sure the device is acquired, if we are gaining focus.
			if( g_pTest )
				g_pTest->AcquireInputDevices();
        }
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

	InputHub().SetInputHandler( g_pInputHandler.get() );

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

	g_CmdLine = lpCmdLine;

	const string iwd = fnop::get_cwd(); // initial working directory
	if( iwd.substr( iwd.length() - 4 ) != "/_App"
	 && iwd.substr( iwd.length() - 4 ) != "\\_App" )
	{
		// working directory is not set to the directory which contains the application binary
//		chdir( "../../../_App" );
		chdir( "../app" );
	}

	// draft - register the render thread id
	SetCurrentThreadAsRenderThread();

	g_pMessageProcedureForGameWindow = MsgProc;

/*
	// init xml parser
	// - released when leaving WinMain()
//	CXMLParserInitReleaseManager parser_mgr;
*/

	string graphics_library_name = "Direct3D";
	CParamLoader param_loader( "config" );
	if( param_loader.IsReady() )
	{
		param_loader.LoadParam( "GraphicsLibrary", graphics_library_name );
		param_loader.CloseFile();
	}

	Result::Name res = SelectGraphicsLibrary( graphics_library_name );
	if( res != Result::SUCCESS )
		return 0;

	CLogOutput_HTML html_log( GetAppTitle() + "_" + string(GetBuildInfo()) + "_Log.html" );
	g_Log.AddLogOutput( &html_log );

	// Create the instance of the test class
	g_pTest = boost::shared_ptr<CGraphicsTestBase>( CreateTestInstance() );

	int w = g_pTest->GetWindowWidth();  // 1024;
	int h = g_pTest->GetWindowHeight(); //  768;
	GameWindow::ScreenMode mode = GameWindow::WINDOWED;//g_pTest->GetFullscreen() ? GameWindow::FULLSCREEN : GameWindow::WINDOWED;
	GameWindowManager().CreateGameWindow( w, h, mode, app_title );

	try
	{
		if( !Init() )
			return 0;
	}
	catch( std::exception& e )
	{
		g_Log.Print( WL_WARNING, "exception: %s", e.what() );
	}


//	ChangeClientAreaSize( hWnd, g_pTest->GetWindowWidth(), g_pTest->GetWindowHeight() );

	ProfileInit();

	// init font
/*	string font_name
//		= "Lucida Console";
		= "DotumChe";
	g_pFont = pTexFontCFontSharedPtr( new CFont( font_name, 8, 16 ) );*/
	shared_ptr<CTextureFont> pTexFont( new CTextureFont );
	pTexFont->InitFont( g_BitstreamVeraSansMono_Bold_256 );
	pTexFont->SetFontSize( 8, 16 );
	g_pFont = pTexFont;

	// Enter the message loop
	MSG msg;
	ZeroMemory( &msg, sizeof(msg) );

	GlobalTimer().Start();

	while( msg.message!=WM_QUIT )
	{
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
		{
			GlobalTimer().UpdateFrameTime();

			g_CameraController.UpdateCameraPose( GlobalTimer().GetFrameTime() );

			UpdateCameraMatrices();

			float frametime = GlobalTimer().GetFrameTime();
			if( 0.1f < frametime )
				frametime = 0.1f;

			Update( frametime );

			g_Camera.SetPose( g_CameraController.GetPose() );

			if( g_pTest->UseRenderBase() )
				g_pTest->RenderBase();
			else
				Render();

			ProfileDumpOutputToBuffer();

			GameWindowManager_Win32().OnMainLoopFinished();

			Sleep( 5 );
		}
    }

	g_pTest.reset();

	ReleaseGraphicsResources();

    // Clean up everything and exit the app
//	UnregisterClass( app_class_name.c_str(), wc.hInstance );
    return 0;
}
