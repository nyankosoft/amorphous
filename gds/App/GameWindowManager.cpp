#include "GameWindowManager.hpp"
#include "GameWindowManager_Win32_GL.hpp"
#include "Graphics/GraphicsDevice.hpp"
#include "Graphics/OpenGL/GLInitialization.hpp"
#include "Support/Log/DefaultLog.hpp"

#ifdef BUILD_WITH_DIRECT3D
#include "GameWindowManager_Win32_D3D.hpp"
#include "Graphics/Direct3D/D3DInitialization.hpp"
#endif /* BUILD_WITH_DIRECT3D */


namespace amorphous
{

using namespace std;


static GameWindowManager *g_pGameWindowManager = NULL;


Result::Name SelectGraphicsLibrary_Win32( const std::string& graphics_library_name )
{
	if( graphics_library_name == "OpenGL" )
	{
		g_pGameWindowManager_Win32 = &GetGameWindowManager_Win32_GL();
		InitializeOpenGLClasses();
	}
#ifdef BUILD_WITH_DIRECT3D
	else if( graphics_library_name == "Direct3D" )
	{
		g_pGameWindowManager_Win32 = &GetGameWindowManager_Win32_D3D();
		InitializeDirect3DClasses();
	}
#endif /* BUILD_WITH_DIRECT3D */
	else
	{
		LOG_PRINT_ERROR( "Unsupported graphics library: " + graphics_library_name );
		g_pGameWindowManager_Win32 = NULL;

		return Result::INVALID_ARGS;
	}

	CGraphicsDeviceHolder::Get()->SelectGraphicsDevice( graphics_library_name );

//	PrimitiveRenderer().Init();

	g_pGameWindowManager = g_pGameWindowManager_Win32;

	return Result::SUCCESS;
}


Result::Name SelectGraphicsLibrary( const std::string& graphics_library_name )
{
#ifdef _MSC_VER
	return SelectGraphicsLibrary_Win32( graphics_library_name );
#else
//	return SelectGraphicsLibrary_???( graphics_library_name );
#endif
}


GameWindowManager& GetGameWindowManager()
{
	return *g_pGameWindowManager;
}


} // namespace amorphous
