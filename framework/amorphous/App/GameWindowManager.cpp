#include "GameWindowManager.hpp"

#ifdef _MSC_VER
#include "GameWindowManager_Win32.hpp"
#else
//#include "GameWindowManager_???.hpp"
#endif


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
//	Result::Name res = return SelectGraphicsLibrary_???( graphics_library_name );
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


} // namespace amorphous
