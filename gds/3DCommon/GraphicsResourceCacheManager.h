

class CResourceCacheManager
{
	std::vector<boost::shared_ptr<CGraphicsResourceEntry>> m_vecpCache;

public:

	bool Preload( const std::string& xml_filename );

	/// returns a cached resource that matches the desc
	boost::shared_ptr<CGraphicsResourceEntry> GetCachedResource( const CGraphicsResourceDesc& desc );
};


bool CResourceCacheManager::Preload( const std::string& xml_filename )
{
	return true;
}


template<>
shared_ptr<CGraphicsResourceEntry>find_cached_resource( desc, std::vector<boost::shared_ptr<T>>& vecpEntry );
{
	for( size_t i=0; i<num; i++ )
	{
		if( .CanBeUsedAsCache( desc ) )
			return vecpEntry[i];
	}

	return shared_ptr<CGraphicsResourceEntry>();
}

shared_ptr<CGraphicsResourceEntry> CResourceCacheManager::GetCachedResource( const CGraphicsResourceDesc& desc )
{
	/// find preloaded resource that matches the description

	switch( desc.type )
	{
	case Texture: return find_cached_resource( desc, m_vecpTextureEntry );
	default: 
	}
	m_vecp
	.CanBeUsedAsCache();

	
}
