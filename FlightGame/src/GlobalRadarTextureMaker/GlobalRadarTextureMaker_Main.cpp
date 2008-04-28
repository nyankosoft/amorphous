//-----------------------------------------------------------------------------
// File: D3DAppTestMain.cpp
//-----------------------------------------------------------------------------

#include "3DCommon/Direct3D9.h"
#include "3DCommon/Font.h"

#include "GameCommon/Timer.h"
#include "Support/FileOpenDialog_Win32.h"
#include "Support/WindowMisc_Win32.h"

#include "GlobalRadarTextureMaker.h"

#include <string>
using namespace std;


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

CFont g_Font;


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render( CGlobalRadarTextureMaker& maker )
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

    // clear the backbuffer to a blue color
    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

    // begin the scene
    pd3dDevice->BeginScene();


	// rendering
	char acStr[256];
	sprintf( acStr, "%f", TIMER.GetFPS() );
///	g_Font.DrawText( acStr, D3DXVECTOR2(20,20), 0xFFFFFFFF );

	// render terrain geometry
	maker.Render();

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
    switch( msg )
    {
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, INT )
{
    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "D3D Test", NULL };
    RegisterClassEx( &wc );

	int window_height = 1024;
	int window_width = 1024;

    // Create the application's window
    HWND hWnd = CreateWindow( "D3D Test", "D3D Application Test",
                              WS_OVERLAPPEDWINDOW, 100, 100, window_width, window_height,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

	ChagneClientAreaSize( hWnd, window_width, window_height );

    // Show the window
    ShowWindow( hWnd, SW_SHOWDEFAULT );
    UpdateWindow( hWnd );

	//Initialize Direct3D
	if( !DIRECT3D9.InitD3D( hWnd ) )
		return 0;

	// init font
	g_Font.InitFont( "Arial", 8, 16 );

    CGlobalRadarTextureMaker maker;
	maker.InitShader();

	string filename;
	GetFilename( filename );
	maker.LoadGeometry( filename );

	float fMargin = 6000.0f;
//	maker.SetViewWidth( 600.0f );
	maker.SetViewWidth( 600000.0f + fMargin );

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

			Render( maker );
		}
    }


    // Clean up everything and exit the app
    UnregisterClass( "D3D Test", wc.hInstance );
    return 0;
}
