//-----------------------------------------------------------------------------
// File: D3DAppTestMain.cpp
//-----------------------------------------------------------------------------

#include <vld.h>

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include "3DCommon/all.h"

#include "GameCommon/Timer.h"
#include "Support.h"
#include "Support/FileOpenDialog_Win32.h"
#include "GameInput.h"

#include "Stage/StaticGeometry.h"

#include "XML/XMLDocumentLoader.h"

#include "StaticGeometryCompilerFG.h"
#include "StaticGeometryCompiler.h"



class CStaticGeometryViewer
{

	boost::shared_ptr<CStaticGeometryBase> m_pStaticGeometry;

public:

	CStaticGeometryViewer() {}

	~CStaticGeometryViewer() {}

	void Render( const CCamera& camera );

	void Init() {}

	bool LoadFromFile( const std::string& sg_db_filepath )
	{
		m_pStaticGeometry = boost::shared_ptr<CStaticGeometryBase>( new CStaticGeometry( NULL ) );
		return m_pStaticGeometry->LoadFromFile( sg_db_filepath );
	}

	void Update( float dt ) {}
};


void CStaticGeometryViewer::Render( const CCamera& camera )
{
	m_pStaticGeometry->Render( camera, 0 );
}


boost::shared_ptr<CStaticGeometryViewer> g_pTest;

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

CStdKeyboard g_StdKeyboard;

boost::shared_ptr<CFontBase> g_pFont;

CCameraController_Win32 g_CameraController;

CCamera g_Camera;

CLogOutput_OnScreen *g_pLogOutput = NULL;



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

	g_Camera.SetPose( g_CameraController.GetPose() );
	ShaderManagerHub.PushViewAndProjectionMatrices( g_Camera );

	D3DXMATRIX mat;
	g_Camera.GetCameraMatrix( mat );
//	g_pTest->UpdateViewTransform( mat );
	g_Camera.GetProjectionMatrix( mat );
//	g_pTest->UpdateProjectionTransform( mat );


    // clear the backbuffer to a blue color
    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xFF303030, 1.0f, 0 );

    // begin the scene
    pd3dDevice->BeginScene();

	g_pTest->Render( g_Camera );

	// rendering
	char acStr[256];
	sprintf( acStr, "%f", TIMER.GetFPS() );
	g_pFont->DrawText( acStr, Vector2(20,20), 0xFFFFFFFF );
//	g_pFont->DrawText( to_string(TIMER.GetFPS()), D3DXVECTOR2(20,20), 0xFFFFFFFF );

	int i=0;
	const vector<string>& vecProfileResults = GetProfileText();
	BOOST_FOREACH( const string& text, vecProfileResults )
	{
		g_pFont->DrawText( text.c_str(), Vector2( 20, 40 + i*16 ), 0xF0F0F0FF );
		i++;
	}

//	if( g_pLogOutput )
//		g_pLogOutput->Render();

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
	g_pTest.reset();

	g_Log.RemoveLogOutput( g_pLogOutput );
	SafeDelete( g_pLogOutput );

	g_pFont.reset();

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
	boost::shared_ptr<CFont> pFont = boost::shared_ptr<CFont>( new CFont() );
	pFont->InitFont( "Arial", 8, 16 );
	g_pFont = pFont;

	// output log text on the screen
	g_pLogOutput = new CLogOutput_OnScreen( "Arial", 6, 12, 24, 120 );
	g_Log.AddLogOutput( g_pLogOutput );
	g_pLogOutput->SetTopLeftPos( Vector2(8,16) );

	CStaticGeometryCompilerFG compiler_fg;

//	SetDefaultSkyboxMesh( compiler_fg );

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
		if( /* Win32 file open dialog is available == */ true )
		{
			// select a desc file from OpenFile dialog
			if( !GetFilename(filename, NULL) )
				return 0;
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

//	filename = "./testdata/static_geometry_desc_draft.xml";

	// build static geometry
	if( 0 < filename.length() )
	{
		if( filename.substr( filename.length() - 3 ) == "xml" )
		{
			// compile a static geometry archive

			CXMLParserInitReleaseManager xml_parser_mgr;

			// compile static geometry
			LOG_SCOPE( "- static geomery compiler test." );
			CStaticGeometryCompiler compiler;
			bool compiled = compiler.CompileFromXMLDescFile( filename );

			if( compiled )
			{
				// go to the directory of the input xml file
				fnop::set_wd(fnop::get_path(filename));

				// go to the directory where a static geometry file was saved
				string output_filepath = compiler.GetDesc().m_OutputFilepath;
				fnop::set_wd( fnop::get_path(output_filepath) );

				g_pTest = boost::shared_ptr<CStaticGeometryViewer>( new CStaticGeometryViewer() );
				g_pTest->LoadFromFile( fnop::get_nopath(output_filepath) );
			}
			else
			{
				ReleaseMain( wc );
				return 0;
			}
		}
		else
		{
			// compile a static geometry archive of the previous version
			// - create from a .rd (resource desc) file
			compiler_fg.Build( filename );
		}
	}

	// exit the app here if the input file given as a command line argument
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

		Sleep( 2 );
    }

	ReleaseMain( wc );

    return 0;
}
