#ifndef  __GraphicsResourceManager_H__
#define  __GraphicsResourceManager_H__
	

#include <vector>
#include <string>

#include "GraphicsComponentCollector.h"
#include "GraphicsResourceEntries.h"

#include "Support/Singleton.h"
using namespace NS_KGL;


#include <boost/thread.hpp>


class CGraphicsResourceLoader
{
	boost::mutex m_Mutex;

protected:

	static CSingleton<CGraphicsResourceLoader> m_obj;

public:

	static CGraphicsResourceLoader* Get() { return m_obj.get(); }

	void operator()()
	{
	}
};


inline CGraphicsResourceLoader& GraphicsResourceLoader()
{
	return (*CGraphicsResourceLoader::Get());
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

	bool m_AsyncLoadingAllowed;

	std::vector<CGraphicsResourceEntry *> m_vecpResourceEntry;

private:

	inline LPDIRECT3DTEXTURE9 GetTexture( int iTextureEntryID );

	inline CD3DXMeshObjectBase *GetMeshObject( int iMeshObjectEntryID );

	inline CShaderManager *GetShaderManager( int iShaderEntryID );

	int LoadGraphicsResource( const CGraphicsResourceDesc& desc );

	void IncResourceRefCount( int entry_id );
	void DecResourceRefCount( int entry_id );

	CGraphicsResourceEntry *CreateGraphicsResourceEntry( const CGraphicsResourceDesc& desc );

	/// called from handle
	int LoadTexture( std::string filename );

	int CreateTexture( const CTextureResourceDesc& desc );

	/// called from handle
	int LoadMeshObject( std::string filename, int mesh_type );

	/// called from handle
	int LoadShaderManager( std::string filename );

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

	return ((CTextureEntry *)m_vecpResourceEntry[iTextureEntryID])->GetTexture();
}


inline CD3DXMeshObjectBase *CGraphicsResourceManager::GetMeshObject( int iMeshObjectEntryID )
{
	if( iMeshObjectEntryID < 0 )
		return NULL;	// invalid texture ID

	return ((CMeshObjectEntry *)m_vecpResourceEntry[iMeshObjectEntryID])->GetMeshObject();
}


inline CShaderManager *CGraphicsResourceManager::GetShaderManager( int iShaderEntryID )
{
	if( iShaderEntryID < 0 )
		return NULL;	// invalid texture ID

	return ((CShaderManagerEntry *)m_vecpResourceEntry[iShaderEntryID])->GetShaderManager();
}


inline CGraphicsResourceManager& GraphicsResourceManager()
{
	return (*CGraphicsResourceManager::Get());
}


#endif		/*  __GraphicsResourceManager_H__  */
