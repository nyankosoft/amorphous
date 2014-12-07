#include "DemoSwitcher.hpp"
#include "GraphicsTestBase.hpp"
#include "DemoFactory.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/App/GameWindowManager.hpp"
#include "amorphous/Support/ParamLoader.hpp"

using std::string;
//using namespace amorphous;


ApplicationBase *amorphous::CreateApplicationInstance() { return new DemoSwitcher; }


int DemoSwitcher::Init()
{
	boost::shared_ptr<DemoFactory> pDemoFactory( CreateDemoFactory() );
	SetDemoFactory( pDemoFactory );

	InitDemo();

	return 0;
}


void DemoSwitcher::Update( float dt )
{
	if( m_pDemo )
	{
		if( m_pDemo->UseCameraControl() )
			EnableCameraControl();
		else
			DisableCameraControl();

		m_pDemo->Update( dt );
	}
}


void DemoSwitcher::Render()
{
	if( !m_pDemo )
		return;

	// Applied at the next Render() call
	SetBackgroundColor( m_pDemo->GetBackgroundColor() );

	// Why does this cause an error?
//	const Camera &camera = this->Camera();

	m_pDemo->UpdateViewTransform( Camera().GetCameraMatrix() );
	m_pDemo->UpdateProjectionTransform( Camera().GetProjectionMatrix() );

	m_pDemo->Render();

	if( m_DisplayDebugInfo )
		m_pDemo->DisplayDebugInfo();
}


void DemoSwitcher::HandleInput( const InputData& input )
{
	if( m_pDemo )
		m_pDemo->HandleInput( input );

	switch( input.iGICode )
	{
	case GIC_F1:
		if( input.iType == ITYPE_KEY_PRESSED )
			m_DisplayDebugInfo = !m_DisplayDebugInfo;
		break;

	case GIC_PAGE_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
			NextDemo();
		break;

	case GIC_PAGE_UP:
		if( input.iType == ITYPE_KEY_PRESSED )
			PrevDemo();
		break;

	default:
		GraphicsApplicationBase::HandleInput( input );
	}
}


CGraphicsTestBase *DemoSwitcher::CreateDemoInstance( unsigned int index )
{
	if( m_pDemoFactory )
		return m_pDemoFactory->CreateDemoInstance( index );
	else
		return NULL;
//	return CreateTestInstance( GetDemoNames()[index] );
}


bool DemoSwitcher::InitDemo( int index )
{
	if( index < 0 )
		return false;

	// Create the instance of the test class
	m_pDemo.reset( CreateDemoInstance(index) );
	if( !m_pDemo )
		return false;

	m_pDemo->SetCameraController( GetCameraController() );

	const std::string app_title = m_pDemo->GetAppTitle();
	const std::string app_class_name = app_title;

	static bool window_created = false;
	string window_title = app_title;
	window_title += " (" + GetGraphicsLibraryName() + ")";
	GetGameWindowManager().SetWindowTitleText( window_title );
//	if( !window_created )
//	{
//		int w = m_pDemo->GetWindowWidth();  // 1280;
//		int h = m_pDemo->GetWindowHeight(); //  720;
//		LoadParamFromFile( "config", "screen_resolution", w, h );
//		GameWindow::ScreenMode mode = GameWindow::WINDOWED;//m_pDemo->GetFullscreen() ? GameWindow::FULLSCREEN : GameWindow::WINDOWED;
//		GetGameWindowManager().CreateGameWindow( w, h, mode, window_title );
//		Camera().SetAspectRatio( (float)w / (float)h );
//
//		window_created = true;
//	}
//	else
//	{
//		GetGameWindowManager().SetWindowTitleText( window_title );
//	}

	m_pDemo->InitBase();

	m_pDemo->Init();

//	g_pInputHandler.reset( new CGraphicsTestInputHandler(m_pDemo) );

//	g_pCameraController.reset( new CameraController( sg_CameraControllerInputHandlerIndex ) );

	return true;
}


bool DemoSwitcher::InitDemo()
{
	if( !m_pDemoFactory )
		return false;

	// Load the demo name from file
	std::string demo_name;
	LoadParamFromFile( "params.txt", "demo", demo_name );
	for( unsigned int i=0; i<m_pDemoFactory->GetNumDemos(); i++ )
	{
		if( m_pDemoFactory->GetDemoNames()[i] == demo_name )
		{
			m_DemoIndex = i;
			break;
		}
	}

	return InitDemo( m_DemoIndex );
}


void DemoSwitcher::NextDemo()
{
	if( !m_pDemoFactory )
		return;

	m_DemoIndex = (m_DemoIndex + 1) % m_pDemoFactory->GetNumDemos();

	InitDemo( m_DemoIndex );
}


void DemoSwitcher::PrevDemo()
{
	if( !m_pDemoFactory )
		return;

	m_DemoIndex = (m_DemoIndex + m_pDemoFactory->GetNumDemos() - 1) % m_pDemoFactory->GetNumDemos();

	InitDemo( m_DemoIndex );
}
