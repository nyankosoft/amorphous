#include "GraphicsResourceHandle.hpp"
#include "GraphicsResourceEntries.hpp"

using namespace std;
using namespace boost;


GraphicsResourceState::Name CGraphicsResourceHandle::GetEntryState()
{
	if( GetEntry() )
	{
		if( GetEntry()->GetResource() )
			return GetEntry()->GetResource()->GetState();
		else
		{
			if( GetEntry()->GetState() == CGraphicsResourceEntry::STATE_RESERVED )
				return GraphicsResourceState::LOADING_ASYNCHRONOUSLY;
			else
				return GraphicsResourceState::RELEASED;
		}
	}
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
