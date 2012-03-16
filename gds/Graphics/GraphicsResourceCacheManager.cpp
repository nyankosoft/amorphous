#include "GraphicsResourceCacheManager.hpp"
#include "GraphicsResources.hpp"


using boost::shared_ptr;


//===============================================================
// CGraphicsResourceFactory
//===============================================================

/// define the singleton instance
CSingleton<CGraphicsResourceFactory> CGraphicsResourceFactory::m_obj;


CGraphicsResourceFactory::CGraphicsResourceFactory()
{
}


CGraphicsResourceFactory::~CGraphicsResourceFactory()
{
}


void CGraphicsResourceFactory::Init( CGraphicsResourceFactoryImpl *pFactoryImpl )
{
	m_pImpl = shared_ptr<CGraphicsResourceFactoryImpl>( pFactoryImpl );
}


shared_ptr<CGraphicsResource> CGraphicsResourceFactory::CreateGraphicsResource( const CGraphicsResourceDesc &desc )
{
	switch(desc.GetResourceType())
	{
	case GraphicsResourceType::Texture: return CreateTextureResource( *dynamic_cast<const CTextureResourceDesc*>(&desc) );
	case GraphicsResourceType::Mesh:    return CreateMeshResource( *dynamic_cast<const CMeshResourceDesc*>(&desc) );
	case GraphicsResourceType::Shader:  return CreateShaderResource( *dynamic_cast<const CShaderResourceDesc*>(&desc) );
	default:
		return shared_ptr<CGraphicsResource>();
	}
}


//===============================================================
// CGraphicsResourceCacheManager
//===============================================================

/// define the singleton instance
CSingleton<CGraphicsResourceCacheManager> CGraphicsResourceCacheManager::m_obj;


bool CGraphicsResourceCacheManager::Preload( const std::string& xml_filename )
{
	return true;
}


void CGraphicsResourceCacheManager::AddCache( CGraphicsResourceDesc& desc )
{
	// create as a cached resource
	desc.m_IsCachedResource = true;

	shared_ptr<CGraphicsResource> ptr = GraphicsResourceFactory().CreateGraphicsResource( desc );

//	ptr->IsCachedResource = true;

	// create an empty texture / mesh
	ptr->Create();

	m_vecpResurceCache.push_back( ptr );
}


/// find preloaded cached resource that best matches the argument description
shared_ptr<CGraphicsResource> CGraphicsResourceCacheManager::GetCachedResource( const CGraphicsResourceDesc& desc )
{
	const int num_resources = (int)m_vecpResurceCache.size();
	int max_score = 0;
	int max_score_resource_index = -1;
	for( int i=0; i<num_resources; i++ )
	{
		shared_ptr<CGraphicsResource> pResource = m_vecpResurceCache[i];
		int score = pResource->CanBeUsedAsCache( desc );
		if( max_score < score )
		{
			max_score = score;
			max_score_resource_index = i;
		}
	}

	if( 0 <= max_score_resource_index )
	{
		// found a resource that can be used as cache
		return m_vecpResurceCache[ max_score_resource_index ];
	}

	return shared_ptr<CGraphicsResource>();
}


void CGraphicsResourceCacheManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	const size_t num_resources = m_vecpResurceCache.size();
	for( size_t i=0; i<num_resources; i++ )
	{
		m_vecpResurceCache[i]->Load();
	}
}


// Release all the cached resources
void CGraphicsResourceCacheManager::ReleaseGraphicsResources()
{
	const size_t num_resources = m_vecpResurceCache.size();
	for( size_t i=0; i<num_resources; i++ )
	{
		m_vecpResurceCache[i]->ReleaseCachedResource();
	}
}
