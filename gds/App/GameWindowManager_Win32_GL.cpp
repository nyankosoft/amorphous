#include "GameWindowManager_Win32_GL.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/2DPrimitive/2DPrimitiveRenderer.hpp"
#include "Graphics/OpenGL/GLGraphicsDevice.hpp"
#include "Graphics/OpenGL/GLExtensions.hpp"
#include "Support/WindowMisc_Win32.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/StringAux.hpp"


namespace amorphous
{

using namespace std;


LRESULT (WINAPI *g_pMessageProcedureForGameWindow_Win32_GL)( HWND, UINT, WPARAM, LPARAM ) = NULL;

// definition of the singleton instance
GameWindowManager_Win32_GL GameWindowManager_Win32_GL::ms_SingletonInstance_;


void ReSizeGLScene(GLsizei width, GLsizei height)		// Resize and initialize the GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}


// done by CGLGraphicsDevice
/*
int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	glEnable(GL_TEXTURE_2D);
	LOG_GL_ERROR( "glEnable() failed." );

	return TRUE;										// Initialization Went OK
}
*/


GameWindowManager_Win32_GL::GameWindowManager_Win32_GL()
{
	m_CurrentScreenMode = GameWindow::WINDOWED;

	m_hDC  = NULL;
	m_hRC  = NULL;
	m_hWnd = NULL;
	m_hInstance = NULL;
}


GameWindowManager_Win32_GL::~GameWindowManager_Win32_GL()
{
//	DIRECT3D9.Release();
//	UnregisterClass( m_ApplicationClassName.c_str(), m_WindowClassEx.hInstance );
}


/*	This Code Creates Our OpenGL Window.  Parameters Are:
 *	title			- Title To Appear At The Top Of The Window
 *	width			- Width Of The GL Window Or Fullscreen Mode
 *	height			- Height Of The GL Window Or Fullscreen Mode
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)
 */

//bool CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
bool GameWindowManager_Win32_GL::CreateGameWindow( int iScreenWidth, int iScreenHeight, GameWindow::ScreenMode screen_mode, const std::string& app_title )
{
	LOG_FUNCTION_SCOPE();

	int bits = 16;

	const int width  = iScreenWidth;
	const int height = iScreenHeight;

	GLuint		PixelFormat;		        // Holds The Results After Searching For A Match
	WNDCLASS	wc;					        // Windows Class Structure
	DWORD		dwExStyle;			        // Window Extended Style
	DWORD		dwStyle;			        // Window Style
	RECT		WindowRect;			        // Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left   =(long)0;		        // Set Left Value To 0
	WindowRect.right  =(long)width;	        // Set Right Value To Requested Width
	WindowRect.top    =(long)0;	            // Set Top Value To 0
	WindowRect.bottom =(long)height;        // Set Bottom Value To Requested Height

	string class_name = "[[" + app_title + "]]";
	m_ClassName = class_name;

	m_hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) g_pMessageProcedureForGameWindow;//_Win32_GL;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;                                    // No Extra Window Data
	wc.cbWndExtra		= 0;                                    // No Extra Window Data
	wc.hInstance		= m_hInstance;                          // Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);          // Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);          // Load The Arrow Pointer
	wc.hbrBackground	= NULL;                                 // No Background Required For GL
	wc.lpszMenuName		= NULL;                                 // We Don't Want A Menu
	wc.lpszClassName	= class_name.c_str();                   // Set The Class Name

	if( !RegisterClass(&wc) )									// Attempt To Register The Window Class
	{
		LOG_PRINT_ERROR( "RegisterClass() failed. Failed to register the window class." );
		return FALSE;											// Return FALSE
	}
	
	if( screen_mode == GameWindow::FULLSCREEN )												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;                // Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;               // Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;                 // Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if( ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL )
		{
			// Use the windowed mode
			screen_mode = GameWindow::WINDOWED;		// Windowed Mode Selected.
		}
	}

	if( screen_mode == GameWindow::FULLSCREEN )						// Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW;                                // Window Extended Style
		dwStyle   = WS_POPUP;		                                // Windows Style
		ShowCursor(FALSE);			                                // Hide Mouse Pointer
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;             // Window Extended Style
		dwStyle   = WS_OVERLAPPEDWINDOW;                            // Windows Style
	}												                

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(m_hWnd=CreateWindowEx( dwExStyle,							// Extended Style For The Window
								class_name.c_str(),	                // Class Name
								app_title.c_str(),		            // Window Title
								dwStyle |				            // Defined Window Style
								WS_CLIPSIBLINGS |		            // Required Window Style
								WS_CLIPCHILDREN,		            // Required Window Style
								0, 0,					            // Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								m_hInstance,						// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow(); // Reset The Display
		LOG_PRINT_ERROR( "CreateWindowEx() failed." );
		return FALSE;
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if( !(m_hDC=GetDC(m_hWnd)) )                    // Did we get a Device Context?
	{
		KillGLWindow();								// Reset The Display
		LOG_PRINT_ERROR( "Can't create a GL Device Context." );
		return FALSE;								// Return FALSE
	}

	if( !(PixelFormat=ChoosePixelFormat(m_hDC,&pfd)) )	// Did Windows find a matching pixel format?
	{
		KillGLWindow(); // Reset The Display
		LOG_PRINT_ERROR( "Can't find a suitable PixelFormat." );
		return FALSE;								// Return FALSE
	}

	if( !SetPixelFormat(m_hDC,PixelFormat,&pfd) )     // Are We Able To Set The Pixel Format?
	{
		KillGLWindow(); // Reset The Display
		LOG_PRINT_ERROR( "Can't set the PixelFormat." );
		return FALSE;								// Return FALSE
	}

	if (!(m_hRC=wglCreateContext(m_hDC)))           // Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		LOG_PRINT_ERROR( "Can't create a GL Rendering Context." );
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(m_hDC,m_hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		LOG_PRINT_ERROR( "Can't activate the GL Rendering Context." );
		return FALSE;								// Return FALSE
	}

	ShowWindow(m_hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(m_hWnd);						// Slightly Higher Priority
	SetFocus(m_hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

//	if (!InitGL())									// Initialize Our Newly Created GL Window
	if( !GLGraphicsDevice().Init( iScreenWidth, iScreenHeight, (screen_mode == ScreenMode::WINDOWED) ? ScreenMode::WINDOWED : ScreenMode::FULLSCREEN ) )
	{
		KillGLWindow();								// Reset The Display
		LOG_PRINT_ERROR( "InitGL() failed." );
		return FALSE;								// Return FALSE
	}

	// initialize OpenGL extensions
	initExtensions( m_hDC );

	// save width and height
	m_iCurrentScreenWidth  = iScreenWidth;
	m_iCurrentScreenHeight = iScreenHeight;

	// update graphics params
	GraphicsParameters params = GraphicsComponentCollector::Get()->GetGraphicsParams();
	params.ScreenWidth  = iScreenWidth;
	params.ScreenHeight = iScreenHeight;
	params.bWindowed = ( screen_mode == GameWindow::WINDOWED );
	GraphicsComponentCollector::Get()->SetGraphicsPargams( params );

	// init 2d primitive renderer
	Get2DPrimitiveRenderer().Init();

	return TRUE;									// Success
}


void GameWindowManager_Win32_GL::ChangeScreenSize( int iNewScreenWidth,
												 int iNewScreenHeight,
												 bool bFullScreen )
{/*
	// check if the requested window size has a valid aspect ratio
	// (width : height) must be 4:3
//	if( 0.01f < fabs( (float)iNewScreenHeight / (float)iNewScreenWidth - 0.75f ) )
//		return;


	// release all the graphic resources ( textures, vertex buffers, mesh models, and so on )
	GraphicsComponentCollector::Get()->ReleaseGraphicsResources();

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


	GraphicsParameters param;
	param.ScreenWidth  = iNewScreenWidth;
	param.ScreenHeight = iNewScreenHeight;
	param.bWindowed = (!bFullScreen);

	// notify all the graphics components to load their resources
	GraphicsComponentCollector::Get()->LoadGraphicsResources( param );

	// notify changes to all the game components
//	GAMECOMPONENTCOLLECTOR.AdaptToNewScreenSize();

	if( m_CurrentScreenMode == GameWindow::WINDOWED )
	{
		ChangeClientAreaSize( m_hWnd, m_iCurrentScreenWidth, m_iCurrentScreenHeight );
	}

	// adjust the position of the window so that the game screen appear in the middle of the display
	int iDesktopWidth, iDesktopHeight;
	GetCurrentResolution( &iDesktopWidth, &iDesktopHeight );
*/
}


void GameWindowManager_Win32_GL::OnMainLoopFinished()
{
	SwapBuffers( m_hDC );
}


/*
void GameWindowManager_Win32_GL::SetWindowLeftTopCornerPosition( int left, int top )
{
	// set the position
	// - use SWP_NOSIZE flag to ignore window size parameters
	::SetWindowPos( m_hWnd, HWND_TOP, left, top, 0, 0, SWP_NOSIZE );
}
*/

/*
 *		This Code Was Created By Jeff Molofee 2000
 *		A HUGE Thanks To Fredric Echols For Cleaning Up
 *		And Optimizing The Base Code, Making It More Flexible!
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit My Site At nehe.gamedev.net
 */


bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
//LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc


/// Properly Kill The Window
Result::Name GameWindowManager_Win32_GL::KillGLWindow()
{
	if( m_CurrentScreenMode == GameWindow::FULLSCREEN )	// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (m_hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(m_hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		m_hRC = NULL;										// Set RC To NULL
	}

	if (m_hDC && !ReleaseDC(m_hWnd,m_hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		m_hDC=NULL;										// Set DC To NULL
	}

	if (m_hWnd && !DestroyWindow(m_hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release m_hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		m_hWnd=NULL;										// Set m_hWnd To NULL
	}

	if (!UnregisterClass(m_ClassName.c_str(),m_hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		m_hInstance=NULL;									// Set hInstance To NULL
	}

	return Result::SUCCESS;
}


} // namespace amorphous
