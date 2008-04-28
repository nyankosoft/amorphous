//-----------------------------------------------------------------------------
// File: StageMain.cpp
// Desc: Load the bspx file and provides a simple 
//-----------------------------------------------------------------------------
#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <direct.h>


#include "3DCommon/Direct3D9.h"
#include "3DCommon/font.h"
//#include "3DCommon/3DRect.h"
//#include "3DCommon/fps.h"

#include "Support/memory_helpers.h"
#include "Support/FileOpenDialog_Win32.h"

#include "MaterialFileCompiler.h"


/*  link d3d9.lib d3dx9.lib winmm.lib  */

//----------------------  Global Variables  -----------------------------------
CFont g_Font;

//----------------------  Forward Declaration  -----------------------------------


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
				
		case WM_KEYDOWN:
			switch( wParam )
			{
			case VK_F1:
				break;
			case VK_F2:
				break;
			case VK_F4:
				break;
			case VK_F5:
				break;
			case VK_F7:
				break;
			case VK_F12:
				break;
			}
			break;

		case WM_KEYUP:
			break;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


void Render()
{
	//=======================   Rendering start   ===========================
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	   pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

	// Begin the scene
	pd3dDevice->BeginScene();

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	// End the scene
	pd3dDevice->EndScene();
	// Present the backbuffer contents to the display
	pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


bool Init( LPSTR lpCmdLine )
{
	return true;
}


void Compile( LPSTR lpCmdLine )
{
	char acFilename[1024];

	if( strlen(lpCmdLine) == 0 )
	{
		GetFilename( acFilename );
	}
	else
	{	// A file has been specified as the argument
		// trim the double quotation characters at the beginning and at the end of the string
		strcpy( acFilename, lpCmdLine+1 );
		acFilename[ strlen(acFilename)-1 ] = '\0';
	}

	CMaterialFileCompiler mat_compiler;

	mat_compiler.SetOutputDirectory( "D:\\3D_Development\\Project\\App\\StageBase\\Stage\\" );

	mat_compiler.Compile( acFilename );





//	mat_compiler.Compile( "..\\..\\Project1\\GameMain\\temp\\MaterialList.mat");
//	mat_compiler.SaveToBinaryFile( "..\\..\\Project1\\GameMain\\System\\Material.dat");

}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, INT )
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "D3DTest", NULL };
    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( "D3DTest", "MaterialFileCompiler - done.",
                              WS_OVERLAPPEDWINDOW, 320, 200, 640, 480,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

    // Initialize Direct3D
    if( !CDIRECTX9.InitD3D(hWnd) )
		return 0;

    // Show the window
    ShowWindow( hWnd, SW_SHOWDEFAULT );
    UpdateWindow( hWnd );

	if( !Init(lpCmdLine) )
		return 1;

	D3DXMATRIX matCamera, matProj;

	D3DXMatrixLookAtLH( &matCamera, &D3DXVECTOR3(0, 0, -2), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0) );
	D3DXMatrixPerspectiveFovLH( &matProj, 3.141592f / 3.0f, 4.0f / 3.0f, 0.1f, 100.0f);

	// initialize font object
	g_Font.InitFont( "ÇlÇr ÉSÉVÉbÉN", 12, 24);


	Compile( lpCmdLine );


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
			//Update FPS
//			FPS.UpdateFPS();

			Render();

		}
    }

    // Clean up everything before finishing
	CDIRECTX9.Release();
    UnregisterClass( "D3DTest", wc.hInstance );

    return 0;
}





