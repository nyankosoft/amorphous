#include "AsyncResourceLoader.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/base.hpp"


namespace amorphous
{

using namespace std;


singleton<AsyncResourceLoader> AsyncResourceLoader::m_obj;


AsyncResourceLoader::AsyncResourceLoader()
:
m_bEndIOThread(false)
{
	if( !GetGraphicsResourceManager().IsAsyncLoadingAllowed() )
	{
		// asynchronous loading is turned off
		return;
	}

	// create resource IO thread, which
	// - loads resources (textures, meshes, etc.) from disk to memory
	// - copy resources loaded on memory to locked graphics resource memory
	m_pIOThread = shared_ptr<thread>( new thread( CIOThreadStarter(this) ) );
}


AsyncResourceLoader::~AsyncResourceLoader()
{
	Release();
}


/// Release the resource IO thread
void AsyncResourceLoader::Release()
{
	if( m_pIOThread )
	{
		m_bEndIOThread = true;
		m_pIOThread->join();
		m_pIOThread.reset();
	}
}


bool AsyncResourceLoader::AddResourceLoadRequest( const ResourceLoadRequest& req )
{
	std::lock_guard<std::mutex> lock(m_IOMutex);

	m_ResourceLoadRequestQueue.push( req );

	return true;
}


bool AsyncResourceLoader::AddGraphicsDeviceRequest( const CGraphicsDeviceRequest& req )
{
	std::lock_guard<std::mutex> lock(m_GraphicsDeviceMutex);

	m_GraphicsDeviceRequestQueue.push( req );

	return true;
}


void AsyncResourceLoader::IOThreadMain()
{
	ProcessResourceLoadRequests();
}


void AsyncResourceLoader::ProcessResourceLoadRequest()
{
	bool copied = false;
	Result::Name res = Result::SUCCESS;
	shared_ptr<GraphicsResourceLoader> pLoader;
	ResourceLoadRequest req( ResourceLoadRequest::LoadFromDisk, shared_ptr<GraphicsResourceLoader>(), weak_ptr<GraphicsResourceEntry>() );

	if( 0 < m_ResourceLoadRequestQueue.size() )
	{
		std::lock_guard<std::mutex> lock(m_IOMutex);
		req = m_ResourceLoadRequestQueue.front();
		m_ResourceLoadRequestQueue.pop();
	}
	else
		return;

	switch( req.GetRequestType() )
	{
	case ResourceLoadRequest::LoadFromDisk:
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
				std::lock_guard<std::mutex> lock(m_IOMutex);
				m_ResourceLoadRequestQueue.push( req );
			}
//			continue;
			break;

		default:
			break;
		}
		break;

	case ResourceLoadRequest::CopyToGraphicsMemory:
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
void AsyncResourceLoader::ProcessResourceLoadRequests()
{
	auto seconds_to_sleep = 1s; // 1 [sec]

	while( !m_bEndIOThread )
	{
		std::this_thread::sleep_for(seconds_to_sleep);

		ProcessResourceLoadRequest();
	}
}


void AsyncResourceLoader::ProcessGraphicsDeviceRequests()
{
	PROFILE_FUNCTION();

	bool resource_locked = false;
	shared_ptr<GraphicsResourceEntry> pSrcEntry, pEntry;
	CGraphicsDeviceRequest req( CGraphicsDeviceRequest::Lock, shared_ptr<GraphicsResourceLoader>(), weak_ptr<GraphicsResourceEntry>() );
	bool res = false;

	for( int i=0; i<1; i++ )
	{
		{
			std::lock_guard<std::mutex> lock(m_GraphicsDeviceMutex);

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
						ResourceLoadRequest( ResourceLoadRequest::CopyToGraphicsMemory, req.m_pLoader, pEntry )
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


} // namespace amorphous
