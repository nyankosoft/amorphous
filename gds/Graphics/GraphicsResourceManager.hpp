#ifndef  __GraphicsResourceManager_H__
#define  __GraphicsResourceManager_H__
	

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "GraphicsComponentCollector.hpp"
#include "GraphicsResources.hpp"
#include "GraphicsResourceDescs.hpp"
#include "GraphicsResourceEntries.hpp"
#include "GraphicsResourceLoaders.hpp"

#include "Support/Singleton.hpp"
using namespace NS_KGL;


/**
 * manage graphics resources
 * - save memory by sharing the same texture / mesh resources with reference counting.
 * - singleton class
 *   - must be released before the CGraphicsComponentCollector singleton is destroyed.
*/
class CGraphicsResourceManager : public CGraphicsComponent
{
private:

	bool m_AsyncLoadingAllowed;

	boost::shared_ptr<CGraphicsResourceCacheManager> m_pCacheManager;

	std::vector< boost::shared_ptr<CGraphicsResourceEntry> > m_vecpResourceEntry;

	boost::mutex m_ResourceLock;

private:

	/// Synchronously loads a graphics resource
	boost::shared_ptr<CGraphicsResourceEntry> LoadGraphicsResource( const CGraphicsResourceDesc& desc );

	/// creates and registers a graphics resources of the specified type
	/// returns the shared ointer to the created and registered resource
	boost::shared_ptr<CGraphicsResourceEntry> CreateGraphicsResourceEntry();

	/// called from handle
	boost::shared_ptr<CGraphicsResourceEntry> LoadTexture( const CTextureResourceDesc& desc );

	boost::shared_ptr<CGraphicsResourceEntry> CreateTexture( const CTextureResourceDesc& desc );

	/// called from handle
	boost::shared_ptr<CGraphicsResourceEntry> LoadMesh( const CMeshResourceDesc& desc );

	/// called from handle
	boost::shared_ptr<CGraphicsResourceEntry> LoadShaderManager( const CShaderResourceDesc& desc );

	bool ReleaseResourceEntry( boost::shared_ptr<CGraphicsResourceEntry> ptr );

	boost::shared_ptr<CGraphicsResourceLoader> CreateResourceLoader(
		boost::shared_ptr<CGraphicsResourceEntry> pEntry,
		const CGraphicsResourceDesc& desc );

	boost::shared_ptr<CGraphicsResourceEntry> FindSameLoadedResource( const CGraphicsResourceDesc& desc );

	/// asynchronously loads a graphics resource
	/// - sends load request and returns
	/// - index to the entry is returned immediately
	boost::shared_ptr<CGraphicsResourceEntry> LoadAsync( const CGraphicsResourceDesc& desc );

protected:
///	CGraphicsResourceManager();		//singleton

	static CSingleton<CGraphicsResourceManager> m_obj;

public:

	static CGraphicsResourceManager* Get() { return m_obj.get(); }

	static void ReleaseSingleton() { m_obj.Release(); }

	CGraphicsResourceManager();

	virtual ~CGraphicsResourceManager();

	/// need to be called by user?
	/// - this class is a singleton, and the graphics component collector is also a singleton
	/// - this class must be destroyed before the graphics component collector
	void Release();

	/// reload any updated files since the last load
	void Refresh();

	/// returns whether the asynchronous loading feature is allowed
	bool IsAsyncLoadingAllowed() const { return m_AsyncLoadingAllowed; }

	void AllowAsyncLoading( bool allow );

//	boost::shared_ptr<CGraphicsResourceEntry> CreateAt( const CGraphicsResourceDesc& desc, int dest_index );

	void LoadGraphicsResources( const CGraphicsParameters& rParam );
	void ReleaseGraphicsResources();

	void AddCache( CGraphicsResourceDesc& desc );

	void GetStatus( GraphicsResourceType::Name type, char *pDestBuffer );

	friend class CGraphicsResourceHandle;
	friend class CTextureHandle;
	friend class CMeshObjectHandle;
	friend class CShaderHandle;
};


//------------------------------- inline implementations -------------------------------

inline CGraphicsResourceManager& GraphicsResourceManager()
{
	return (*CGraphicsResourceManager::Get());
}


#endif		/*  __GraphicsResourceManager_H__  */
