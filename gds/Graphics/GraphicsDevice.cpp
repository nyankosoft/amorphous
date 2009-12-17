#include "GraphicsDevice.hpp"
#include "Direct3D9.hpp"
#include "GLGraphicsDevice.hpp"


CSingleton<CGraphicsDeviceHolder> CGraphicsDeviceHolder::m_obj;


// TODO: make this a member of CGraphicsDevice
bool g_IsReadyToLoadGraphicsResources = false;


Result::Name CGraphicsDeviceHolder::SelectGraphicsDevice( const std::string& library_name )
{
	if( library_name == "Direct3D" )
	{
		m_pGraphicsDevice = &DIRECT3D9;
	}
	else if( library_name == "OpenGL" )
	{
		m_pGraphicsDevice = &GLGraphicsDevice();
	}
	else
	{
		LOG_PRINT_ERROR( " Unsupported graphics devicve: " + library_name );
		return Result::UNKNOWN_ERROR;
	}

	return Result::SUCCESS;
}
