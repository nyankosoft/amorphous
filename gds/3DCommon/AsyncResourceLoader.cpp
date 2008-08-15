#include "AsyncResourceLoader.h"
#include <boost/thread/xtime.hpp>

using namespace std;
using namespace boost;


class Result
{
public:
	enum Name
	{
		SUCCESS = 0,
		RESOURCE_NOT_FOUND,
		RESOURCE_IN_USE,
		NUM_RESULTS
	};
};


CSingleton<CAsyncResourceLoader> CAsyncResourceLoader::m_obj;


CAsyncResourceLoader::CAsyncResourceLoader()
:
m_bEndIOThread(false)
{
	if( !GraphicsResourceManager().IsAsyncLoadingAllowed() )
	{
		// asynchronous loading is turned off
		return;
	}

	// create resource IO thread, which
	// - loads resources (textures, meshes, etc.) from disk to memory
	// - copy resources loaded on memory to locked graphics resource memory
	m_pIOThread = shared_ptr<thread>( new thread( CIOThreadStarter(this) ) );
}


/// Release the resource IO thread
void CAsyncResourceLoader::Release()
{
	if( m_pIOThread )
	{
		m_bEndIOThread = true;
		m_pIOThread->join();
		m_pIOThread.reset();
	}
}


bool CAsyncResourceLoader::AddResourceLoadRequest( const CResourceLoadRequest& req )
{
	mutex::scoped_lock scoped_lock(m_IOMutex);

	m_ResourceLoadRequestQueue.push( req );

	return true;
}


bool CAsyncResourceLoader::AddGraphicsDeviceRequest( const CGraphicsDeviceRequest& req )
{
	mutex::scoped_lock scoped_lock(m_GraphicsDeviceMutex);

	m_GraphicsDeviceRequestQueue.push( req );

	return true;
}


void CAsyncResourceLoader::IOThreadMain()
{
	ProcessResourceLoadRequests();
}


/// called by file IO thread
void CAsyncResourceLoader::ProcessResourceLoadRequests()
{
	bool copied = false;
	bool res = false;
//	Result::Name res;
	shared_ptr<CGraphicsResourceLoader> pLoader;
	CResourceLoadRequest req( CResourceLoadRequest::LoadFromDisk, shared_ptr<CGraphicsResourceLoader>(), weak_ptr<CGraphicsResourceEntry>() );

	boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC);
	xt.sec += 1; // 1 [sec]

	while( !m_bEndIOThread )
	{
		boost::thread::sleep(xt);

		if( 0 < m_ResourceLoadRequestQueue.size() )
		{
			mutex::scoped_lock scoped_lock(m_IOMutex);
			req = m_ResourceLoadRequestQueue.front();
			m_ResourceLoadRequestQueue.pop();
		}
		else
			continue;

		switch( req.GetRequestType() )
		{
		case CResourceLoadRequest::LoadFromDisk:
			pLoader = req.m_pLoader;
			// load the resource from the disk
/*			res = pLoader->LoadFromDisk();
			switch(res)
			{
			case LoadResult::SUCCESS:
				break;
			case LoadResult::RESOURCE_NOT_FOUND:
				continue;
				break;
			case LoadResult::RESOURCE_IN_USE:
				m_ResourceLoadRequestQueue.push( req );
				continue;
				// try again later
				break;
			default:
				break;
			}*/

			// load the resource from disk or memory
			// - texture and mesh resources
			//   -> loaded from the disk
			// - sub resources of the mesh (i.e., vertices, indices, etc.)
			//   -> loaded from mesh archive on memory
			res = pLoader->Load();

			if( res )
			{
				// Fills out resource desc (texture & mesh)
				// Creates a lock request (texture)
				// Create load requests for sub resources (mesh)
				pLoader->OnLoadingCompleted( pLoader );

				// loaded the resource
				// - send lock request to render thread in order to
				//   copy the loaded resource to some graphics memory
//				mutex::scoped_lock scoped_lock(m_GraphicsDeviceMutex);
//				m_GraphicsDeviceRequestQueue.push(
//					CGraphicsDeviceRequest( CGraphicsDeviceRequest::Lock, req.m_pLoader, req.m_pResourceEntry )
//					);
			}
			else
			{
				int failed_to_load_a_resource = 1;
			}
			break;

		case CResourceLoadRequest::CopyToGraphicsMemory:
			// copy loaded data to locked graphics memory
			copied = req.m_pLoader->CopyLoadedContentToGraphicsResource();
			if( copied )
			{
				AddGraphicsDeviceRequest( 
					CGraphicsDeviceRequest( CGraphicsDeviceRequest::Unlock, req.m_pLoader, req.m_pResourceEntry.lock() )
					);
			}
			
			break;

		default:
			break;
		}
	}
}


void CAsyncResourceLoader::ProcessGraphicsDeviceRequests()
{
	bool resource_locked = false;
	shared_ptr<CGraphicsResourceEntry> pSrcEntry, pEntry;
	CGraphicsDeviceRequest req( CGraphicsDeviceRequest::Lock, shared_ptr<CGraphicsResourceLoader>(), weak_ptr<CGraphicsResourceEntry>() );

	for( int i=0; i<1; i++ )
	{
		{
			mutex::scoped_lock scoped_lock(m_GraphicsDeviceMutex);

			if( m_GraphicsDeviceRequestQueue.size() == 0 )
				break; // no request to process

			req = m_GraphicsDeviceRequestQueue.front();
			m_GraphicsDeviceRequestQueue.pop();
		}

		switch( req.m_RequestType )
		{
		case CGraphicsDeviceRequest::Lock:
			{
				// Do either of the following
				// - draw one from the cache (no need to create a new resource on memory)
				// - create one from the desc
				req.m_pLoader->AcquireResource();

				bool locked = req.m_pLoader->Lock();

				// find a resource entry that matches the resource description

//				pSrcEntry = req.m_pResourceEntry.lock();
//				if( !pSrcEntry )
//					continue;

				// texture / mesh sizes are stored in the loader
				// - GraphicsResourceManager can determine the graphics resource entry that matches the specification.
				//   i.e., It can find whether a cached resource is available
				//   Therefore, GraphicsResourceManager does either of the following,
				//   - Creates a new graphics resource entry.
				//   - Draws one from the cache.
				// - This will overwrite the slot that has been occupied by pSrcEntry. i.e., ref count of pSrcEntry
				//   will be zero after leaving this scope
/*				pEntry = GraphicsResourceManager().CreateAt( pSrcEntry->GetDesc(), pSrcEntry->GetIndex() );
				if( pEntry )
				{
					// update the entry
					req.m_pResourceEntry = pEntry;

					// refresh the entry held by the loader
					// - Should separate resource entry class and resource class to avoid this?
					req.m_pLoader->SetEntry( pEntry );

//					resource_locked = pEntry->Lock(); // Does not work for the mesh, since it has 3 buffers to lock - VB, IB, and attribute buffer

					req.m_pLoader->Lock();
				}*/

				// mark the entry as locked

				// graphics resource memory has been locked and it's ready to receive data loaded from the disk
				// - add a copy request to IO queue
				if( locked )
				{
					AddResourceLoadRequest(
						CResourceLoadRequest( CResourceLoadRequest::CopyToGraphicsMemory, req.m_pLoader, pEntry )
					);
				}

				// save the weak/shared ptr to the entry
//				desc/req.pLockedEntry = ;
			}
			break;

		case CGraphicsDeviceRequest::Unlock:
			{
				bool unlocked = req.m_pLoader->Unlock();
/*				pEntry = req.m_pResourceEntry.lock();
				if( pEntry )
					pEntry->Unlock();
*/
				if( unlocked )
				{
					if( pEntry->GetResource() )
						pEntry->GetResource()->SetState( GraphicsResourceState::LOADED );
				}
			}
			break;
		}
	}
}
