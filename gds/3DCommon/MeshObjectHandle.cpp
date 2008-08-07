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


bool CMeshObjectHandle::Load()
{
	// if currently holding a mesh, release it
	Release();

	m_EntryID = GraphicsResourceManager().LoadMeshObject( filename, m_MeshType );

	if( 0 <= m_EntryID )
		return true;
	else
		return false;
}


bool CMeshObjectHandle::LoadAsync( int priority )
{
	CMeshResourceDesc desc;
	desc.Filename = filename;

	m_EntryID = GraphicsResourceManager().LoadAsync( desc );

	return true;
}


void CMeshObjectHandle::Serialize( IArchive& ar, const unsigned int version )
{
	CGraphicsResourceHandle::Serialize( ar, version );

	ar & m_MeshType;
}
