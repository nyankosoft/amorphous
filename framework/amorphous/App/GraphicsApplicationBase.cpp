#include "GraphicsApplicationBase.hpp"
#include "amorphous/App/GameWindowManager.hpp"
#include "amorphous/Support/Timer.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/MiscAux.hpp"
#include "amorphous/Support/CameraController.hpp"
#include "amorphous/Support/BitmapImage.hpp"
#include "amorphous/Support/WindowMisc_Win32.hpp"
#include "amorphous/Support/Log/DefaultLogAux.hpp"
#include "amorphous/Input/InputHub.hpp"
#include "amorphous/Graphics/Font/BuiltinFonts.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/ShaderManagerHub.hpp"
#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"


namespace amorphous
{

using namespace std;
using namespace boost;


static int sg_CameraControllerInputHandlerIndex = 1;
static int sg_InputHandlerIndex = 1;


GraphicsApplicationBase::GraphicsApplicationBase()
:
m_UseCameraController(true),
m_BackgroundColor( SFloatRGBAColor::Blue() )
{
	m_pCameraController.reset( new CameraController( sg_CameraControllerInputHandlerIndex ) );

	m_pInputHandler.reset( new CInputDataDelegate<GraphicsApplicationBase>( this ) );

	if( GetInputHub().GetInputHandler(sg_InputHandlerIndex) )
		GetInputHub().GetInputHandler(sg_InputHandlerIndex)->AddChild( m_pInputHandler.get() );
	else
		GetInputHub().PushInputHandler( sg_InputHandlerIndex, m_pInputHandler.get() );
}


GraphicsApplicationBase::~GraphicsApplicationBase()
{
	GetInputHub().RemoveInputHandler( sg_InputHandlerIndex, m_pInputHandler.get() );
	m_pCameraController.reset();
}


void GraphicsApplicationBase::UpdateCameraMatrices()
{
}


void GraphicsApplicationBase::UpdateFrame()
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

	GetGameWindowManager().OnMainLoopFinished();

	boost::this_thread::sleep( boost::posix_time::milliseconds( 2 ) );
}


void GraphicsApplicationBase::RenderBase()
{
	PROFILE_FUNCTION();

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
	GraphicsDevice().BeginScene();

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

	// end the scene
	GraphicsDevice().EndScene();

	// present the backbuffer contents to the display
	GraphicsDevice().Present();

	GetShaderManagerHub().PopViewAndProjectionMatrices();
}


void GraphicsApplicationBase::Run()
{
	const std::string app_title = GetApplicationTitle();

	// init xml parser
	// - released when leaving the scope
//	CXMLParserInitReleaseManager parser_mgr;


	string graphics_library_name = "OpenGL";
	ParamLoader param_loader( "config" );
	if( param_loader.IsReady() )
	{
		param_loader.LoadParam( "graphics_library", graphics_library_name );
		param_loader.CloseFile();
	}

	Result::Name res = SelectGraphicsLibrary( graphics_library_name );
	if( res != Result::SUCCESS )
		return;

	InitHTMLLog( app_title + "_" + string(GetBuildInfo()) + "_Log.html" );

	InitFreeImage();

	unsigned int res_x = 0, res_y = 0;
	GetCurrentPrimaryDisplayResolution( res_x, res_y );

	int w=0,h=0;
	if( 1920 < res_x )
	{
		// A rsolution higher than full HD; make the screen size full HD
		w = 1920;
		h = 1080;
	}
	else if( 1280 < res_x )
	{
		// A rsolution higher than HD 720; An HD 720-sized window should fit.
		w = 1280;
		h =  720;
	}
	else
	{
		// Probably an old display or a display on a small laptop; we go with XGA.
		w = 1024;
		h =  768;
	}

	m_Camera.SetAspectRatio( (float)w / (float)h );

	GameWindow::ScreenMode mode = GameWindow::WINDOWED;//g_pTest->GetFullscreen() ? GameWindow::FULLSCREEN : GameWindow::WINDOWED;
	GetGameWindowManager().CreateGameWindow( w, h, mode, app_title );

	m_WindowedModeResolution.width  = w;
	m_WindowedModeResolution.height = h;

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
		GlobalLog().Print( WL_WARNING, "exception: %s", e.what() );
	}


//	ChangeClientAreaSize( hWnd, g_pTest->GetWindowWidth(), g_pTest->GetWindowHeight() );

	ProfileInit();

	// init font
	m_pFont = CreateDefaultBuiltinFont();

	MainLoop( this );
}


void GraphicsApplicationBase::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case GIC_F11:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			ToggleScreenModes();
		}
		break;
	default:
		break;
	}

}


void GraphicsApplicationBase::ToggleScreenModes()
{
	if( GetGameWindowManager().IsFullscreen() )
	{
		if( m_WindowedModeResolution.width == 0 || m_WindowedModeResolution.height == 0 )
			return;

		// Changed to the windowed mode
		GetGameWindowManager().ChangeScreenSize( (int)m_WindowedModeResolution.width, (int)m_WindowedModeResolution.height, false );
	}
	else
	{
		m_WindowedModeResolution.width  = GetGameWindowManager().GetScreenWidth();
		m_WindowedModeResolution.height = GetGameWindowManager().GetScreenHeight();

		unsigned int width = 0, height = 0;

		GetCurrentPrimaryDisplayResolution( width, height );

		if( width == 0 || height == 0 )
			return;

		LOG_PRINTF(( "Switching to the fullscreen mode (%dx%d)", width, height ));

		// Changed to the fullscreen mode
		GetGameWindowManager().ChangeScreenSize( (int)width, (int)height, true );
	}
}


} // namespace amorphous
