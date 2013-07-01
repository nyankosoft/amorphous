//-----------------------------------------------------------------------------
// File: D3DAppTestMain.cpp
//-----------------------------------------------------------------------------

#include <boost/foreach.hpp>

#include "amorphous/Graphics.hpp"
#include "amorphous/Graphics/Font/BuiltinFonts.hpp"
#include "amorphous/Graphics/Direct3D/Direct3D9.hpp"
#include "amorphous/Support/MiscAux.hpp"
#include "amorphous/Support/WindowMisc_Win32.hpp"
#include "amorphous/Support/CameraController_Win32.hpp"
#include "amorphous/Support/Timer.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/BitmapImage.hpp" // For InitFreeImage()
#include "amorphous/Support/lfs.hpp"
#include "amorphous/Support/CameraController.hpp"
#include "amorphous/Input.hpp"
#include "amorphous/Input/StdMouseInput.hpp"
//#include "amorphous/XML.hpp"
#include "amorphous/App/GameWindowManager_Win32.hpp"
//#include <gl/gl.h>

#include "GraphicsTestBase.hpp"

//#include <vld.h> // Visual Leak Detector - available only in the debugging mode of the debug build

using namespace std;
using namespace boost;


extern CGraphicsTestBase *CreateTestInstance();


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


static boost::shared_ptr<CGraphicsTestBase> g_pTest;

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

static Win32StdKeyboard g_StdKeyboard;

static boost::shared_ptr<InputHandler> g_pInputHandler;

static boost::shared_ptr<FontBase> g_pFont;

Camera g_Camera;

//float g_FOV = (float)PI / 4.0f;

static const int sg_CameraControllerInputHandlerIndex = 0;
static const int sg_GraphicsTestInputHandlerIndex = 1;


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

	if( g_pTest )
	{
		// Update the pose stored in the camera controller
		g_pTest->UpdateCameraController( frametime );

		g_pTest->Update( frametime );
	}
}


//-----------------------------------------------------------------------------
// Name: Update( float frametime )
// Desc: 
//-----------------------------------------------------------------------------

void UpdateCameraMatrices()
{
	Matrix44 matView;
	g_Camera.GetCameraMatrix( matView );

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
	GetShaderManagerHub().PushViewAndProjectionMatrices( g_Camera );

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
		PROFILE_SCOPE( "Clear the color and depth buffers" );
		const SFloatRGBAColor c = g_pTest->GetBackgroundColor();
		glClearColor( c.red, c.green, c.blue, c.alpha );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	}

	g_pTest->RenderScene();

    // begin the scene
	if( pd3dDevice )
		pd3dDevice->BeginScene();

	g_pTest->Render();

	GraphicsDevice().Enable(  RenderStateType::ALPHA_BLEND );
	GraphicsDevice().Disable( RenderStateType::LIGHTING );

	// display fps
	g_pFont->DrawText( to_string(GlobalTimer().GetFPS()).c_str(), Vector2(20,20), 0xFFFFFFFF );

	int i=0;
	const vector<string>& vecProfileResults = GetProfileText();
	BOOST_FOREACH( const string& text, vecProfileResults )
	{
		g_pFont->DrawText( text.c_str(), Vector2( 20, 40 + i*16 ), 0xF0F0F0FF );
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
	static Win32StdMouseInput s_Mouse;

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

	g_pInputHandler.reset( new CGraphicsTestInputHandler(g_pTest) );

	GetInputHub().SetInputHandler( sg_GraphicsTestInputHandlerIndex, g_pInputHandler.get() );

//	g_pCameraController.reset( new CameraController( sg_CameraControllerInputHandlerIndex ) );

	return true;
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, INT )
{
	g_CmdLine = lpCmdLine;

	const string iwd = lfs::get_cwd(); // initial working directory
	if( iwd.substr( iwd.length() - 4 ) != "/app"
	 && iwd.substr( iwd.length() - 4 ) != "\\app" )
	{
		// working directory is not set to the directory which contains the application binary
//		chdir( "../../../_App" );
		chdir( "../../app" );
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
	ParamLoader param_loader( "config" );
	if( param_loader.IsReady() )
	{
		param_loader.LoadParam( "GraphicsLibrary", graphics_library_name );
	}

	Result::Name res = SelectGraphicsLibrary( graphics_library_name );
	if( res != Result::SUCCESS )
		return 0;

	LogOutput_HTML html_log( "app_" + string(GetBuildInfo()) + "-" + graphics_library_name + "_Log.html" );
	g_Log.AddLogOutput( &html_log );

	InitFreeImage();

	// Create the instance of the test class
	g_pTest = boost::shared_ptr<CGraphicsTestBase>( CreateTestInstance() );
	if( !g_pTest )
		return 0;

	const std::string app_title = g_pTest->GetAppTitle();
	const std::string app_class_name = app_title;

	int w = g_pTest->GetWindowWidth();  // 1280;
	int h = g_pTest->GetWindowHeight(); //  720;
	param_loader.LoadParam( "ScreenResolution", w, h );
	GameWindow::ScreenMode mode = GameWindow::WINDOWED;//g_pTest->GetFullscreen() ? GameWindow::FULLSCREEN : GameWindow::WINDOWED;
	GetGameWindowManager().CreateGameWindow( w, h, mode, app_title );
	g_Camera.SetAspectRatio( (float)w / (float)h );

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
	g_pFont = CreateDefaultBuiltinFont();
	g_pFont->SetFontSize( 8, 16 );

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

			UpdateCameraMatrices();

			float frametime = GlobalTimer().GetFrameTime();
			if( 0.1f < frametime )
				frametime = 0.1f;

			Update( frametime );

			if( g_pTest->GetCameraController() )
				g_Camera.SetPose( g_pTest->GetCameraController()->GetPose() );

			if( g_pTest->UseRenderBase() )
				g_pTest->RenderBase();
			else
				Render();

			ProfileDumpOutputToBuffer();

			GetGameWindowManager_Win32().OnMainLoopFinished();

			Sleep( 5 );
		}
    }

	g_pTest.reset();

	ReleaseGraphicsResources();

    // Clean up everything and exit the app
//	UnregisterClass( app_class_name.c_str(), wc.hInstance );
    return 0;
}
