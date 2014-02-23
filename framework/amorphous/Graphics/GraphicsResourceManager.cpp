#include "Graphics/GraphicsResourceManager.hpp"
#include "Graphics/GraphicsResourceCacheManager.hpp"
#include "Graphics/GraphicsResourceHandle.hpp"
#include "Graphics/AsyncResourceLoader.hpp"
#include "Graphics/ResourceLoadingStateHolder.hpp"
#include "Graphics/GraphicsDevice.hpp"
#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{

using std::vector;
using namespace boost;


// draft
boost::thread::id sg_RenderThreadID;
boost::thread::id GetRenderThreadID() { return sg_RenderThreadID; }

static bool sg_bRenderThreadSpecified = false;
void SetCurrentThreadAsRenderThread()
{
	sg_bRenderThreadSpecified = true;
	sg_RenderThreadID = boost::this_thread::get_id();
}

static std::map< boost::thread::id, boost::shared_ptr<ResourceLoadingStateHolder> > sg_ThreadIDToLoadingStateHolder;
void CreateResourceLoadingStateHolderForCurrentThread()
{
	boost::shared_ptr<ResourceLoadingStateHolder> p( new ResourceLoadingStateHolder );
	sg_ThreadIDToLoadingStateHolder[boost::this_thread::get_id()] = p;
}


boost::shared_ptr<ResourceLoadingStateHolder> GetResourceLoadingStateHolderForCurrentThread()
{
	std::map<thread::id, shared_ptr<ResourceLoadingStateHolder> >::iterator itr
		= sg_ThreadIDToLoadingStateHolder.find( this_thread::get_id() );

	if( itr == sg_ThreadIDToLoadingStateHolder.end() )
		return shared_ptr<ResourceLoadingStateHolder>();
	else
		return itr->second;

}


ResourceLoadingStateSet::Name GetGraphicsResourceLoadingState()
{
	boost::shared_ptr<ResourceLoadingStateHolder> pHolder
		= GetResourceLoadingStateHolderForCurrentThread();

	if( !pHolder )
		return ResourceLoadingStateSet::NO_RESOURCE_LOADING_STATE_HOLDER;

	if( pHolder->AreAllResourceLoaded() )
		return ResourceLoadingStateSet::ALL_LOADED;
	else
		return ResourceLoadingStateSet::NOT_READY;
}


//==================================================================================================
// GraphicsResourceManager
//==================================================================================================

/// define the singleton instance
singleton<GraphicsResourceManager> GraphicsResourceManager::m_obj;


GraphicsResourceManager::GraphicsResourceManager()
{
	m_AsyncLoadingAllowed = true;

//	m_pCacheManager = shared_ptr<GraphicsResourceCacheManager>( new GraphicsResourceCacheManager() );

}


GraphicsResourceManager::~GraphicsResourceManager()
{
	Release();

//	GetAsyncResourceLoader().Release();
}


void GraphicsResourceManager::Release()
{
	ReleaseGraphicsResources();

	m_vecpResourceEntry.resize( 0 );
}


shared_ptr<GraphicsResourceEntry> GraphicsResourceManager::CreateGraphicsResourceEntry()
{
	shared_ptr<GraphicsResourceEntry> pResourceEntry
		= shared_ptr<GraphicsResourceEntry>( new GraphicsResourceEntry() );

	// set the state to reserved
	pResourceEntry->m_State = GraphicsResourceEntry::STATE_RESERVED;

	// add a created entry to a vacant slot

	const size_t num_entries = m_vecpResourceEntry.size();
	for( size_t i=0; i<num_entries; i++ )
	{
		if( !m_vecpResourceEntry[i]
		 || m_vecpResourceEntry[i]->GetState() == GraphicsResourceEntry::STATE_RELEASED )
		{
			m_vecpResourceEntry[i] = pResourceEntry;
			pResourceEntry->IncRefCount();
			return pResourceEntry;
		}
	}

	// create a new element
	m_vecpResourceEntry.push_back( pResourceEntry );

	pResourceEntry->IncRefCount();
	return pResourceEntry;
}


template<class T>
size_t add_weak_ptr_to_vacant_slot( weak_ptr<T> ptr, vector< weak_ptr<T> >& vecPtr )
{
	for( size_t i=0; i<vecPtr.size(); i++ )
	{
		if( !vecPtr[i].lock() )
		{
			vecPtr[i] = ptr;
			return i;
		}
	}

	// create a new element
	vecPtr.push_back( ptr );
	return (int)vecPtr.size() - 1;
}


template<class T>
inline int share_as_same_resource( const GraphicsResourceDesc& desc, const std::vector< boost::shared_ptr<T> >& vecPtr )
{
	const  size_t num_resources = vecPtr.size();
	for( size_t i=0; i<num_resources; i++ )
	{
		// check if the requested can be shared with one already registered to the graphics resource manager
		if( vecPtr[i]
		 && vecPtr[i]->GetState() == GraphicsResourceEntry::STATE_RESERVED // still alive
		 && vecPtr[i]->GetResource()
		 && vecPtr[i]->GetResource()->CanBeSharedAsSameResource( desc ) )
		{
			// requested resource was found in the list
			// - no need to add a new resource.
			// - resource gets loaded if reference count is incremented from 0 to 1
			vecPtr[i]->IncRefCount();
			return (int)i;
		}
	}

	return -1;
}


bool GraphicsResourceManager::ReleaseResourceEntry( boost::shared_ptr<GraphicsResourceEntry> ptr )
{
	if( ptr->GetResource() )
		ptr->GetResource()->ReleaseNonChachedResource();

	return true;

/*	const int index = ptr->GetIndex();

	if( 0 <= index )
	{
		m_vecpResourceEntry[index].reset();
		return true;
	}
	else
	{
		return false;
	}*/
}


shared_ptr<GraphicsResourceLoader> GraphicsResourceManager::CreateResourceLoader( shared_ptr<GraphicsResourceEntry> pEntry,
																				    const GraphicsResourceDesc& desc )
{
	boost::shared_ptr<GraphicsResourceLoader> pLoader;
	switch(desc.GetResourceType())
	{
	case GraphicsResourceType::Texture:
	{
		shared_ptr<DiskTextureLoader> pTexLoader( new DiskTextureLoader(pEntry,*dynamic_cast<const TextureResourceDesc *>(&desc)) );
		pTexLoader->SetWeakPtr( pTexLoader );
		pLoader = pTexLoader;
		break;
	}
	case GraphicsResourceType::Mesh:
	{
		shared_ptr<MeshLoader> pMeshLoader( new MeshLoader(pEntry,*dynamic_cast<const MeshResourceDesc *>(&desc)) );
		pMeshLoader->SetWeakPtr( pMeshLoader );
		pLoader = pMeshLoader;
		break;
	}
	case GraphicsResourceType::Shader:
		pLoader = shared_ptr<GraphicsResourceLoader>( new ShaderLoader(pEntry,*dynamic_cast<const ShaderResourceDesc *>(&desc)) );
	default:
		return pLoader;
	}

	return pLoader;
}


/// Not implemented yet.
/// - Just returns -1 to indicate that there are no sharable resoureces
shared_ptr<GraphicsResourceEntry> GraphicsResourceManager::FindSameLoadedResource( const GraphicsResourceDesc& desc )
{
	const size_t num_entries = m_vecpResourceEntry.size();
	for( size_t i=0; i<num_entries; i++ )
	{
		if( m_vecpResourceEntry[i]->GetState() == GraphicsResourceEntry::STATE_RESERVED
		 && m_vecpResourceEntry[i]->m_pDesc->ResourcePath == desc.ResourcePath )
		{
			return m_vecpResourceEntry[i];
		}
	}

	return shared_ptr<GraphicsResourceEntry>();
}


// async loading steps
// 1. see if the requested resource has already been loaded
//    true -> see if it is sharable
//      true -> share the resource.
//    false (not found) -> send a load request to the resource I/O thread
shared_ptr<GraphicsResourceEntry> GraphicsResourceManager::LoadAsync( const GraphicsResourceDesc& desc )
{
	if( !desc.IsValid() )
		return shared_ptr<GraphicsResourceEntry>();

	if( desc.IsDiskResource() )
	{
		shared_ptr<GraphicsResourceEntry> ptr;
		ptr = FindSameLoadedResource(desc);
		if( ptr )
			return ptr;
	}
	else
	{
		// non-disc resources are not sharable
		return shared_ptr<GraphicsResourceEntry>();
	}

	// create a new empty resource entry to determine the resource index now.
	// - reserves an entry index before loading the resource
	// - rationale: graphics resource handle needs resource index

	shared_ptr<GraphicsResourceEntry> pEntry = CreateGraphicsResourceEntry();

	if( pEntry )
	{
		// make as loading here to avoid accepting the async load requests more than once
		// - set to reserved in CreateGraphicsResourceEntry()
//		pEntry->m_State = GraphicsResourceEntry::;

		// save a copy of the desc
		pEntry->m_pDesc = desc.GetCopy();

		ResourceLoadRequest req( ResourceLoadRequest::LoadFromDisk, CreateResourceLoader(pEntry,desc), pEntry );
		GetAsyncResourceLoader().AddResourceLoadRequest( req );

		// register to the loading state holder
		if( desc.RegisterToLoadingStateHolder )
		{
			shared_ptr<ResourceLoadingStateHolder> pHolder
				= GetResourceLoadingStateHolderForCurrentThread();

			if( pHolder )
				pHolder->AddFromResourceEntry( pEntry );
		}

		return pEntry;
	}
	else
		return shared_ptr<GraphicsResourceEntry>();
}


shared_ptr<GraphicsResourceEntry> GraphicsResourceManager::LoadGraphicsResource( const GraphicsResourceDesc& desc )
{
	LOG_FUNCTION_SCOPE();

	LOG_PRINT_VERBOSE( "desc.ResourcePath: " + desc.ResourcePath );

	if( GraphicsDevice().GetState() != CGraphicsDevice::STATE_INITIALIZED )
	{
		LOG_PRINT_ERROR( "Not ready to load / create graphics resources. Check if a graphics device has been initialized." );
		return shared_ptr<GraphicsResourceEntry>();
	}

	if( !sg_bRenderThreadSpecified )
	{
		LOG_PRINT_WARNING( "Using GraphicsResourceManager without specifying render thread." );
	}

	if( boost::this_thread::get_id() != GetRenderThreadID() )
	{
		return LoadAsync( desc );
	}

//	if( desc.ResourcePath.length() == 0 )
	if( !desc.IsValid() )
	{
		LOG_PRINT_ERROR(( " An invalid resource desc (type: %s, resource path: %s).", GetGraphicsResourceTypeText(desc.GetResourceType()), desc.ResourcePath.c_str() ));
		return shared_ptr<GraphicsResourceEntry>();	// invalid filename
	}

	// search if a same resource has been already loaded or is being loaded
	int shared_index = -1;
	shared_index = share_as_same_resource( desc, m_vecpResourceEntry );

	// found a resource to share
	// just return the index to the entry
	if( 0 <= shared_index )
	{
		LOG_PRINTF_VERBOSE(( " Found a sharable resource (index: %d).", shared_index ));
		return m_vecpResourceEntry[shared_index];
	}

	// not found in the list - need to load as a new resource

	// create an empty entry
	shared_ptr<GraphicsResourceEntry> pResourceEntry = CreateGraphicsResourceEntry();

	if( !pResourceEntry )
	{
		LOG_PRINT_ERROR( " CreateGraphicsResourceEntry() failed." );
		return shared_ptr<GraphicsResourceEntry>();
	}

	// save copy of the resource desc
	pResourceEntry->m_pDesc = desc.GetCopy();

	shared_ptr<GraphicsResource> pResource = GetGraphicsResourceFactory().CreateGraphicsResource( desc );

	if( !pResource )
	{
		LOG_PRINTF_ERROR(( " Failed to create a graphics resource (type: %s, resource path: %s).", GetGraphicsResourceTypeText(desc.GetResourceType()), desc.ResourcePath.c_str() ));
		return shared_ptr<GraphicsResourceEntry>();
	}

	pResourceEntry->SetResource( pResource );

	// load

	bool loaded = pResource->Load();

	if( loaded )
	{
		// increment the reference count
		// - This is always an increment from 0 to 1
		// 5:11 PM 8/31/2008 Changed: done in CreateGraphicsResourceEntry()
//		pResourceEntry->IncRefCount();

		LOG_PRINT( " Created a graphics resource: " + desc.ResourcePath );

		if( desc.ResourcePath.find( "vest.png" ) != std::string::npos )
			int break_here = 1;

		// A resource has been successfully loaded
		return pResourceEntry;
	}
	else
	{
		LOG_PRINTF_WARNING(( " Failed to create a graphics resource (type: %s, resource path: %s).", GetGraphicsResourceTypeText(desc.GetResourceType()), desc.ResourcePath.c_str() ));

		// decrement ref count
		// - This will make the ref count zero and release the non-cachecd resource
		pResourceEntry->DecRefCount();

//		ReleaseResourceEntry( pResourceEntry );
		return shared_ptr<GraphicsResourceEntry>();	// failed to create a resource
	}
}


/// called from handle
shared_ptr<GraphicsResourceEntry> GraphicsResourceManager::LoadTexture( const TextureResourceDesc& desc )
{
	return LoadGraphicsResource( desc );
}


shared_ptr<GraphicsResourceEntry> GraphicsResourceManager::CreateTexture( const TextureResourceDesc& desc )
{
	return LoadGraphicsResource( desc );
}


/// called from handle
shared_ptr<GraphicsResourceEntry> GraphicsResourceManager::LoadMesh( const MeshResourceDesc& desc )
{
	return LoadGraphicsResource( desc );
}


/// called from handle
shared_ptr<GraphicsResourceEntry> GraphicsResourceManager::LoadShaderManager( const ShaderResourceDesc& desc )
{
	return LoadGraphicsResource( desc );
}


void GraphicsResourceManager::LoadGraphicsResources( const GraphicsParameters& rParam )
{
	LOG_FUNCTION_SCOPE();

	// load the resources
	// - Note that reference count is not changed
	size_t i, num_entries = m_vecpResourceEntry.size();
	for( i=0; i<num_entries; i++ )
	{
		shared_ptr<GraphicsResourceEntry> pEntry = m_vecpResourceEntry[i];
		if( pEntry
		 && pEntry->GetResource()
		 && !pEntry->GetResource()->IsCachedResource() ) // Do not load cached resources because it's a job of GraphicsResourceCacheManager
		{
			pEntry->GetResource()->Load();
		}
	}
}


void GraphicsResourceManager::AddCache( GraphicsResourceDesc& desc )
{
	if( m_pCacheManager )
		m_pCacheManager->AddCache( desc );
}

void GraphicsResourceManager::ReleaseGraphicsResources()
{
//	LOG_PRINT( "GraphicsResourceManager::ReleaseGraphicsResources()" );

	// load the resources
	// - Note that reference count is not changed
	size_t i, num_entries = m_vecpResourceEntry.size();
	for( i=0; i<num_entries; i++ )
	{
		shared_ptr<GraphicsResourceEntry> pEntry = m_vecpResourceEntry[i];
		if( pEntry
		 && pEntry->GetResource() )
		{
			pEntry->GetResource()->ReleaseNonChachedResource(); // Do not release cached resources
		}
	}
}


void GraphicsResourceManager::Refresh()
{
	LOG_FUNCTION_SCOPE();

	size_t i, num_entries = m_vecpResourceEntry.size();
	for( i=0; i<num_entries; i++ )
	{
		shared_ptr<GraphicsResourceEntry> pEntry = m_vecpResourceEntry[i];
		if( pEntry && pEntry->GetResource() )
			pEntry->GetResource()->Refresh();
	}
}


void GraphicsResourceManager::AllowAsyncLoading( bool allow )
{
	m_AsyncLoadingAllowed = allow;
}


void GraphicsResourceManager::GetStatus( GraphicsResourceType::Name type, std::string& dest_buffer )
{
	boost::mutex::scoped_lock scoped_lock(m_ResourceLock);

	size_t i, num_entries = m_vecpResourceEntry.size();
	dest_buffer = to_string(num_entries) + " resources in total\n";
	dest_buffer += "----------------------------------------------------------\n";

	for( i=0; i<num_entries; i++ )
	{
		shared_ptr<GraphicsResourceEntry> pEntry = m_vecpResourceEntry[i];
		if( pEntry )
			pEntry->GetStatus( dest_buffer );

		dest_buffer += "\n";
	}
}


void GraphicsResourceManager::GetStatus( GraphicsResourceType::Name type, std::vector<std::string>& dest_buffer )
{
	boost::mutex::scoped_lock scoped_lock(m_ResourceLock);

	dest_buffer.reserve( 0xFF );

	size_t i, num_entries = m_vecpResourceEntry.size();
	dest_buffer.push_back( to_string(num_entries) + " resources in total" );
	dest_buffer.push_back( "----------------------------------------------------------" );

	for( i=0; i<num_entries; i++ )
	{
		shared_ptr<GraphicsResourceEntry> pEntry = m_vecpResourceEntry[i];
		if( pEntry )
		{
			dest_buffer.push_back( std::string() );
			pEntry->GetStatus( dest_buffer.back() );
		}
	}
}






/*
int GraphicsResourceManager::LoadAsync( const GraphicsResourceDesc& desc )
{
	size_t i, num_entries = m_vecpResourceEntry.size();
	for( i=0; i<num_entries; i++ )
	{
//		m_vecpResourceCache[i]->;
	}
}
*/


} // namespace amorphous
