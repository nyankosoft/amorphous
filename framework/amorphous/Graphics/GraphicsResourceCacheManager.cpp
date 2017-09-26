#include "GraphicsResourceCacheManager.hpp"
#include "GraphicsResources.hpp"


namespace amorphous
{


using namespace std;


//===============================================================
// GraphicsResourceFactory
//===============================================================

/// define the singleton instance
singleton<GraphicsResourceFactory> GraphicsResourceFactory::m_obj;


GraphicsResourceFactory::GraphicsResourceFactory()
{
}


GraphicsResourceFactory::~GraphicsResourceFactory()
{
}


void GraphicsResourceFactory::Init( GraphicsResourceFactoryImpl *pFactoryImpl )
{
	m_pImpl = shared_ptr<GraphicsResourceFactoryImpl>( pFactoryImpl );
}


shared_ptr<GraphicsResource> GraphicsResourceFactory::CreateGraphicsResource( const GraphicsResourceDesc &desc )
{
	switch(desc.GetResourceType())
	{
	case GraphicsResourceType::Texture: return CreateTextureResource( *dynamic_cast<const TextureResourceDesc*>(&desc) );
	case GraphicsResourceType::Mesh:    return CreateMeshResource( *dynamic_cast<const MeshResourceDesc*>(&desc) );
	case GraphicsResourceType::Shader:  return CreateShaderResource( *dynamic_cast<const ShaderResourceDesc*>(&desc) );
	default:
		LOG_PRINT_ERROR( "An unsupported resource type" );
		return shared_ptr<GraphicsResource>();
	}
}


//===============================================================
// GraphicsResourceCacheManager
//===============================================================

/// define the singleton instance
singleton<GraphicsResourceCacheManager> GraphicsResourceCacheManager::m_obj;


bool GraphicsResourceCacheManager::Preload( const std::string& xml_filename )
{
	return true;
}


void GraphicsResourceCacheManager::AddCache( GraphicsResourceDesc& desc )
{
	// create as a cached resource
	desc.m_IsCachedResource = true;

	shared_ptr<GraphicsResource> ptr = GraphicsResourceFactory().CreateGraphicsResource( desc );

	if( !ptr )
	{
		LOG_PRINT_ERROR( "Failed to add a cached resource." );
		return;
	}

//	ptr->IsCachedResource = true;

	// create an empty texture / mesh
	ptr->Create();

	m_vecpResurceCache.push_back( ptr );
}


/// find preloaded cached resource that best matches the argument description
shared_ptr<GraphicsResource> GraphicsResourceCacheManager::GetCachedResource( const GraphicsResourceDesc& desc )
{
	const int num_resources = (int)m_vecpResurceCache.size();
	int max_score = 0;
	int max_score_resource_index = -1;
	for( int i=0; i<num_resources; i++ )
	{
		shared_ptr<GraphicsResource> pResource = m_vecpResurceCache[i];
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

	return shared_ptr<GraphicsResource>();
}


void GraphicsResourceCacheManager::LoadGraphicsResources( const GraphicsParameters& rParam )
{
	const size_t num_resources = m_vecpResurceCache.size();
	for( size_t i=0; i<num_resources; i++ )
	{
		m_vecpResurceCache[i]->Load();
	}
}


// Release all the cached resources
void GraphicsResourceCacheManager::ReleaseGraphicsResources()
{
	const size_t num_resources = m_vecpResurceCache.size();
	for( size_t i=0; i<num_resources; i++ )
	{
		m_vecpResurceCache[i]->ReleaseCachedResource();
	}
}


} // namespace amorphous
