#ifndef  __GraphicsResourceManager_H__
#define  __GraphicsResourceManager_H__
	

#define  TEXTURE_MANAGER_SYSTEM


#include <vector>
#include <string>

#include "3DCommon/GraphicsComponentCollector.h"

#include "Support/Singleton.h"
using namespace NS_KGL;


#include "GraphicsResourceEntries.h"


#define GraphicsResourceManager ( (*CGraphicsResourceManager::Get()) )

/**
 * mamage graphics resources
 * - save memory by sharing the same texture / mesh resources with reference counting.
 * - singleton class
 *   - must be released before the CGraphicsComponentCollector singleton is destroyed.
*/
class CGraphicsResourceManager : public CGraphicsComponent
{
private:

	std::vector<CGraphicsResourceEntry *> m_vecpResourceEntry;

private:

	inline LPDIRECT3DTEXTURE9 GetTexture( int iTextureEntryID );

	inline CD3DXMeshObjectBase *GetMeshObject( int iMeshObjectEntryID );

	int LoadGraphicsResource( const CGraphicsResourceDesc& desc );

	void IncResourceRefCount( int entry_id );
	void DecResourceRefCount( int entry_id );

	CGraphicsResourceEntry *CreateGraphicsResourceEntry( const CGraphicsResourceDesc& desc );

	/// called from handle
	int LoadTexture( std::string filename );

	/// called from handle
	int LoadMeshObject( std::string filename, int mesh_type );

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
	
	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam );
	virtual void ReleaseGraphicsResources();

	friend class CGraphicsResourceHandle;
	friend class CTextureHandle;
	friend class CMeshObjectHandle;

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



#endif		/*  __GraphicsResourceManager_H__  */
