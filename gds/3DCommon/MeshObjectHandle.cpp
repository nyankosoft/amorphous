#include "MeshObjectHandle.h"
#include "GraphicsResourceManager.h"

#include "Support/Log/DefaultLog.h"

using namespace std;


//==================================================================================================
// CMeshObjectHandle
//==================================================================================================

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


void CMeshObjectHandle::Serialize( IArchive& ar, const unsigned int version )
{
	CGraphicsResourceHandle::Serialize( ar, version );

	ar & m_MeshType;
}
