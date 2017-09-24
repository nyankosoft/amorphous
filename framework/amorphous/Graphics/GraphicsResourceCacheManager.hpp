#ifndef  __GraphicsResourceCacheManager_H__
#define  __GraphicsResourceCacheManager_H__


#include <vector>
#include <queue>
#include "GraphicsResourceDescs.hpp"
#include "GraphicsComponentCollector.hpp"

#include "amorphous/Support/singleton.hpp"


namespace amorphous
{


//=======================================================================================
// GraphicsResourceFactoryImpl
//=======================================================================================

class GraphicsResourceFactoryImpl
{
public:

	GraphicsResourceFactoryImpl() {}

	virtual ~GraphicsResourceFactoryImpl() {}

//	virtual void CreateGraphicsResource( GraphicsResourceDesc &desc ) = 0;

	virtual std::shared_ptr<TextureResource> CreateTextureResource( const TextureResourceDesc& desc ) = 0;
	virtual std::shared_ptr<MeshResource>    CreateMeshResource( const MeshResourceDesc& desc ) = 0;
	virtual std::shared_ptr<ShaderResource>  CreateShaderResource( const ShaderResourceDesc& desc ) = 0;
};



//=======================================================================================
// GraphicsResourceFactory
//=======================================================================================

class GraphicsResourceFactory
{
	std::shared_ptr<GraphicsResourceFactoryImpl> m_pImpl;

protected:

	static singleton<GraphicsResourceFactory> m_obj;

public:

	GraphicsResourceFactory();

	~GraphicsResourceFactory();

	static GraphicsResourceFactory* Get() { return m_obj.get(); }

	static void ReleaseSingleton() { m_obj.Release(); }

	void Init( GraphicsResourceFactoryImpl *pFactoryImpl );

	std::shared_ptr<GraphicsResource> CreateGraphicsResource( const GraphicsResourceDesc &desc );// { return m_pImpl->CreateGraphicsResource(desc); }

	std::shared_ptr<TextureResource> CreateTextureResource( const TextureResourceDesc& desc ) { return ( m_pImpl ? m_pImpl->CreateTextureResource(desc) : std::shared_ptr<TextureResource>() ); }
	std::shared_ptr<MeshResource>    CreateMeshResource( const MeshResourceDesc& desc )       { return ( m_pImpl ? m_pImpl->CreateMeshResource(desc)    : std::shared_ptr<MeshResource>()    ); }
	std::shared_ptr<ShaderResource>  CreateShaderResource( const ShaderResourceDesc& desc )   { return ( m_pImpl ? m_pImpl->CreateShaderResource(desc)  : std::shared_ptr<ShaderResource>()  ); }
};


inline GraphicsResourceFactory& GetGraphicsResourceFactory()
{
	return (*GraphicsResourceFactory::Get());
}



/**
 - Chached resources are created in advance
   - Usu. at the application startup
 - Not released until the user exit the application
   - User can force the cache manager to release a cached resource,
     but this feature should not be used.

*/
class GraphicsResourceCacheManager : public GraphicsComponent
{
	std::vector< std::shared_ptr<GraphicsResource> > m_vecpResurceCache;

protected:

	static singleton<GraphicsResourceCacheManager> m_obj;

public:

	static GraphicsResourceCacheManager* Get() { return m_obj.get(); }

	static void ReleaseSingleton() { m_obj.Release(); }

	bool Preload( const std::string& xml_filename );

	/// Create a resource and store it as a cache.
	/// Must be called by the render thread.
	void AddCache( GraphicsResourceDesc& desc );

	/// Returns a cached resource that matches the desc
	std::shared_ptr<GraphicsResource> GetCachedResource( const GraphicsResourceDesc& desc );

	void LoadGraphicsResources( const GraphicsParameters& rParam );

	/// Release resources currently not used by GraphicsResourceManager
	void ReleaseGraphicsResources();
};


//------------------------------- inline implementations -------------------------------

inline GraphicsResourceCacheManager& GetGraphicsResourceCacheManager()
{
	return (*GraphicsResourceCacheManager::Get());
}


} // namespace amorphous



#endif /* __GraphicsResourceCacheManager_H__ */
