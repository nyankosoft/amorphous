#include "AsyncResourceLoader.hpp"
#include "Support/Profile.hpp"
#include "../base.hpp"
#include <boost/thread/xtime.hpp>

using namespace std;
using namespace boost;


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


CAsyncResourceLoader::~CAsyncResourceLoader()
{
	Release();
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


void CAsyncResourceLoader::ProcessResourceLoadRequest()
{
	bool copied = false;
	Result::Name res = Result::SUCCESS;
	shared_ptr<CGraphicsResourceLoader> pLoader;
	CResourceLoadRequest req( CResourceLoadRequest::LoadFromDisk, shared_ptr<CGraphicsResourceLoader>(), weak_ptr<CGraphicsResourceEntry>() );

	if( 0 < m_ResourceLoadRequestQueue.size() )
	{
		mutex::scoped_lock scoped_lock(m_IOMutex);
		req = m_ResourceLoadRequestQueue.front();
		m_ResourceLoadRequestQueue.pop();
	}
	else
		return;

	switch( req.GetRequestType() )
	{
	case CResourceLoadRequest::LoadFromDisk:
		pLoader = req.m_pLoader;

		// load the resource from disk or memory
		// - texture and mesh resources
		//   -> loaded from the disk
		// - sub resources of the mesh (i.e., vertices, indices, etc.)
		//   -> loaded from mesh archive on memory
		res = pLoader->Load();

		switch(res)
		{
		case Result::SUCCESS:
			// Fills out resource desc (texture & mesh)
			// Creates a lock request (texture)
			// Create load requests for sub resources (mesh)
			// - Load subresources and send lock requests for each subresource (mesh)
			pLoader->OnLoadingCompleted( pLoader );
			break;

		case Result::RESOURCE_NOT_FOUND:
//			continue;
			break;

		case Result::RESOURCE_IN_USE:
			// try again later
			{
				mutex::scoped_lock scoped_lock(m_IOMutex);
				m_ResourceLoadRequestQueue.push( req );
			}
//			continue;
			break;

		default:
			break;
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
		else
		{
			int failed_to_copy_a_resource_to_locked_graphics_memory = 1;
		}
		
		break;

	default:
		break;
	}
}


/// called by file IO thread
void CAsyncResourceLoader::ProcessResourceLoadRequests()
{
	boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC);
	xt.sec += 1; // 1 [sec]

	while( !m_bEndIOThread )
	{
		boost::thread::sleep(xt);

		ProcessResourceLoadRequest();
	}
}


void CAsyncResourceLoader::ProcessGraphicsDeviceRequests()
{
	PROFILE_FUNCTION();

	bool resource_locked = false;
	shared_ptr<CGraphicsResourceEntry> pSrcEntry, pEntry;
	CGraphicsDeviceRequest req( CGraphicsDeviceRequest::Lock, shared_ptr<CGraphicsResourceLoader>(), weak_ptr<CGraphicsResourceEntry>() );
	bool res = false;

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
				if( !req.m_pLoader->IsReadyToLock() )
				{
					AddGraphicsDeviceRequest( req );
					continue;
				}

				// Do either of the following
				// - Draw one from the cache (no need to create a new resource on memory)
				// - Create a new graphics resource from the desc
				req.m_pLoader->AcquireResource();

				if( req.m_pLoader->GetResource() )
					req.m_pLoader->GetResource()->SetState( GraphicsResourceState::LOADING_ASYNCHRONOUSLY );

				bool locked = req.m_pLoader->Lock();

				// find a resource entry that matches the resource description

				// texture / mesh sizes are stored in the loader
				// - GraphicsResourceManager can determine the graphics resource entry that matches the specification.
				//   i.e., It can find whether a cached resource is available
				//   Therefore, GraphicsResourceManager does either of the following,
				//   - Creates a new graphics resource entry.
				//   - Draws one from the cache.

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

				if( unlocked )
				{
					// Sleep( 2000 );

					req.m_pLoader->OnResourceLoadedOnGraphicsMemory();

//					if( req.m_pLoader->GetResource() )
//						req.m_pLoader->GetResource()->SetState( GraphicsResourceState::LOADED );
				}
				else
					LOG_PRINT_ERROR( "Failed to unlock a resource: " + req.m_pLoader->GetDesc()->ResourcePath );
			}
			break;

		case CGraphicsDeviceRequest::LoadToGraphicsMemoryByRenderThread:
			res = req.m_pLoader->LoadToGraphicsMemoryByRenderThread();
			if( res = false )
			{
				LOG_PRINT_ERROR( "req.m_pLoader->LoadToGraphicsMemoryByRenderThread() failed. Resource: " + req.m_pLoader->GetDesc()->ResourcePath );
			}
			break;
		}
	}
}
