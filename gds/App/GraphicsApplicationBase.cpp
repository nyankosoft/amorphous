#include "GraphicsApplicationBase.hpp"
#include "gds/App/GameWindowManager_Win32.hpp"
#include "gds/Support/Timer.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/Profile.hpp"
#include "gds/Support/MiscAux.hpp"
#include "gds/Support/CameraController.hpp"
#include "gds/Support/BitmapImage.hpp"
#include "gds/Input/InputHub.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Graphics/Shader/ShaderManager.hpp"
#include "gds/Graphics/Shader/ShaderManagerHub.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "gds/Graphics/Direct3D/Direct3D9.hpp"


namespace amorphous
{

using namespace std;
using namespace boost;


static int sg_CameraControllerInputHandlerIndex = 1;
static int sg_InputHandlerIndex = 1;


CGraphicsApplicationBase::CGraphicsApplicationBase()
:
m_UseCameraController(true),
m_BackgroundColor( SFloatRGBAColor::Blue() )
{
	m_pCameraController.reset( new CameraController( sg_CameraControllerInputHandlerIndex ) );

	m_pInputHandler.reset( new CInputDataDelegate<CGraphicsApplicationBase>( this ) );

	if( GetInputHub().GetInputHandler(sg_InputHandlerIndex) )
		GetInputHub().GetInputHandler(sg_InputHandlerIndex)->AddChild( m_pInputHandler.get() );
	else
		GetInputHub().PushInputHandler( sg_InputHandlerIndex, m_pInputHandler.get() );
}


CGraphicsApplicationBase::~CGraphicsApplicationBase()
{
	GetInputHub().RemoveInputHandler( sg_InputHandlerIndex, m_pInputHandler.get() );
	m_pCameraController.reset();
}


void CGraphicsApplicationBase::UpdateCameraMatrices()
{
}


void CGraphicsApplicationBase::UpdateFrame()
{
	GlobalTimer().UpdateFrameTime();

	m_pCameraController->UpdateCameraPose( GlobalTimer().GetFrameTime() );

	UpdateCameraMatrices();

	float frametime = GlobalTimer().GetFrameTime();
	if( 0.1f < frametime )
		frametime = 0.1f;

	Update( frametime );

	if( m_UseCameraController )
		m_Camera.SetPose( m_pCameraController->GetPose() );

//	if( m_UseRenderBase )
//		g_pTest->RenderBase();
//	else
		RenderBase();

	ProfileDumpOutputToBuffer();

	GameWindowManager_Win32().OnMainLoopFinished();

	boost::this_thread::sleep( boost::posix_time::milliseconds( 2 ) );
}


void CGraphicsApplicationBase::RenderBase()
{
	PROFILE_FUNCTION();

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );

//	g_Camera.SetPose( g_CameraController.GetPose() );

	// Set the camera and projection tansforms to
	// 1. fixed function pipeline
	// 2. currently loaded shaders
	GetShaderManagerHub().PushViewAndProjectionMatrices( m_Camera );

	// Commented out: done by ShaderManagerHub::PushViewAndProjectionMatrices()
//	FixedFunctionPipelineManager().SetViewTransform( m_Camera.GetCameraMatrix() );
//	FixedFunctionPipelineManager().SetProjectionTransform( m_Camera.GetProjectionMatrix() );

	Matrix44 mat;
	m_Camera.GetCameraMatrix( mat );
//	UpdateViewTransform( mat );
	m_Camera.GetProjectionMatrix( mat );
//	UpdateProjectionTransform( mat );

	GraphicsDevice().SetRenderState( RenderStateType::ALPHA_BLEND, true );
	GraphicsDevice().SetSourceBlendMode( AlphaBlend::SrcAlpha );
	GraphicsDevice().SetDestBlendMode( AlphaBlend::InvSrcAlpha );

    // clear the backbuffer to a blue color
//	GraphicsDevice().SetClearColor( g_pTest->GetBackgroundColor() );
//	GraphicsDevice().SetClearDepth( 1.0f );
//	GraphicsDevice().Clear()
	const SFloatRGBAColor& bg_color = m_BackgroundColor;
	GraphicsDevice().SetClearColor( m_BackgroundColor );
	GraphicsDevice().SetClearDepth( 1.0f );
	GraphicsDevice().Clear( BufferMask::COLOR | BufferMask::DEPTH );

//	g_pTest->RenderScene();

    // begin the scene
	if( pd3dDevice )
		pd3dDevice->BeginScene();

	Render();

	// display FPS
	m_pFont->DrawText( to_string(GlobalTimer().GetFPS()).c_str(), Vector2(20,20), 0xFFFFFFFF );

	int i=0;
	const vector<string>& vecProfileResults = GetProfileText();
	for( size_t i=0; i<vecProfileResults.size(); i++ )
	{
		const string& text = vecProfileResults[i];
//		g_pFont->DrawText( text.c_str(), Vector2( 20, 40 + i*16 ), 0xF0F0F0FF );
	}

	if( pd3dDevice )
	{
		// end the scene
		pd3dDevice->EndScene();

		// present the backbuffer contents to the display
		pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	GetShaderManagerHub().PopViewAndProjectionMatrices();
}


void CGraphicsApplicationBase::Run()
{
	const std::string app_title = GetApplicationTitle();

//	g_CmdLine = lpCmdLine;
/*
	const string iwd = lfs::get_cwd(); // initial working directory
	if( iwd.substr( iwd.length() - 4 ) != "/_App"
	 && iwd.substr( iwd.length() - 4 ) != "\\_App" )
	{
		// working directory is not set to the directory which contains the application binary
//		chdir( "../../../_App" );
		chdir( "../app" );
	}*/

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
		param_loader.CloseFile();
	}

	Result::Name res = SelectGraphicsLibrary( graphics_library_name );
	if( res != Result::SUCCESS )
		return;

	CLogOutput_HTML html_log( app_title + "_" + string(GetBuildInfo()) + "_Log.html" );
	g_Log.AddLogOutput( &html_log );

	InitFreeImage();

	// Create the instance of the test class
//	g_pTest = boost::shared_ptr<CGraphicsTestBase>( CreateTestInstance() );

	int w = 1024;//GetWindowWidth();
	int h = 768;//GetWindowHeight();
	GameWindow::ScreenMode mode = GameWindow::WINDOWED;//g_pTest->GetFullscreen() ? GameWindow::FULLSCREEN : GameWindow::WINDOWED;
	GameWindowManager().CreateGameWindow( w, h, mode, app_title );

	try
	{
		if( Init() != 0 )
		{
			LOG_PRINT_ERROR( " Init() failed." );
			return;
		}
	}
	catch( std::exception& e )
	{
		g_Log.Print( WL_WARNING, "exception: %s", e.what() );
	}


//	ChangeClientAreaSize( hWnd, g_pTest->GetWindowWidth(), g_pTest->GetWindowHeight() );

	ProfileInit();

	// init font
	m_pFont = CreateDefaultBuiltinFont();

	MainLoop( this );
}


} // namespace amorphous
