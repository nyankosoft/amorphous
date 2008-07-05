#include "GraphicsResourceHandle.h"
#include "GraphicsResourceManager.h"

//#include "Support/Log/DefaultLog.h"
#include "Support/Serialization/Serialization.h"

using namespace std;

/*
void CGraphicsResourceHandle::IncResourceRefCount()
{
	if( 0 <= m_EntryID )
        GraphicsResourceManager().IncResourceRefCount( *this );
}


void CGraphicsResourceHandle::DecResourceRefCount()
{
	if( 0 <= m_EntryID )
        GraphicsResourceManager().DecResourceRefCount( *this );
}
*/

void CGraphicsResourceHandle::Serialize( IArchive& ar, const unsigned int version )
{
	ar & filename;

//	if( ar.GetMode() == IArchive::MODE_INPUT )
//		Release();
}


bool CGraphicsResourceHandle::LoadAsync( int priority )
{
	if(  GraphicsResourceManager().IsAsyncLoadingAllowed() )
	{
//		ResourceLoadRequest req;
//		req.name = filename;
//		req.type = GetResourceType();
//		m_EntryID = GraphicsResourceManager().LoadAsync( desc );
	}
	else
		Load();

	return true;
}
