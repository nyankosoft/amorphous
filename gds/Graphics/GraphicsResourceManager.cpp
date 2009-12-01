#include "Graphics/GraphicsResourceManager.hpp"
#include "Graphics/GraphicsResourceCacheManager.hpp"
#include "Graphics/GraphicsResourceHandle.hpp"
#include "Graphics/AsyncResourceLoader.hpp"
#include "Graphics/ResourceLoadingStateHolder.hpp"
#include "Graphics/Direct3D9.hpp"

#include "Support/Log/DefaultLog.hpp"
#include "Support/SafeDelete.hpp"
#include "Support/SafeDeleteVector.hpp"

#include "Support/Serialization/BinaryDatabase.hpp"

using namespace std;
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

static std::map< boost::thread::id, boost::shared_ptr<CResourceLoadingStateHolder> > sg_ThreadIDToLoadingStateHolder;
void CreateResourceLoadingStateHolderForCurrentThread()
{
	boost::shared_ptr<CResourceLoadingStateHolder> p( new CResourceLoadingStateHolder );
	sg_ThreadIDToLoadingStateHolder[boost::this_thread::get_id()] = p;
}


boost::shared_ptr<CResourceLoadingStateHolder> GetResourceLoadingStateHolderForCurrentThread()
{
	using namespace std;
	using namespace boost;

	map<thread::id, shared_ptr<CResourceLoadingStateHolder> >::iterator itr
		= sg_ThreadIDToLoadingStateHolder.find( this_thread::get_id() );

	if( itr == sg_ThreadIDToLoadingStateHolder.end() )
		return shared_ptr<CResourceLoadingStateHolder>();
	else
		return itr->second;

}


CResourceLoadingStateSet::Name GetGraphicsResourceLoadingState()
{
	boost::shared_ptr<CResourceLoadingStateHolder> pHolder
		= GetResourceLoadingStateHolderForCurrentThread();

	if( !pHolder )
		return CResourceLoadingStateSet::NO_RESOURCE_LOADING_STATE_HOLDER;

	if( pHolder->AreAllResourceLoaded() )
		return CResourceLoadingStateSet::ALL_LOADED;
	else
		return CResourceLoadingStateSet::NOT_READY;
}


//==================================================================================================
// CGraphicsResourceManager
//==================================================================================================

/// define the singleton instance
CSingleton<CGraphicsResourceManager> CGraphicsResourceManager::m_obj;


CGraphicsResourceManager::CGraphicsResourceManager()
{
	m_AsyncLoadingAllowed = true;

//	m_pCacheManager = shared_ptr<CGraphicsResourceCacheManager>( new CGraphicsResourceCacheManager() );

}


CGraphicsResourceManager::~CGraphicsResourceManager()
{
	Release();

//	AsyncResourceLoader().Release();
}


void CGraphicsResourceManager::Release()
{
	ReleaseGraphicsResources();

	m_vecpResourceEntry.resize( 0 );
}


shared_ptr<CGraphicsResourceEntry> CGraphicsResourceManager::CreateGraphicsResourceEntry()
{
	shared_ptr<CGraphicsResourceEntry> pResourceEntry
		= shared_ptr<CGraphicsResourceEntry>( new CGraphicsResourceEntry() );

	// set the state to reserved
	pResourceEntry->m_State = CGraphicsResourceEntry::STATE_RESERVED;

	// add a created entry to a vacant slot

	const size_t num_entries = m_vecpResourceEntry.size();
	for( size_t i=0; i<num_entries; i++ )
	{
		if( !m_vecpResourceEntry[i]
		 || m_vecpResourceEntry[i]->GetState() == CGraphicsResourceEntry::STATE_RELEASED )
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
size_t add_weak_ptr_to_vacant_slot( weak_ptr<T> ptr, vector<weak_ptr<T>>& vecPtr )
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
inline int share_as_same_resource( const CGraphicsResourceDesc& desc, const std::vector<boost::shared_ptr<T>>& vecPtr )
{
	const  size_t num_resources = vecPtr.size();
	for( size_t i=0; i<num_resources; i++ )
	{
		// check if the requested can be shared with one already registered to the graphics resource manager
		if( vecPtr[i]
		 && vecPtr[i]->GetState() == CGraphicsResourceEntry::STATE_RESERVED // still alive
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


bool CGraphicsResourceManager::ReleaseResourceEntry( boost::shared_ptr<CGraphicsResourceEntry> ptr )
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


shared_ptr<CGraphicsResourceLoader> CGraphicsResourceManager::CreateResourceLoader( shared_ptr<CGraphicsResourceEntry> pEntry,
																				    const CGraphicsResourceDesc& desc )
{
	boost::shared_ptr<CGraphicsResourceLoader> pLoader;
	switch(desc.GetResourceType())
	{
	case GraphicsResourceType::Texture:
		pLoader = shared_ptr<CGraphicsResourceLoader>( new CDiskTextureLoader(pEntry,*dynamic_cast<const CTextureResourceDesc *>(&desc)) );
		break;
	case GraphicsResourceType::Mesh:
	{
		shared_ptr<CMeshLoader> pMeshLoader( new CMeshLoader(pEntry,*dynamic_cast<const CMeshResourceDesc *>(&desc)) );
		pMeshLoader->SetWeakPtr( pMeshLoader );
		pLoader = pMeshLoader;
		break;
	}
	case GraphicsResourceType::Shader:
		pLoader = shared_ptr<CGraphicsResourceLoader>( new CShaderLoader(pEntry,*dynamic_cast<const CShaderResourceDesc *>(&desc)) );
	default:
		return pLoader;
	}

	return pLoader;
}


/// Not implemented yet.
/// - Just returns -1 to indicate that there are no sharable resoureces
shared_ptr<CGraphicsResourceEntry> CGraphicsResourceManager::FindSameLoadedResource( const CGraphicsResourceDesc& desc )
{
	const size_t num_entries = m_vecpResourceEntry.size();
	for( size_t i=0; i<num_entries; i++ )
	{
		if( m_vecpResourceEntry[i]->GetState() == CGraphicsResourceEntry::STATE_RESERVED
		 && m_vecpResourceEntry[i]->m_pDesc->ResourcePath == desc.ResourcePath )
		{
			return m_vecpResourceEntry[i];
		}
	}

	return shared_ptr<CGraphicsResourceEntry>();
}


// async loading steps
// 1. see if the requested resource has already been loaded
//    true -> see if it is sharable
//      true -> share the resource.
//    false (not found) -> send a load request to the resource I/O thread
shared_ptr<CGraphicsResourceEntry> CGraphicsResourceManager::LoadAsync( const CGraphicsResourceDesc& desc )
{
	if( !desc.IsValid() )
		return shared_ptr<CGraphicsResourceEntry>();

	if( desc.IsDiskResource() )
	{
		shared_ptr<CGraphicsResourceEntry> ptr;
		ptr = FindSameLoadedResource(desc);
		if( ptr )
			return ptr;
	}
	else
	{
		// non-disc resources are not sharable
		return shared_ptr<CGraphicsResourceEntry>();
	}

	// create a new empty resource entry to determine the resource index now.
	// - reserves an entry index before loading the resource
	// - rationale: graphics resource handle needs resource index

	shared_ptr<CGraphicsResourceEntry> pEntry = CreateGraphicsResourceEntry();

	if( pEntry )
	{
		// make as loading here to avoid accepting the async load requests more than once
		// - set to reserved in CreateGraphicsResourceEntry()
//		pEntry->m_State = CGraphicsResourceEntry::;

		// save a copy of the desc
		pEntry->m_pDesc = desc.GetCopy();

		CResourceLoadRequest req( CResourceLoadRequest::LoadFromDisk, CreateResourceLoader(pEntry,desc), pEntry );
		AsyncResourceLoader().AddResourceLoadRequest( req );

		// register to the loading state holder
		if( desc.RegisterToLoadingStateHolder )
		{
			shared_ptr<CResourceLoadingStateHolder> pHolder
				= GetResourceLoadingStateHolderForCurrentThread();

			if( pHolder )
				pHolder->AddFromResourceEntry( pEntry );
		}

		return pEntry;
	}
	else
		return shared_ptr<CGraphicsResourceEntry>();
}


shared_ptr<CGraphicsResourceEntry> CGraphicsResourceManager::LoadGraphicsResource( const CGraphicsResourceDesc& desc )
{
	LOG_FUNCTION_SCOPE();

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
		return shared_ptr<CGraphicsResourceEntry>();	// invalid filename

	// search if a same resource has been already loaded or is being loaded
	int shared_index = -1;
	shared_index = share_as_same_resource( desc, m_vecpResourceEntry );

	// found a resource to share
	// just return the index to the entry
	if( 0 <= shared_index )
		return m_vecpResourceEntry[shared_index];

	// not found in the list - need to load as a new resource

	// create an empty entry
	shared_ptr<CGraphicsResourceEntry> pResourceEntry = CreateGraphicsResourceEntry();

	if( !pResourceEntry )
		return shared_ptr<CGraphicsResourceEntry>();

	// save copy of the resource desc
	pResourceEntry->m_pDesc = desc.GetCopy();

	shared_ptr<CGraphicsResource> pResource = GraphicsResourceFactory().CreateGraphicsResource( desc );

	if( !pResource )
	{
		LOG_PRINT_ERROR( " Failed to create a graphics resource." );
		return shared_ptr<CGraphicsResourceEntry>();
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

		// A resource has been successfully loaded
		return pResourceEntry;
	}
	else
	{
		LOG_PRINT_WARNING( " Failed to create a graphics resource: " + desc.ResourcePath );

		// decrement ref count
		// - This will make the ref count zero and release the non-cachecd resource
		pResourceEntry->DecRefCount();

//		ReleaseResourceEntry( pResourceEntry );
		return shared_ptr<CGraphicsResourceEntry>();	// failed to create a resource
	}
}


/// called from handle
shared_ptr<CGraphicsResourceEntry> CGraphicsResourceManager::LoadTexture( const CTextureResourceDesc& desc )
{
	return LoadGraphicsResource( desc );
}


shared_ptr<CGraphicsResourceEntry> CGraphicsResourceManager::CreateTexture( const CTextureResourceDesc& desc )
{
	return LoadGraphicsResource( desc );
}


/// called from handle
shared_ptr<CGraphicsResourceEntry> CGraphicsResourceManager::LoadMesh( const CMeshResourceDesc& desc )
{
	return LoadGraphicsResource( desc );
}


/// called from handle
shared_ptr<CGraphicsResourceEntry> CGraphicsResourceManager::LoadShaderManager( std::string filename )
{
	CShaderResourceDesc desc;
	desc.ResourcePath = filename;
	return LoadGraphicsResource( desc );
}


void CGraphicsResourceManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	LOG_FUNCTION_SCOPE();

	// load the resources
	// - Note that reference count is not changed
	size_t i, num_entries = m_vecpResourceEntry.size();
	for( i=0; i<num_entries; i++ )
	{
		shared_ptr<CGraphicsResourceEntry> pEntry = m_vecpResourceEntry[i];
		if( pEntry
		 && pEntry->GetResource()
		 && !pEntry->GetResource()->IsCachedResource() ) // Do not load cached resources because it's a job of CGraphicsResourceCacheManager
		{
			pEntry->GetResource()->Load();
		}
	}
}


void CGraphicsResourceManager::AddCache( CGraphicsResourceDesc& desc )
{
	if( m_pCacheManager )
		m_pCacheManager->AddCache( desc );
}

void CGraphicsResourceManager::ReleaseGraphicsResources()
{
//	g_Log.Print( "CGraphicsResourceManager::ReleaseGraphicsResources()" );

	// load the resources
	// - Note that reference count is not changed
	size_t i, num_entries = m_vecpResourceEntry.size();
	for( i=0; i<num_entries; i++ )
	{
		shared_ptr<CGraphicsResourceEntry> pEntry = m_vecpResourceEntry[i];
		if( pEntry
		 && pEntry->GetResource() )
		{
			pEntry->GetResource()->ReleaseNonChachedResource(); // Do not release cached resources
		}
	}
}


void CGraphicsResourceManager::Refresh()
{
	LOG_FUNCTION_SCOPE();

	size_t i, num_entries = m_vecpResourceEntry.size();
	for( i=0; i<num_entries; i++ )
	{
		shared_ptr<CGraphicsResourceEntry> pEntry = m_vecpResourceEntry[i];
		if( pEntry && pEntry->GetResource() )
			pEntry->GetResource()->Refresh();
	}
}


void CGraphicsResourceManager::AllowAsyncLoading( bool allow )
{
	m_AsyncLoadingAllowed = allow;
}


void CGraphicsResourceManager::GetStatus( GraphicsResourceType::Name type, char *pDestBuffer )
{
	boost::mutex::scoped_lock scoped_lock(m_ResourceLock);

	size_t i, num_entries = m_vecpResourceEntry.size();
	char buffer[1024];
	sprintf( pDestBuffer, "%d resources in total\n", num_entries );
	strcat(  pDestBuffer, "----------------------------------------------------------\n" );

	for( i=0; i<num_entries; i++ )
	{
		shared_ptr<CGraphicsResourceEntry> pEntry = m_vecpResourceEntry[i];
		if( pEntry )
			pEntry->GetStatus( buffer );

		strcat( pDestBuffer, buffer );
		strcat( pDestBuffer, "\n" );
	}
}






/*
int CGraphicsResourceManager::LoadAsync( const CGraphicsResourceDesc& desc )
{
	size_t i, num_entries = m_vecpResourceEntry.size();
	for( i=0; i<num_entries; i++ )
	{
//		m_vecpResourceCache[i]->;
	}
}
*/
