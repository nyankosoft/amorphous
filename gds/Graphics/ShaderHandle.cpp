#include "ShaderHandle.hpp"
#include "Support/Log/DefaultLog.hpp"

using namespace std;


//==================================================================================================
// CShaderHandle
//==================================================================================================

const CShaderHandle CShaderHandle::ms_NullHandle;


bool CShaderHandle::Load( const CShaderResourceDesc& desc )
{
	Release();

	// TODO: support asynchronous loading
	if( true /*desc.LoadingMode == CResourceLoadingMode::SYNCHRONOUS*/ )
	{
		m_pResourceEntry = GraphicsResourceManager().LoadShaderManager( desc );
	}
	else
	{
		m_pResourceEntry = GraphicsResourceManager().LoadAsync( desc );
	}

	if( m_pResourceEntry )
		return true;	// the resource has been successfully loaded
	else
		return false;	// the loading failed - this is mostly because the texture file was not found
}


bool CShaderHandle::Load( const std::string& resource_path )
{
	CShaderResourceDesc desc;
	desc.ResourcePath = resource_path;
	return Load( desc );
}


CShaderManager *CShaderHandle::GetShaderManager()
{
	if( GetEntry()
	 && GetEntry()->GetShaderResource() )
	{
		return GetEntry()->GetShaderResource()->GetShaderManager();
	}
	else
		return NULL;
}
