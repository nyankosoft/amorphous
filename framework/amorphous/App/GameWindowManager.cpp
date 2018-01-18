#include "GameWindowManager.hpp"

#ifdef _MSC_VER
#include "GameWindowManager_Win32.hpp"
#else
#include "GameWindowManager_Generic.hpp"
#endif

#include "amorphous/Graphics/GraphicsComponentCollector.hpp"
#include "amorphous/Graphics/2DPrimitive/2DPrimitiveRenderer.hpp"


namespace amorphous
{

using namespace std;


static GameWindowManager *g_pGameWindowManager = nullptr;
static std::string sg_GraphicsLibraryName;


Result::Name SelectGraphicsLibrary( const std::string& graphics_library_name )
{
#ifdef _MSC_VER
	Result::Name res = SelectGraphicsLibrary_Win32( graphics_library_name, g_pGameWindowManager );
#else
	Result::Name res = SelectGraphicsLibrary_Generic( graphics_library_name, g_pGameWindowManager );
#endif

	if( res == Result::SUCCESS )
		sg_GraphicsLibraryName = graphics_library_name;

	return res;
}

const std::string& GetGraphicsLibraryName()
{
	return sg_GraphicsLibraryName;
}

GameWindowManager& GetGameWindowManager()
{
	return *g_pGameWindowManager;
}

bool GameWindowManager::Init( int screen_width, int screen_height, GameWindow::ScreenMode screen_mode )
{
	// save width and height
	m_iCurrentScreenWidth  = screen_width;
	m_iCurrentScreenHeight = screen_height;

	// update graphics params
	GraphicsParameters params = GraphicsComponentCollector::Get()->GetGraphicsParams();
	params.ScreenWidth  = screen_width;
	params.ScreenHeight = screen_height;
	params.bWindowed = ( screen_mode == GameWindow::WINDOWED );
	GraphicsComponentCollector::Get()->SetGraphicsPargams( params );

	// init 2d primitive renderer
	Get2DPrimitiveRenderer().Init();

	return true;
}

} // namespace amorphous
