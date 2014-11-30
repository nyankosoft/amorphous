#include <boost/foreach.hpp>
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/GraphicsResourceManager.hpp"
#include "amorphous/Graphics/Font/BuiltinFonts.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/ShaderManagerHub.hpp"
#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "amorphous/Graphics/Direct3D/Direct3D9.hpp"
#include "amorphous/Support/MiscAux.hpp"
#include "amorphous/Support/CameraController_Win32.hpp"
#include "amorphous/Support/CameraController.hpp"
#include "amorphous/Support/Timer.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/BitmapImage.hpp" // For InitFreeImage()
#include "amorphous/Support/lfs.hpp"
#include "amorphous/Support/Log/DefaultLogAux.hpp"
#include "amorphous/Support/FreeTypeAux.hpp"
#include "amorphous/Input/Win32/StdMouseInput.hpp"
#include "amorphous/Input/Win32/StdKeyboard.hpp"
//#include "amorphous/XML.hpp"
#include "amorphous/App/GameWindowManager_Win32.hpp"
//#include <gl/gl.h>

#include "GraphicsTestBase.hpp"

//#include <vld.h> // Visual Leak Detector - available only in the debugging mode of the debug build

using namespace std;
using namespace boost;


// >>> Implemented by demo app
unsigned int GetNumDemos();
const char **GetDemoNames();
CGraphicsTestBase *CreateDemoInstance( unsigned int index );
// >>> Implemented by demo app


class AppDemoFrameworkInputHandler;


// global variable


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


static boost::shared_ptr<CGraphicsTestBase> m_pDemo;

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

static Win32StdKeyboard g_StdKeyboard;

static boost::shared_ptr<InputHandler> g_pInputHandler;
static boost::shared_ptr<AppDemoFrameworkInputHandler> g_pDemoAppInputHandler;

Camera g_Camera;

//float g_FOV = (float)PI / 4.0f;

static const int sg_CameraControllerInputHandlerIndex = 0;
static const int sg_GraphicsTestInputHandlerIndex = 1;

static int m_DemoIndex = -1;

static void NextDemo();
static void PrevDemo();


static void ReleaseGraphicsResources()
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


static void Update( float frametime )
{
	PROFILE_FUNCTION();

	if( m_pDemo )
	{
		// Update the pose stored in the camera controller
		m_pDemo->UpdateCameraController( frametime );

		m_pDemo->Update( frametime );
	}
}


static void UpdateCameraMatrices()
{
	Matrix44 matView;
	g_Camera.GetCameraMatrix( matView );

	Matrix44 view;
	view.SetRowMajorMatrix44( (Scalar *)&matView );
	FixedFunctionPipelineManager().SetViewTransform( view );
}


/// \brief Draws the scene
static void Render()
{
	PROFILE_FUNCTION();

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );

//	g_Camera.SetPose( g_CameraController.GetPose() );
	GetShaderManagerHub().PushViewAndProjectionMatrices( g_Camera );

	GraphicsDevice().SetRenderState( RenderStateType::ALPHA_BLEND, true );
	GraphicsDevice().SetSourceBlendMode( AlphaBlend::SrcAlpha );
	GraphicsDevice().SetDestBlendMode( AlphaBlend::InvSrcAlpha );

    // clear the backbuffer to a blue color
//	GraphicsDevice().SetClearColor( m_pDemo->GetBackgroundColor() );
//	GraphicsDevice().SetClearDepth( 1.0f );
//	GraphicsDevice().Clear()
	if( pd3dDevice )
	{
		pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, m_pDemo->GetBackgroundColor().GetARGB32(), 1.0f, 0 );
	}
	else
	{
		PROFILE_SCOPE( "Clear the color and depth buffers" );
		const SFloatRGBAColor c = m_pDemo->GetBackgroundColor();
		glClearColor( c.red, c.green, c.blue, c.alpha );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	}

	m_pDemo->RenderScene();

    // begin the scene
	if( pd3dDevice )
		pd3dDevice->BeginScene();

	m_pDemo->Render();

	m_pDemo->DisplayDebugInfo();

	if( pd3dDevice )
	{
		// end the scene
		pd3dDevice->EndScene();

		// present the backbuffer contents to the display
		pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}
}


/// \brief The window's message handler
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
			if( m_pDemo )
				m_pDemo->AcquireInputDevices();
        }
        break;

	default:
		break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


static int Run( LPSTR lpCmdLine )
{
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

	string graphics_library_name = "OpenGL";
	ParamLoader param_loader( "config" );
	if( param_loader.IsReady() )
	{
		param_loader.LoadParam( "graphics_library", graphics_library_name );
	}

	Result::Name res = SelectGraphicsLibrary( graphics_library_name );
	if( res != Result::SUCCESS )
		return 0;

	InitHTMLLog( "app_" + string(GetBuildInfo()) + "-" + graphics_library_name + "_Log.html" );

	InitFreeImage();

	LogFreeTypeLibraryVersion();

	bool initialized = InitDemo();
	if( !initialized )
		return 0;

//	ChangeClientAreaSize( hWnd, m_pDemo->GetWindowWidth(), m_pDemo->GetWindowHeight() );

	ProfileInit();

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

			if( m_pDemo->GetCameraController() )
				g_Camera.SetPose( m_pDemo->GetCameraController()->GetPose() );

			if( m_pDemo->UseRenderBase() )
				m_pDemo->RenderBase();
			else
				Render();

			ProfileDumpOutputToBuffer();

			GetGameWindowManager_Win32().OnMainLoopFinished();

			Sleep( 5 );
		}
    }

	m_pDemo.reset();

	ReleaseGraphicsResources();

    // Clean up everything and exit the app
//	UnregisterClass( app_class_name.c_str(), wc.hInstance );
    return 0;
}


/// \brief The application's entry point
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, INT )
{
	int ret = 0;

	try
	{
		ret = Run( lpCmdLine );
	}
	catch( std::exception& e )
	{
		GlobalLog().Print( WL_WARNING, "exception: %s", e.what() );
	}

	return ret;
}
