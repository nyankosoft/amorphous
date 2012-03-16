#ifndef  __GraphicsResourceCacheManager_H__
#define  __GraphicsResourceCacheManager_H__


#include <vector>
#include <queue>
#include "GraphicsResourceDescs.hpp"
#include "GraphicsComponentCollector.hpp"

#include "../Support/Singleton.hpp"
using namespace NS_KGL;


//=======================================================================================
// CGraphicsResourceFactoryImpl
//=======================================================================================

class CGraphicsResourceFactoryImpl
{
public:

	CGraphicsResourceFactoryImpl() {}

	virtual ~CGraphicsResourceFactoryImpl() {}

//	virtual void CreateGraphicsResource( CGraphicsResourceDesc &desc ) = 0;

	virtual boost::shared_ptr<CTextureResource> CreateTextureResource( const CTextureResourceDesc& desc ) = 0;
	virtual boost::shared_ptr<CMeshResource>    CreateMeshResource( const CMeshResourceDesc& desc ) = 0;
	virtual boost::shared_ptr<CShaderResource>  CreateShaderResource( const CShaderResourceDesc& desc ) = 0;
};



//=======================================================================================
// CGraphicsResourceFactory
//=======================================================================================

class CGraphicsResourceFactory
{
	boost::shared_ptr<CGraphicsResourceFactoryImpl> m_pImpl;

protected:

	static CSingleton<CGraphicsResourceFactory> m_obj;

public:

	CGraphicsResourceFactory();

	~CGraphicsResourceFactory();

	static CGraphicsResourceFactory* Get() { return m_obj.get(); }

	static void ReleaseSingleton() { m_obj.Release(); }

	void Init( CGraphicsResourceFactoryImpl *pFactoryImpl );

	boost::shared_ptr<CGraphicsResource> CreateGraphicsResource( const CGraphicsResourceDesc &desc );// { return m_pImpl->CreateGraphicsResource(desc); }

	boost::shared_ptr<CTextureResource> CreateTextureResource( const CTextureResourceDesc& desc ) { return ( m_pImpl ? m_pImpl->CreateTextureResource(desc) : boost::shared_ptr<CTextureResource>() ); }
	boost::shared_ptr<CMeshResource>    CreateMeshResource( const CMeshResourceDesc& desc )       { return ( m_pImpl ? m_pImpl->CreateMeshResource(desc)    : boost::shared_ptr<CMeshResource>()    ); }
	boost::shared_ptr<CShaderResource>  CreateShaderResource( const CShaderResourceDesc& desc )   { return ( m_pImpl ? m_pImpl->CreateShaderResource(desc)  : boost::shared_ptr<CShaderResource>()  ); }
};


inline CGraphicsResourceFactory& GraphicsResourceFactory()
{
	return (*CGraphicsResourceFactory::Get());
}



/**
 - Chached resources are created in advance
   - Usu. at the application startup
 - Not released until the user exit the application
   - User can force the cache manager to release a cached resource,
     but this feature should not be used.

*/
class CGraphicsResourceCacheManager : public CGraphicsComponent
{
	std::vector< boost::shared_ptr<CGraphicsResource> > m_vecpResurceCache;

protected:

	static CSingleton<CGraphicsResourceCacheManager> m_obj;

public:

	static CGraphicsResourceCacheManager* Get() { return m_obj.get(); }

	static void ReleaseSingleton() { m_obj.Release(); }

	bool Preload( const std::string& xml_filename );

	/// Create a resource and store it as a cache.
	/// Must be called by the render thread.
	void AddCache( CGraphicsResourceDesc& desc );

	/// Returns a cached resource that matches the desc
	boost::shared_ptr<CGraphicsResource> GetCachedResource( const CGraphicsResourceDesc& desc );

	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	/// Release resources currently not used by CGraphicsResourceManager
	void ReleaseGraphicsResources();
};


//------------------------------- inline implementations -------------------------------

inline CGraphicsResourceCacheManager& GraphicsResourceCacheManager()
{
	return (*CGraphicsResourceCacheManager::Get());
}



#endif /* __GraphicsResourceCacheManager_H__ */
