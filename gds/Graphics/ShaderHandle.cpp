#include "ShaderHandle.hpp"
#include "GraphicsResourceManager.hpp"
#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;


//==================================================================================================
// ShaderHandle
//==================================================================================================

const ShaderHandle ShaderHandle::ms_NullHandle;


bool ShaderHandle::Load( const ShaderResourceDesc& desc )
{
	Release();

	// TODO: support asynchronous loading
	if( true /*desc.LoadingMode == CResourceLoadingMode::SYNCHRONOUS*/ )
	{
		m_pResourceEntry = GetGraphicsResourceManager().LoadShaderManager( desc );
	}
	else
	{
		m_pResourceEntry = GetGraphicsResourceManager().LoadAsync( desc );
	}

	if( m_pResourceEntry )
		return true;	// the resource has been successfully loaded
	else
		return false;	// the loading failed - this is mostly because the texture file was not found
}


bool ShaderHandle::Load( const std::string& resource_path )
{
	ShaderResourceDesc desc;
	desc.ResourcePath = resource_path;
	return Load( desc );
}


ShaderManager *ShaderHandle::GetShaderManager()
{
	if( GetEntry()
	 && GetEntry()->GetShaderResource() )
	{
		return GetEntry()->GetShaderResource()->GetShaderManager();
	}
	else
		return NULL;
}


} // namespace amorphous
