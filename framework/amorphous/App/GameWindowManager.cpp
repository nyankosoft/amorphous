#include "GameWindowManager.hpp"

#ifdef _MSC_VER
#include "GameWindowManager_Win32.hpp"
#else
//#include "GameWindowManager_???.hpp"
#endif


namespace amorphous
{

using namespace std;


static GameWindowManager *g_pGameWindowManager = NULL;


Result::Name SelectGraphicsLibrary( const std::string& graphics_library_name )
{
#ifdef _MSC_VER
	return SelectGraphicsLibrary_Win32( graphics_library_name, g_pGameWindowManager );
#else
//	return SelectGraphicsLibrary_???( graphics_library_name );
#endif
}


GameWindowManager& GetGameWindowManager()
{
	return *g_pGameWindowManager;
}


} // namespace amorphous
