#ifndef  __GraphicsResourceManager_H__
#define  __GraphicsResourceManager_H__
	

#include <string>
#include <vector>
#include <queue>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

#include "GraphicsComponentCollector.h"
#include "GraphicsResource.h"
#include "GraphicsResourceEntries.h"
#include "GraphicsResourceLoaders.h"

#include "Support/Singleton.h"
using namespace NS_KGL;


#include <boost/thread.hpp>


class CAsyncResourceLoader;


class CResourceLoadRequest
{
public:

	enum Type
	{
		LoadFromDisk,
		CopyToGraphicsMemory,
		NumRequestTypes
	};

private:

	boost::weak_ptr<CGraphicsResourceEntry> m_pResourceEntry;

//	int	m_ResourceEntryIndex;

	/// all the loaders have resource desc?
	boost::shared_ptr<CGraphicsResourceLoader> m_pLoader;

	Type m_RequestType;

public:

	CResourceLoadRequest( Type type,
		boost::weak_ptr<CGraphicsResourceEntry> pEntry//,
//		int entry_index
		)
		:
	m_RequestType(type),
	m_pResourceEntry(pEntry)
//	m_ResourceEntryIndex(entry_index)
	{}

	Type GetRequestType() const { return m_RequestType; }

//	boost::weak_ptr<CGraphicsResourceEntry> GetResourceEntry() { return m_pResourceEntry; }

	friend class CAsyncResourceLoader;
};


class CGraphicsDeviceRequest
{
public:

	enum Type
	{
		Lock,
		Unlock,
		NumRequestTypes
	};

	Type m_RequestType;

	boost::weak_ptr<CGraphicsResourceEntry> m_pResourceEntry;

public:

	CGraphicsDeviceRequest( CGraphicsDeviceRequest::Type type, boost::weak_ptr<CGraphicsResourceEntry> pEntry )
		:
	m_RequestType(type),
	m_pResourceEntry(pEntry)
	{}


	friend class CAsyncResourceLoader;
};


class CAsyncResourceLoader
{
	boost::mutex m_Mutex;

	std::queue<CResourceLoadRequest> m_ResourceLoadRequestQueue;

	std::queue<CGraphicsDeviceRequest> m_GraphicsDeviceRequestQueue;

protected:

	static CSingleton<CAsyncResourceLoader> m_obj;

public:

	static CAsyncResourceLoader* Get() { return m_obj.get(); }

	bool AddResourceLoadRequest( const CResourceLoadRequest& req );

	void ProcessResourceLoadRequest();

	void ProcessGraphicsDeviceRequests();

	void operator()()
	{
	}
};


inline CAsyncResourceLoader& AsyncResourceLoader()
{
	return (*CAsyncResourceLoader::Get());
}


//#define GraphicsResourceManager ( (*CGraphicsResourceManager::Get()) )

/**
 * mamage graphics resources
 * - save memory by sharing the same texture / mesh resources with reference counting.
 * - singleton class
 *   - must be released before the CGraphicsComponentCollector singleton is destroyed.
*/
class CGraphicsResourceManager : public CGraphicsComponent
{
private:

	boost::mutex m_IOMutex;

	bool m_AsyncLoadingAllowed;

	std::vector<boost::weak_ptr<CGraphicsResourceEntry>> m_vecpResourceEntry;

	std::vector<boost::shared_ptr<CTextureEntry>>       m_vecpTextureEntry;
	std::vector<boost::shared_ptr<CMeshObjectEntry>>    m_vecpMeshEntry;
	std::vector<boost::shared_ptr<CShaderManagerEntry>> m_vecpShaderEntry;

private:

	inline LPDIRECT3DTEXTURE9 GetTexture( int iTextureEntryID );

	inline CD3DXMeshObjectBase *GetMeshObject( int iMeshObjectEntryID );

	inline CShaderManager *GetShaderManager( int iShaderEntryID );

	/// Synchronously loads a graphics resource
	int LoadGraphicsResource( const CGraphicsResourceDesc& desc );

//	void IncResourceRefCount( const CGraphicsResourceHandle& handle );
//	void DecResourceRefCount( const CGraphicsResourceHandle& handle );

	CTextureEntry& GetTextureEntry( int id )      { return *(m_vecpTextureEntry[id].get()); }
	CMeshObjectEntry& GetMeshEntry( int id )      { return *(m_vecpMeshEntry[id].get()); }
	CShaderManagerEntry& GetShaderEntry( int id ) { return *(m_vecpShaderEntry[id].get()); }

	/// creates and registers a graphics resources of the specified type
	/// returns the shared ointer to the created and registered resource
	boost::shared_ptr<CGraphicsResourceEntry> CreateGraphicsResourceEntry( const CGraphicsResourceDesc& desc );

	/// called from handle
	int LoadTexture( std::string filename );

	int CreateTexture( const CTextureResourceDesc& desc );

	/// called from handle
	int LoadMeshObject( std::string filename, int mesh_type );

	/// called from handle
	int LoadShaderManager( std::string filename );

	bool ReleaseResourceEntry( boost::shared_ptr<CGraphicsResourceEntry> ptr );

	/// asynchronously loads a graphics resource
	/// - sends load request and returns
	/// - index to the entry is returned immediately
	int LoadAsync( const CGraphicsResourceDesc& desc );

	template<class T>
	size_t AddEntryToVacantSlot( T ptr, vector<T>& vecPtr );


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

	boost::shared_ptr<CGraphicsResourceEntry> CreateAt( const CGraphicsResourceDesc& desc, int dest_index );
	
	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam );
	virtual void ReleaseGraphicsResources();

	friend class CGraphicsResourceHandle;
	friend class CTextureHandle;
	friend class CMeshObjectHandle;
	friend class CShaderHandle;

//	void Init();
};


inline LPDIRECT3DTEXTURE9 CGraphicsResourceManager::GetTexture( int iTextureEntryID )
{
	if( iTextureEntryID < 0 )
		return NULL;	// invalid texture ID

	return m_vecpTextureEntry[iTextureEntryID]->GetTexture();
}


inline CD3DXMeshObjectBase *CGraphicsResourceManager::GetMeshObject( int iMeshObjectEntryID )
{
	if( iMeshObjectEntryID < 0 )
		return NULL;	// invalid texture ID

	return m_vecpMeshEntry[iMeshObjectEntryID]->GetMeshObject();
}


inline CShaderManager *CGraphicsResourceManager::GetShaderManager( int iShaderEntryID )
{
	if( iShaderEntryID < 0 )
		return NULL;	// invalid texture ID

	return m_vecpShaderEntry[iShaderEntryID]->GetShaderManager();
}


inline CGraphicsResourceManager& GraphicsResourceManager()
{
	return (*CGraphicsResourceManager::Get());
}


#endif		/*  __GraphicsResourceManager_H__  */
