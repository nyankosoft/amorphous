#ifndef  __GraphicsResourceManager_H__
#define  __GraphicsResourceManager_H__
	

#include <string>
#include <vector>
#include <memory>

#include "GraphicsComponentCollector.hpp"
#include "GraphicsResources.hpp"
#include "GraphicsResourceLoaders.hpp"

#include "amorphous/Support/singleton.hpp"


namespace amorphous
{


/**
 * manage graphics resources
 * - save memory by sharing the same texture / mesh resources with reference counting.
 * - singleton class
 *   - must be released before the GraphicsComponentCollector singleton is destroyed.
*/
class GraphicsResourceManager : public GraphicsComponent
{
private:

	bool m_AsyncLoadingAllowed;

	std::shared_ptr<GraphicsResourceCacheManager> m_pCacheManager;

	std::vector< std::shared_ptr<GraphicsResourceEntry> > m_vecpResourceEntry;

	std::mutex m_ResourceLock;

private:

	/// Synchronously loads a graphics resource
	std::shared_ptr<GraphicsResourceEntry> LoadGraphicsResource( const GraphicsResourceDesc& desc );

	/// creates and registers a graphics resources of the specified type
	/// returns the shared ointer to the created and registered resource
	std::shared_ptr<GraphicsResourceEntry> CreateGraphicsResourceEntry();

	/// called from handle
	std::shared_ptr<GraphicsResourceEntry> LoadTexture( const TextureResourceDesc& desc );

	std::shared_ptr<GraphicsResourceEntry> CreateTexture( const TextureResourceDesc& desc );

	/// called from handle
	std::shared_ptr<GraphicsResourceEntry> LoadMesh( const MeshResourceDesc& desc );

	/// called from handle
	std::shared_ptr<GraphicsResourceEntry> LoadShaderManager( const ShaderResourceDesc& desc );

	bool ReleaseResourceEntry( std::shared_ptr<GraphicsResourceEntry> ptr );

	std::shared_ptr<GraphicsResourceLoader> CreateResourceLoader(
		std::shared_ptr<GraphicsResourceEntry> pEntry,
		const GraphicsResourceDesc& desc );

	std::shared_ptr<GraphicsResourceEntry> FindSameLoadedResource( const GraphicsResourceDesc& desc );

	/// asynchronously loads a graphics resource
	/// - sends load request and returns
	/// - index to the entry is returned immediately
	std::shared_ptr<GraphicsResourceEntry> LoadAsync( const GraphicsResourceDesc& desc );

protected:
///	GraphicsResourceManager();		//singleton

	static singleton<GraphicsResourceManager> m_obj;

public:

	static GraphicsResourceManager* Get() { return m_obj.get(); }

	static void ReleaseSingleton() { m_obj.Release(); }

	GraphicsResourceManager();

	virtual ~GraphicsResourceManager();

	/// need to be called by user?
	/// - this class is a singleton, and the graphics component collector is also a singleton
	/// - this class must be destroyed before the graphics component collector
	void Release();

	/// reload any updated files since the last load
	void Refresh();

	/// returns whether the asynchronous loading feature is allowed
	bool IsAsyncLoadingAllowed() const { return m_AsyncLoadingAllowed; }

	void AllowAsyncLoading( bool allow );

//	std::shared_ptr<GraphicsResourceEntry> CreateAt( const GraphicsResourceDesc& desc, int dest_index );

	void LoadGraphicsResources( const GraphicsParameters& rParam );
	void ReleaseGraphicsResources();

	void AddCache( GraphicsResourceDesc& desc );

	void GetStatus( GraphicsResourceType::Name type, std::string& dest_buffer );

	void GetStatus( GraphicsResourceType::Name type, std::vector<std::string>& dest_buffer );

	friend class GraphicsResourceHandle;
	friend class TextureHandle;
	friend class MeshHandle;
	friend class ShaderHandle;
};


//------------------------------- inline implementations -------------------------------

inline GraphicsResourceManager& GetGraphicsResourceManager()
{
	return (*GraphicsResourceManager::Get());
}

//------------------------------- global functions -------------------------------

void SetCurrentThreadAsRenderThread();

} // namespace amorphous



#endif		/*  __GraphicsResourceManager_H__  */
