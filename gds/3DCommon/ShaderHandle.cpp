#include "ShaderHandle.h"

#include "Support/Log/DefaultLog.h"

using namespace std;


//==================================================================================================
// CShaderHandle
//==================================================================================================

const CShaderHandle CShaderHandle::ms_NullHandle;

bool CShaderHandle::Load()
{
	Release();

	m_EntryID = GraphicsResourceManager.LoadShaderManager( filename );

	if( m_EntryID == -1 )
		return false;	// the loading failed - this is mostly because the texture file was not found
	else
		return true;	// the texture has been successfully loaded
}


CShaderManager *CShaderHandle::GetShaderManager()
{
	return GraphicsResourceManager.GetShaderManager( m_EntryID );
}
