#include "GameWindowManager_Win32_D3D.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/2DPrimitive/2DPrimitiveRenderer.hpp"
#include <math.h>

#include "Support/WindowMisc_Win32.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/StringAux.hpp"


namespace amorphous
{


LRESULT (WINAPI *g_pMessageProcedureForGameWindow)( HWND, UINT, WPARAM, LPARAM ) = NULL;

// definition of the singleton instance
CGameWindowManager_Win32_D3D CGameWindowManager_Win32_D3D::ms_SingletonInstance_;


CGameWindowManager_Win32_D3D::CGameWindowManager_Win32_D3D()
{
	m_CurrentScreenMode = GameWindow::WINDOWED;
}


CGameWindowManager_Win32_D3D::~CGameWindowManager_Win32_D3D()
{
	DIRECT3D9.Release();
    UnregisterClass( m_ApplicationClassName.c_str(), m_WindowClassEx.hInstance );
}


void GetCurrentResolution(int* piDesktopWidth, int* piDesktopHeight)
{
	// experiment : check the current resolution
	RECT desktop_rect;
	HWND hDesktopWnd = GetDesktopWindow();
	GetClientRect( hDesktopWnd, &desktop_rect );

	*piDesktopWidth  = desktop_rect.right - desktop_rect.left;
	*piDesktopHeight = desktop_rect.bottom   - desktop_rect.top;
}


bool CGameWindowManager_Win32_D3D::CreateGameWindow( int iScreenWidth, int iScreenHeight, GameWindow::ScreenMode screen_mode, const std::string& app_title )
{
	LOG_FUNCTION_SCOPE();

	// detemine the specification of the window class
	m_WindowClassEx.cbSize		= sizeof(WNDCLASSEX); 
    m_WindowClassEx.style			= CS_CLASSDC; 
//	m_WindowClassEx.lpfnWndProc	= MsgProc;
//	m_WindowClassEx.lpfnWndProc	= m_pMsgProc; 		// error: wrong function pointer
    m_WindowClassEx.lpfnWndProc	= g_pMessageProcedureForGameWindow; 		// correct: a valid function pointer to a message procedure

	m_ApplicationClassName = "Application[" + app_title + "]";

    m_WindowClassEx.cbClsExtra	= 0L; 
    m_WindowClassEx.cbWndExtra	= 0L; 
    m_WindowClassEx.hInstance		= GetModuleHandle(NULL); 
    m_WindowClassEx.hIcon			= NULL; 
    m_WindowClassEx.hCursor       = NULL; 
    m_WindowClassEx.hbrBackground = NULL; 
    m_WindowClassEx.lpszMenuName  = NULL; 
    m_WindowClassEx.lpszClassName = m_ApplicationClassName.c_str();//"Stage Test"; 
    m_WindowClassEx.hIconSm		= NULL;

    // register the window class
    ATOM atom = RegisterClassEx( &m_WindowClassEx );

	int iDesktopWidth, iDesktopHeight;
	GetCurrentResolution( &iDesktopWidth, &iDesktopHeight );

    // create the application's window
    m_hWnd = CreateWindow( m_ApplicationClassName.c_str(), app_title.c_str(),
                           WS_OVERLAPPEDWINDOW,
                           //WS_POPUPWINDOW, // no title bar
                           //WS_OVERLAPPED, // does not have standard buttons like close, minimize, etc.
						   (iDesktopWidth  - iScreenWidth ) / 2,
						   (iDesktopHeight - iScreenHeight) / 2,
						   iScreenWidth,
						   iScreenHeight,
                           GetDesktopWindow(),
						   NULL,
						   m_WindowClassEx.hInstance,
						   NULL );

	if( screen_mode == GameWindow::WINDOWED )
		ChangeClientAreaSize( m_hWnd, iScreenWidth, iScreenHeight );

	LOG_PRINT( fmt_string("Created a window (size: %d x %d)", iScreenWidth, iScreenHeight ) );

    // initialize Direct3D & Create the scene geometry
	int d3d_screen_mode = screen_mode == GameWindow::WINDOWED ? CDirect3D9::WINDOWED : CDirect3D9::FULLSCREEN;
    if( !DIRECT3D9.InitD3D( m_hWnd, iScreenWidth, iScreenHeight, d3d_screen_mode ) )
		return false;

	// init 2d primitive renderer
	PrimitiveRenderer().Init();

	// show the window
    ShowWindow( m_hWnd, SW_SHOWDEFAULT );
    UpdateWindow( m_hWnd );

	LOG_PRINT( "Displayed and updated the window" );

	m_iCurrentScreenWidth  = iScreenWidth;
	m_iCurrentScreenHeight = iScreenHeight;

	// set the window size for all the game components
//	CGameComponent::SetScreenSize( iScreenWidth, iScreenHeight );

	// update parameters for GraphicsComponents
	CGraphicsParameters params;
	params.ScreenWidth  = iScreenWidth;
	params.ScreenHeight = iScreenHeight;
	params.bWindowed    = screen_mode == GameWindow::WINDOWED ? true : false;

	CGraphicsComponentCollector::Get()->SetGraphicsPargams( params );

	return true;
}


void CGameWindowManager_Win32_D3D::ChangeScreenSize( int iNewScreenWidth,
												 int iNewScreenHeight,
												 bool bFullScreen )
{
	// check if the requested window size has a valid aspect ratio
	// (width : height) must be 4:3
//	if( 0.01f < fabs( (float)iNewScreenHeight / (float)iNewScreenWidth - 0.75f ) )
//		return;


	// release all the graphic resources ( textures, vertex buffers, mesh models, and so on )
	CGraphicsComponentCollector::Get()->ReleaseGraphicsResources();

	if( !DIRECT3D9.ResetD3DDevice( m_hWnd, iNewScreenWidth, iNewScreenHeight, bFullScreen ) )
	{
		// the requested resolution is not available - restore the current settings
		bool bCurrentModeFullScreen = (m_CurrentScreenMode == GameWindow::FULLSCREEN) ? true : false;
		DIRECT3D9.ResetD3DDevice( m_hWnd, m_iCurrentScreenWidth, m_iCurrentScreenHeight, bCurrentModeFullScreen );
	}

	// update the current resolution and the screen mode
	m_iCurrentScreenWidth  = iNewScreenWidth;
	m_iCurrentScreenHeight = iNewScreenHeight;
	m_CurrentScreenMode   = GameWindow::WINDOWED; // TODO: use the new screen mode


	CGraphicsParameters param;
	param.ScreenWidth  = iNewScreenWidth;
	param.ScreenHeight = iNewScreenHeight;
	param.bWindowed = (!bFullScreen);

	// notify all the graphics components to load their resources
	CGraphicsComponentCollector::Get()->LoadGraphicsResources( param );

	// notify changes to all the game components
//	GAMECOMPONENTCOLLECTOR.AdaptToNewScreenSize();

	if( m_CurrentScreenMode == GameWindow::WINDOWED )
	{
		ChangeClientAreaSize( m_hWnd, m_iCurrentScreenWidth, m_iCurrentScreenHeight );
	}

	// adjust the position of the window so that the game screen appear in the middle of the display
	int iDesktopWidth, iDesktopHeight;
	GetCurrentResolution( &iDesktopWidth, &iDesktopHeight );
/*	int iResult = ::SetWindowPos(m_hWnd, HWND_TOP,
		                         (iDesktopWidth  - m_iCurrentScreenWidth ) / 2,
							     (iDesktopHeight - m_iCurrentScreenHeight) / 2,
							     m_iCurrentScreenWidth  + GetNonClientAreaWidth(m_hWnd),
							     m_iCurrentScreenHeight + GetNonClientAreaHeight(m_hWnd),
							     0 );
*/
}


} // namespace amorphous
