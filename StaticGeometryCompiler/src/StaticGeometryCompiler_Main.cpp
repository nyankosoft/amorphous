//-----------------------------------------------------------------------------
// File: D3DAppTestMain.cpp
//-----------------------------------------------------------------------------

#include "3DCommon/Direct3D9.h"
#include "3DCommon/Font.h"
#include "3DCommon/LogOutput_OnScreen.h"

#include "GameCommon/Timer.h"

#include "Support/CameraController_Win32.h"
//#include "Support/FileOpenDialog_Win32.h"
#include "Support/ParamLoader.h"
#include "Support/Log/DefaultLog.h"
#include "Support/memory_helpers.h"
#include "Support/fnop.h"
#include "XML/XMLDocumentLoader.h"

#include "StaticGeometryCompilerFG.h"

//#include "StaticGeometryDesc.h"

#include <vld.h>


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

CFont g_Font;

CCameraController_Win32 g_CameraController;

CLogOutput_OnScreen *g_pLogOutput = NULL;


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

    // clear the backbuffer to a blue color
    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

    // begin the scene
    pd3dDevice->BeginScene();


	// rendering
	char acStr[256];
	sprintf( acStr, "%f", TIMER.GetFPS() );
	g_Font.DrawText( acStr, D3DXVECTOR2(20,20), 0xFFFFFFFF );

	if( g_pLogOutput )
		g_pLogOutput->Render();

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

	switch( msg )
	{
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

/*
/// availability: Windows
std::string get_exe_filepath()
{
	char exe_filepath[1024];
	memset( exe_filepath, 0, sizeof(exe_filepath) );
	GetModuleFileName( NULL, exe_filepath, 1023 );

	return string(exe_filepath);
}


void SetDefaultSkyboxMesh( CStaticGeometryCompilerFG& compiler )
{

//	const char *argv0 = argv[0];

	char full_path[1024];
	_fullpath( full_path, ".", 1023 ); 

	// push current directory and move to the directory where the program is placed

	fnop::dir_stack dirstack;
	dirstack.setdir( fnop::get_path(get_exe_filepath()) );

	// load default skybox (stored under the directory of exe)
	compiler.SetDefaultSkyboxMesh( "default_skybox.msh" );

	// pop the previous working directory
	dirstack.prevdir();
}
*/


void ReleaseMain( WNDCLASSEX& wc )
{
	g_Log.RemoveLogOutput( g_pLogOutput );
	SafeDelete( g_pLogOutput );

    // Clean up everything and exit the app
    UnregisterClass( "D3D Test", wc.hInstance );
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, INT )
{
	string initial_working_directory = fnop::get_cwd();

	string cmd_line = lpCmdLine;

    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "D3D Test", NULL };
    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( "D3D Test", "Static Geometry Compiler",
                              WS_OVERLAPPEDWINDOW, 200, 200, 800, 600,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );


    // Show the window
    ShowWindow( hWnd, SW_SHOWDEFAULT );
    UpdateWindow( hWnd );

	// Initialize Direct3D
	if( !DIRECT3D9.InitD3D( hWnd ) )
		return 0;

	// init font
	g_Font.InitFont( "Arial", 8, 16 );

	// output log text on the screen
	g_pLogOutput = new CLogOutput_OnScreen( "Arial", 6, 12, 24, 120 );
	g_Log.AddLogOutput( g_pLogOutput );
	g_pLogOutput->SetTopLeftPos( Vector2(8,16) );


	// experiment - load a xml file
/*	{
		CXMLParserInitReleaseManager xml_parser_mgr;

		LOG_SCOPE( "- An xml doc load experiment. Comment out this block to use static geomery compiler." );
		CStaticGeometryDesc desc;
		desc.LoadFromXML( "./static_geometry_desc_draft.xml" );
		return 0;
	}
*/

	CStaticGeometryCompilerFG compiler;

//	SetDefaultSkyboxMesh( compiler );

	string filename;
	if( 0 < cmd_line.length() )
	{
		// a filename is given as an argument

		if( cmd_line[0] == '"' )
		{
			// trim the double quotation characters at the beginning and the end of the string
			filename = cmd_line.substr( 1, cmd_line.length() - 2 );
		}
		else
		{
			filename = cmd_line;
		}

		// set the working directory to the one immediately under the desc file

        fnop::set_wd( fnop::get_path(filename) );

		filename = fnop::get_nopath( filename );
	}
	else
	{
		// no command line arguments
		if( /* Win32 file open dialog is available == */ false )
		{
			// select a desc file from OpenFile dialog
//			if( !GetFilename(filename, NULL) )
//				return 0;
		}
		else
		{
			// select a desc file written in "./default_input"
			// - see "D:\R&D\Project\App2\StaticGeometryCompiler"
			CParamLoader loader( "default_input.txt" );
			if( loader.IsReady() )
			{
				loader.LoadParam( "input", filename );
			}

			fnop::set_wd( fnop::get_path(filename) );

			filename = fnop::get_nopath( filename );
		}
	}

	// set log output device
	// - GetFilename() has changed working directory to the directory where the selected file exists.
	//   Open the log file in the same directory.
	//   
	CLogOutput_HTML html_log( "log.html" );
	g_Log.AddLogOutput( &html_log );

	CLogOutput_TextFile textfile_log( "log.txt" );
	g_Log.AddLogOutput( &textfile_log );

	g_Log.Print( "initial working directory: " + initial_working_directory );

	g_Log.Print( "current working directory: " + fnop::get_cwd() );

	g_Log.Print( "command line argument string: " + cmd_line );

	g_Log.Print( "Compiling a static geometry from the following desc file: '%s'", filename.c_str() );

//	g_Log.Print( "directory path of exe file: " + fnop::get_path(get_exe_filepath()) );

//	g_Log.Print( "_fullpath( dest, '.', MAX_PATH ) > dest: '%s'", full_path );

	// build static geometry
	if( 0 < filename.length() )
		compiler.Build( filename );

	if( cmd_line.length() )
	{
		ReleaseMain( wc );
		return 0;
	}

	// Enter the message loop
	MSG msg;
	ZeroMemory( &msg, sizeof(msg) );

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

			Render();
		}
    }

	ReleaseMain( wc );

    return 0;
}
