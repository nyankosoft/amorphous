#include "GraphicsResourceHandle.hpp"
#include "GraphicsResourceEntries.hpp"


namespace amorphous
{

using namespace std;
using namespace boost;


GraphicsResourceState::Name GraphicsResourceHandle::GetEntryState()
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
void GraphicsResourceHandle::IncResourceRefCount()
{
	if( GetEntry() )
        GetEntry()->IncRefCount();
}


void GraphicsResourceHandle::DecResourceRefCount()
{
	if( GetEntry() )
        GetEntry()->DecRefCount();
}
*/


} // namespace amorphous
