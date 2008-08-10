#include "MeshObjectHandle.h"
#include "GraphicsResourceManager.h"

#include "Support/Log/DefaultLog.h"

using namespace std;


//==================================================================================================
// CMeshObjectHandle
//==================================================================================================


void CMeshObjectHandle::IncResourceRefCount()
{
	if( 0 <= m_EntryID )
		GraphicsResourceManager().GetMeshEntry(m_EntryID).IncRefCount();
}


void CMeshObjectHandle::DecResourceRefCount()
{
	if( 0 <= m_EntryID )
		GraphicsResourceManager().GetMeshEntry(m_EntryID).DecRefCount();
}


CMeshType::Name CMeshObjectHandle::GetMeshType() const
{
	if( 0 <= m_EntryID )
		return GraphicsResourceManager().GetMeshEntry(m_EntryID).GetMeshType();
	else
		return CMeshType::INVALID; // TODO: return a value that means invalid request
}


bool CMeshObjectHandle::Load( const std::string& resource_path )
{
	CMeshResourceDesc desc;
	desc.ResourcePath = resource_path;
	return Load( desc );
}


bool CMeshObjectHandle::Load( const CMeshResourceDesc& desc )
{
	// if currently holding a mesh, release it
	Release();

	if( desc.LoadingMode == CResourceLoadingMode::SYNCHRONOUS )
	{
		m_EntryID = GraphicsResourceManager().LoadMesh( desc );
	}
	else
	{
		m_EntryID = GraphicsResourceManager().LoadAsync( desc );
	}

	if( 0 <= m_EntryID )
		return true;
	else
		return false;
}



/*
bool CMeshObjectHandle::LoadAsync( int priority )
{
	CMeshResourceDesc desc;
	desc.Filename = filename;


	return true;
}
*/
/*
void CMeshObjectHandle::Serialize( IArchive& ar, const unsigned int version )
{
	CGraphicsResourceHandle::Serialize( ar, version );
}
*/