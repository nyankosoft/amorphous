//-----------------------------------------------------------------------------
// File: D3DAppTestMain.cpp
//-----------------------------------------------------------------------------

//#include <vld.h>

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include "Graphics/all.hpp"
#include "Graphics/LogOutput_OnScreen.hpp"
#include "Support.hpp"
#include "Support/FileOpenDialog_Win32.hpp"
#include "Support/MiscAux.hpp"
#include "Input.hpp"
#include "Stage/StaticGeometry.hpp"
#include "XML/XMLDocumentLoader.hpp"

#include "StaticGeometryCompilerFG.h"
#include "StaticGeometryCompiler.h"
#include "StaticGeometryCompiler_Main.h"



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

CPlatformDependentCameraController g_CameraController;

CCamera g_Camera;

std::string m_OutputFilepath;

CLogOutput_ScrolledTextBuffer *g_pLogOutput = NULL;



//-----------------------------------------------------------------------------
// Name: Update( float frametime )
// Desc: 
//-----------------------------------------------------------------------------
void Update( float frametime )
{
	PROFILE_FUNCTION();

	if( g_pTest )
		g_pTest->Update( frametime );
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	PROFILE_FUNCTION();

	ONCE( LOG_PRINT( " Entered" ) );

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

	if( g_pTest )
		g_pTest->Render( g_Camera );

	// rendering
	char acStr[256];
	sprintf( acStr, "%f", GlobalTimer().GetFPS() );
	g_pFont->DrawText( acStr, Vector2(20,20), 0xFFFFFFFF );
//	g_pFont->DrawText( to_string(GlobalTimer().GetFPS()), D3DXVECTOR2(20,20), 0xFFFFFFFF );

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

	ONCE( LOG_PRINT( " Leaving" ) );
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	g_CameraController.HandleMessage( msg, wParam, lParam );

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
	LOG_FUNCTION_SCOPE();

	g_pTest.reset();

	g_Log.RemoveLogOutput( g_pLogOutput );
	SafeDelete( g_pLogOutput );

	g_pFont.reset();

    // Clean up everything and exit the app
    UnregisterClass( "D3D Test", wc.hInstance );
}


int RunWin32App( const std::string& cmd_line )
{
	string initial_working_directory = fnop::get_cwd();

	// init font
	boost::shared_ptr<CFont> pFont = boost::shared_ptr<CFont>( new CFont() );
	pFont->InitFont( "Arial", 8, 16 );
	g_pFont = pFont;

	bool compiled = RunStaticGeometryCompiler( cmd_line, initial_working_directory );

	if( compiled )
	{
		g_pTest = boost::shared_ptr<CStaticGeometryViewer>( new CStaticGeometryViewer() );
		g_pTest->LoadFromFile( fnop::get_nopath(m_OutputFilepath) );
	}

	// exit the app here if the input file given as a command line argument
	if( cmd_line.length() )
	{
		return 0;
	}

	LOG_PRINT( " Entering the main message loop..." );

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
			GlobalTimer().UpdateFrameTime();

			g_CameraController.UpdateCameraPose( GlobalTimer().GetFrameTime() );

//			g_CameraController.SetCameraMatrix();

			Render();
		}

		Sleep( 2 );
    }

	LOG_PRINT( " Cleaning up..." );

    return 0;
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

	try
	{
		int ret = RunWin32App( cmd_line );
	}
	catch( exception& e )
	{
		g_Log.Print( WL_ERROR, "exception: %s", e.what() );
	}

	ReleaseMain( wc );

	return 0;
}
