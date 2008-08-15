#include "GraphicsResourceHandle.h"
#include "GraphicsResourceEntries.h"

//#include "Support/Log/DefaultLog.h"
#include "Support/Serialization/Serialization.h"

using namespace std;
using namespace boost;


GraphicsResourceState::Name CGraphicsResourceHandle::GetEntryState()
{
	if( GetEntry()
	 && GetEntry()->GetResource() )
		return GetEntry()->GetResource()->GetState();
	else
		return GraphicsResourceState::RELEASED;
}

/*
void CGraphicsResourceHandle::IncResourceRefCount()
{
	if( GetEntry() )
        GetEntry()->IncRefCount();
}


void CGraphicsResourceHandle::DecResourceRefCount()
{
	if( GetEntry() )
        GetEntry()->DecRefCount();
}
*/
