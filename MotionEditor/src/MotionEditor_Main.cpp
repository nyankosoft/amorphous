//-----------------------------------------------------------------------------
// File: D3DAppTestMain.cpp
// NOTE: Press cancel on the open file dialog to compile and siplay test motion data
//-----------------------------------------------------------------------------

//#include <vld.h>

#include <boost/filesystem.hpp>
#include <gds/Input/StdKeyboard.hpp>
#include <gds/Graphics.hpp>
#include <gds/Support/lfs.hpp>
#include <gds/Support/Timer.hpp>
#include <gds/Support/CameraController_Win32.hpp>
#include <gds/Support/FileOpenDialog_Win32.hpp>
#include <gds/Support/memory_helpers.hpp>
#include <gds/Support/Profile.hpp>
#include <gds/Support/MiscAux.hpp>
#include <gds/XML/XMLDocumentLoader.hpp>
#include <gds/App/GameWindowManager.hpp>

#include <gds/MotionSynthesis/MotionDatabaseBuilder.hpp>
#include <gds/MotionSynthesis/BVHMotionDatabaseCompiler.hpp>

#include "MotionPrimitiveViewer.h"

using namespace std;
using namespace boost;
using namespace boost::filesystem;

//-----------------------------------------------------------------------------
// externs
//-----------------------------------------------------------------------------

void SetCurrentThreadAsRenderThread();


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

boost::shared_ptr<CMotionPrimitiveViewer> g_pMotionPrimitiveViewer;

CFont g_Font;

CPlatformDependentCameraController g_CameraController;

CTextureHandle g_DefaultTexture;

//CShaderManager g_ShaderManager;
//CShaderLightManager g_ShaderLightManager;

//float g_FOV = D3DX_PI / 4.0f;

//Vector3 g_vLightPosition;


CStdKeyboard g_StdKeyboardInput;


void ReleaseGraphicsResources()
{
	g_DefaultTexture.Release();
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	D3DXMATRIX matView;
	Matrix34 cam_pose = g_CameraController.GetPose();
	cam_pose.GetInverseROT().GetRowMajorMatrix44( matView );

	CCamera cam;
	cam.SetPose( g_CameraController.GetPose() );
	ShaderManagerHub.PushViewAndProjectionMatrices( cam );

	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // clear the backbuffer to a blue color
    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(32,32,32), 1.0f, 0 );

    // begin the scene
    pd3dDevice->BeginScene();

	if( g_pMotionPrimitiveViewer )
	{
		g_pMotionPrimitiveViewer->SetViewerPose( g_CameraController.GetPose() );
		g_pMotionPrimitiveViewer->Render();
	}

/*	if( CShader::Get()->GetCurrentShaderManager() )
	{
		g_ShaderManager.SetViewTransform( g_CameraController.GetCameraMatrix() );

		RenderMeshes();
	}
*/
	// rendering
	char acStr[256];
	sprintf( acStr, "%f", GlobalTimer().GetFPS() );
	g_Font.DrawText( acStr, Vector2(20,20), 0xFFFFFFFF );

    // end the scene
    pd3dDevice->EndScene();

    // present the backbuffer contents to the display
    pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


void Update( float dt )
{
	g_CameraController.UpdateCameraPose( dt );

//	g_CameraController.SetCameraMatrix();

	if( g_pMotionPrimitiveViewer )
		g_pMotionPrimitiveViewer->Update( dt );
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	g_CameraController.HandleMessage( msg, wParam, lParam );

	D3DXMATRIX matWorld, matProj;

	switch( msg )
	{
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;

		case WM_KEYDOWN:
			g_StdKeyboardInput.NotifyKeyDown( (int)wParam );

/*			switch( wParam )
			{
			case VK_ADD:
				g_FOV -= 0.01f;
				if( g_FOV <= 0.01f )
					g_FOV = 0.0f;
				D3DXMatrixPerspectiveFovLH( &matProj, g_FOV, 640.0f / 480.0f, 0.5f, 200.0f );
				DIRECT3D9.GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );
				g_ShaderManager.SetProjectionTransform( matProj );
				break;
			case VK_SUBTRACT:
				g_FOV += 0.01f;
				D3DXMatrixPerspectiveFovLH( &matProj, g_FOV, 640.0f / 480.0f, 0.5f, 200.0f );
				DIRECT3D9.GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );
				g_ShaderManager.SetProjectionTransform( matProj );
				break;
			}*/
			break;

		case WM_KEYUP:
			g_StdKeyboardInput.NotifyKeyUp( (int)wParam );
			break;

		default:
			break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


bool Init()
{
/*	if( !g_ShaderManager.LoadShaderFromFile( "StaticGeometryViewer.fx" ) )
		return false;

	CShader::Get()->SetShaderManager( &g_ShaderManager );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	g_ShaderManager.SetWorldTransform( matWorld );

    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 640.0f / 480.0f, 0.5f, 320.0f );
    DIRECT3D9.GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );
	g_ShaderManager.SetProjectionTransform( matProj );

	g_ShaderLightManager.Init();

	CHemisphericDirLight light;
	light.UpperColor.SetRGBA( 1.00f, 1.00f, 1.00f, 1.00f );
	light.LowerColor.SetRGBA( 0.25f, 0.25f, 0.25f, 1.00f );
	Vector3 vDir = Vector3(1.0f,-3.0f,1.5f);
	Vec3Normalize( vDir, vDir );
	light.vDirection = vDir;
	g_ShaderLightManager.SetLight( 0, light );

//	g_vLightPosition = - vDir * 20.0f;
//	g_vLightPosition = D3DXVECTOR3(0,30,0);
	g_vLightPosition = D3DXVECTOR3(-250,60,-500) * 500.0f;
	g_LensFlare.SetLightPosition( g_vLightPosition );

	DWORD dwColor = 0xFFF0F0F0;
//	DWORD dwColor = 0xFF8080FF;
	CTextureTool::CreateTexture( &dwColor, 1, 1, &g_DefaultTexture );
*/
	return true;
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, INT )
{
	string cmdline( lpCmdLine );

	const string init_wd = lfs::get_cwd();
	if( init_wd.rfind( "app" ) != init_wd.length() - 3 )
	{
		// change directory from the project file directory to the application directory
		lfs::set_wd( "../app" );
	}

	const std::string default_wd = lfs::get_cwd();

    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "D3D Test", NULL };

    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( "D3D Test", "D3D Application",
                              WS_OVERLAPPEDWINDOW, 100, 100, 1024, 768,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );


    // Show the window
    ShowWindow( hWnd, SW_SHOWDEFAULT );
    UpdateWindow( hWnd );

	// Initialize Direct3D
	if( !DIRECT3D9.InitD3D( hWnd ) )
		return 0;

	Vector3 vInitCamPos( 1.5f, 1.0f, -2.0f );
	g_CameraController.SetPose(
		Matrix34(
		vInitCamPos,
		CreateOrientFromFwdDir( Vec3GetNormalized(-vInitCamPos) )
		)
	);

	CLogOutput_HTML html_log( "MotionEditor_" + string(GetBuildInfo()) + ".html" );
	g_Log.AddLogOutput( &html_log );

	/// init the xml parser (calls Initialize() in ctor)
	CXMLParserInitReleaseManager xml_parser_mgr;

	// init graphics library (Direct3D/OpenGL)
	SelectGraphicsLibrary( "Direct3D" );

	SetCurrentThreadAsRenderThread();

	if( !Init() )
		return 0;

	shared_ptr<msynth::CMotionPrimitiveCompilerCreator> pCompilerCreator( new msynth::CBVHMotionPrimitiveCompilerCreator );
	msynth::RegisterMotionPrimitiveCompilerCreator( pCompilerCreator );

	msynth::CMotionDatabaseBuilder mdb;
	string input_filepath;
	if( 0 < cmdline.length() )
		input_filepath = cmdline;
	else
	{
		GetFilename( input_filepath );
		if( input_filepath.length() == 0 )
			input_filepath = "../resources/bvh/Mocappers/ordinary.xml";
	}

	string mdb_filepath;
	bool built = true;
	if( path(input_filepath).extension() == ".xml" )
	{
	//	string output_mdb_filepath = "../resources/mdb.bin";
		built = mdb.Build( input_filepath );

		bool saved = true;//mdb.SaveMotionDatabaseToFile( output_mdb_filepath );
	
		mdb_filepath = mdb.GetOutputFilepath();
	}
	else
		mdb_filepath = input_filepath;

	// set the working directory to the default for the application
	// because it may have been changed by user operations on FileOpenDialog.
	lfs::set_wd( default_wd );

	if( built )//&& saved )
	{
		// successfully built and saved a motion database
		g_pMotionPrimitiveViewer
			= boost::shared_ptr<CMotionPrimitiveViewer>( new CMotionPrimitiveViewer() );

		g_pMotionPrimitiveViewer->LoadMotionPrimitivesFromDatabase( mdb_filepath/*, tbl_name*/ );
	}

	// init font
//	g_Font.InitFont( "", 8, 16 );

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

			Update( GlobalTimer().GetFrameTime() );

			Render();

			Sleep( 5 );
		}
    }

	g_pMotionPrimitiveViewer.reset();


    // Clean up everything and exit the app
    UnregisterClass( "D3D Test", wc.hInstance );
    return 0;
}
