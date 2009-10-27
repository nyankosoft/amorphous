#ifndef  __GraphicsResourceLoaders_H__
#define  __GraphicsResourceLoaders_H__


#include "Graphics/fwd.hpp"
using namespace MeshModel;
#include "GraphicsResources.hpp"
#include "GraphicsResourceEntries.hpp"
#include "Support/fnop.hpp"
#include "Support/SafeDelete.hpp"
#include "Support/BitmapImage.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"
using namespace GameLib1::Serialization;


const std::string g_NullString = "";


class CGraphicsResourceLoader
{
	/// entry that stores the loaded resource
	boost::weak_ptr<CGraphicsResourceEntry> m_pResourceEntry;

protected:

//	virtual boost::shared_ptr<CGraphicsResourceEntry> GetResourceEntry() = 0;

	boost::shared_ptr<CGraphicsResourceEntry> GetResourceEntry() { return m_pResourceEntry.lock(); }

	const boost::shared_ptr<CGraphicsResourceEntry> GetResourceEntry() const { return m_pResourceEntry.lock(); }

	inline const std::string& GetSourceFilepath();

public:

	CGraphicsResourceLoader( boost::weak_ptr<CGraphicsResourceEntry> pEntry )
		:
	m_pResourceEntry(pEntry)
	{}

	inline boost::shared_ptr<CGraphicsResource> GetResource();

	virtual Result::Name Load();

	Result::Name LoadFromDisk();

	virtual bool LoadFromFile( const std::string& filepath ) { return false; }

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname ) { return false; }

	/// Called by the resource IO thread
	/// - copy the loaded resource to locked buffer
	virtual bool CopyLoadedContentToGraphicsResource() { return false; }

	virtual bool AcquireResource();

	/// Called by the rendering thread
	/// - Why virtual ?
	///   -> Mesh resources need separate locks and unlocks for subresources
	virtual bool Lock();

	/// Called by the rendering thread
	/// - Why virtual ?
	///   -> See Lock() above
	virtual bool Unlock();

//	virtual void SetEntry( boost::weak_ptr<CGraphicsResourceEntry> pEntry ) = 0;

	/// add a lock request to the graphics device request queue of the async resource loader
	virtual void OnLoadingCompleted( boost::shared_ptr<CGraphicsResourceLoader> pSelf );

	/// - Called by async resource loader when the resource is loaded on locked graphics memory
	///   and the memory is successfully unlocked.
	/// - By default, this method set the resource state to GraphicsResourceState::LOADED.
	virtual void OnResourceLoadedOnGraphicsMemory();

	virtual bool IsReadyToLock() const { return true; }

	/// Used to fill out desc properties that can be obtained
	/// only after the resource is loaded from disk
	/// - e.g., width and height of image files for textures
	virtual void FillResourceDesc() {}

	/// sub resource loaders of mesh don't have descs
	virtual const CGraphicsResourceDesc *GetDesc() const { return NULL; }

	virtual bool LoadToGraphicsMemoryByRenderThread() { return false; }
};


/// loads a texture from disk
class CDiskTextureLoader : public CGraphicsResourceLoader
{
	/// Stores image properties such as width and height.
	/// - Image properties are obtained from m_Image after the image is loaded
	CTextureResourceDesc m_TextureDesc;

	/// Stores texture data loaded from disk
	CBitmapImage m_Image;

protected:

//	boost::shared_ptr<CGraphicsResourceEntry> GetResourceEntry() { return m_pTextureEntry.lock(); }

public:

	CDiskTextureLoader( boost::weak_ptr<CGraphicsResourceEntry> pEntry, const CTextureResourceDesc& desc )
		:
	CGraphicsResourceLoader(pEntry),
	m_TextureDesc(desc)
	{}

	bool LoadFromFile( const std::string& filepath );

	/// load image from the db as an image archive
	bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	/// copy the bitmap image to the locked texture surface
	bool CopyLoadedContentToGraphicsResource();

	void FillResourceDesc();

	const CGraphicsResourceDesc *GetDesc() const { return &m_TextureDesc; }

	/// called by the system
	/// - called inside CopyTo()
	void FillTexture( CLockedTexture& texture );
};


class CMeshLoader : public CGraphicsResourceLoader
{
	CMeshResourceDesc m_MeshDesc;

	boost::shared_ptr<C3DMeshModelArchive> m_pArchive;

	U32 m_MeshLoaderStateFlags;

	boost::weak_ptr<CMeshLoader> m_pSelf;

	void *m_pVertexBufferContent;

	std::vector<U16> m_vecIndexBufferContent;

	std::vector<D3DXATTRIBUTERANGE> m_vecAttributeRange;

public:

	CMeshLoader( boost::weak_ptr<CGraphicsResourceEntry> pEntry, const CMeshResourceDesc& desc );

	~CMeshLoader();

	bool LoadFromFile( const std::string& filepath );

	bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	bool CopyLoadedContentToGraphicsResource();

	bool AcquireResource();

	/// Does not lock the mesh resource
	/// - Loaders for each component of the mesh (vertices, indices, and attribute buffers)
	///   do the lock & unlock, and copy each resource to the mesh
	bool Lock() { return false; }

	/// Does not unlock the mesh resource
	/// - See the comment of Lock()
	bool Unlock() { return false; }

	void OnLoadingCompleted( boost::shared_ptr<CGraphicsResourceLoader> pSelf );

	void OnResourceLoadedOnGraphicsMemory();

	void LoadMeshSubresources();

	void SetWeakPtr( boost::weak_ptr<CMeshLoader> pSelf ) { m_pSelf = pSelf; }

	const CGraphicsResourceDesc *GetDesc() const { return &m_MeshDesc; }

	void RaiseStateFlags( U32 flags );

	U32 GetMeshLoaderStateFlags() const { return m_MeshLoaderStateFlags; }

	void SendLockRequestIfAllSubresourcesHaveBeenLoaded();

	bool LoadToGraphicsMemoryByRenderThread();

	vector<U16>& IndexBufferContent() { return m_vecIndexBufferContent; }

	void *VertexBufferContent() { return m_pVertexBufferContent; }

	std::vector<D3DXATTRIBUTERANGE>& AttributeTable() { return m_vecAttributeRange; }

	enum SubResouceLoadingStateFlag
	{
		VERTICES_LOADED      = ( 1 << 0 ),
		INDICES_LOADED       = ( 1 << 1 ),
		ATTRIB_TABLES_LOADED = ( 1 << 2 ),
	};
};


class CD3DXMeshLoaderBase : public CGraphicsResourceLoader
{
protected:

	/// entry to that stores the loaded resource
//	boost::weak_ptr<CGraphicsResourceEntry> m_pMeshEntry;

//	CMeshResourceDesc m_Desc;

	boost::shared_ptr<C3DMeshModelArchive> m_pArchive;

	/// Store the shared_ptr of the mesh loader so that it do not get released
	/// after loading mesh archive.
	boost::shared_ptr<CMeshLoader> m_pMeshLoader;

public:

	CD3DXMeshLoaderBase( boost::weak_ptr<CGraphicsResourceEntry> pEntry )
		:
	CGraphicsResourceLoader(pEntry)
	{}

	virtual ~CD3DXMeshLoaderBase() {}

	bool AcquireResource() { return true; }

	inline CBasicMesh *GetMesh();

	Result::Name Load();

	virtual bool LoadFromArchive() = 0;

	inline GraphicsResourceState::Name GetSubResourceState( CMeshSubResource::Name subresource ) const;

	inline void SetSubResourceState( CMeshSubResource::Name subresource, GraphicsResourceState::Name state );

	CD3DXMeshObjectBase *GetD3DMeshImpl();

	friend class CMeshLoader;
};

class CD3DXMeshVerticesLoader : public CD3DXMeshLoaderBase
{
	// hold data to be copied to VB of the mesh
//	std::vector<unsigned char> m_vecVertexBufferContent;
	void *m_pVertexBufferContent;
	
	int m_VertexBufferSize;

	void *m_pLockedVertexBuffer;

protected:

//	const std::string& GetSourceFilepath() { return m_Desc.Filename; }

public:

	CD3DXMeshVerticesLoader( boost::weak_ptr<CGraphicsResourceEntry> pEntry )
		:
	CD3DXMeshLoaderBase(pEntry),
	m_pLockedVertexBuffer(NULL),
	m_pVertexBufferContent(NULL)
	{}

	~CD3DXMeshVerticesLoader() { SafeDelete(m_pVertexBufferContent); }

	bool LoadFromArchive();

	bool CopyLoadedContentToGraphicsResource();

	// Lock the index buffer and save the pointer to the locked buffer
	bool Lock();

	bool Unlock();

	void OnResourceLoadedOnGraphicsMemory();

	bool IsReadyToLock() const;
};


class CD3DXMeshIndicesLoader : public CD3DXMeshLoaderBase
{

//	std::vector<unsigned char> m_vecIndexBufferContent;

	void *m_pIndexBufferContent;

	int m_IndexBufferSize;

	void *m_pLockedIndexBuffer;

protected:

//	const std::string& GetSourceFilepath() { return m_Desc.Filename; }

public:

	CD3DXMeshIndicesLoader( boost::weak_ptr<CGraphicsResourceEntry> pEntry )
		:
	CD3DXMeshLoaderBase(pEntry),
	m_pLockedIndexBuffer(NULL),
	m_pIndexBufferContent(NULL)
	{}

	~CD3DXMeshIndicesLoader() { SafeDelete(m_pIndexBufferContent); }

	bool LoadFromArchive();

	bool CopyLoadedContentToGraphicsResource();

	// Lock the index buffer and save the pointer to the locked buffer
	bool Lock();

	bool Unlock();

	void OnResourceLoadedOnGraphicsMemory();

	bool IsReadyToLock() const;
};


class CD3DXMeshAttributeTableLoader : public CD3DXMeshLoaderBase
{
	DWORD *m_pLockedAttributeBuffer;

protected:

//	const std::string& GetSourceFilepath() { return m_Desc.Filename; }

public:

	CD3DXMeshAttributeTableLoader( boost::weak_ptr<CGraphicsResourceEntry> pEntry )
		:
	CD3DXMeshLoaderBase(pEntry),
	m_pLockedAttributeBuffer(NULL)
	{}

	~CD3DXMeshAttributeTableLoader() {}

	// copy the resource directly from mesh archive
	// -> No need to convert data format
	virtual bool LoadFromArchive() { return true; }

	bool CopyLoadedContentToGraphicsResource();

	// Lock the index buffer and save the pointer to the locked buffer
	bool Lock();

	bool Unlock();

	void OnResourceLoadedOnGraphicsMemory();

	bool IsReadyToLock() const;

};


//--------------------------- inline implementations ---------------------------

//==============================================================================
// CGraphicsResourceLoader
//==============================================================================

inline boost::shared_ptr<CGraphicsResource> CGraphicsResourceLoader::GetResource()
{
	boost::shared_ptr<CGraphicsResourceEntry> pEntry = GetResourceEntry();

	if( pEntry )
		return pEntry->GetResource();
	else
		return boost::shared_ptr<CGraphicsResource>();
}


inline const std::string& CGraphicsResourceLoader::GetSourceFilepath()
{
	const CGraphicsResourceDesc *pDesc = GetDesc();

	if( pDesc )
	{
		return pDesc->ResourcePath;
	}
	else
		return g_NullString;

/*	boost::shared_ptr<CGraphicsResourceEntry> pEntry = GetResourceEntry();
	if( pEntry && pEntry->GetResource() )
		return pEntry->GetResource()->GetDesc().ResourcePath;
	else
		return g_NullString;*/
}


//==============================================================================
// CD3DXMeshObjectBase
//==============================================================================

inline CBasicMesh *CD3DXMeshLoaderBase::GetMesh()
{
	boost::shared_ptr<CGraphicsResourceEntry> pEntry = GetResourceEntry();
	if( pEntry )
	{
		boost::shared_ptr<CMeshResource> pMesh = pEntry->GetMeshResource();

		if( pMesh )
			return pMesh->GetMeshInLoading().get();
		else
			return NULL;
	}
	else
		return NULL;
}


inline GraphicsResourceState::Name CD3DXMeshLoaderBase::GetSubResourceState( CMeshSubResource::Name subresource ) const
{
	if( GetResourceEntry()
	 && GetResourceEntry()->GetMeshResource() )
	{
		return GetResourceEntry()->GetMeshResource()->GetSubResourceState( subresource );
	}
	else
		return GraphicsResourceState::RELEASED;
}


inline void CD3DXMeshLoaderBase::SetSubResourceState( CMeshSubResource::Name subresource,
													  GraphicsResourceState::Name state )
{
	if( GetResourceEntry()
	 && GetResourceEntry()->GetMeshResource() )
	{
		GetResourceEntry()->GetMeshResource()->SetSubResourceState( subresource, state );
	}
}


/// loads a shader from disk
class CShaderLoader : public CGraphicsResourceLoader
{
	/// Stores filepath
	CShaderResourceDesc m_ShaderDesc;

	/// Stores shader contents
	stream_buffer m_ShaderTextBuffer;

public:

	CShaderLoader( boost::weak_ptr<CGraphicsResourceEntry> pEntry, const CShaderResourceDesc& desc )
		:
	CGraphicsResourceLoader(pEntry),
	m_ShaderDesc(desc)
	{}

	bool LoadFromFile( const std::string& filepath );

	/// load image from the db as an image archive
//	bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	/// copy the bitmap image to the locked texture surface
//	bool CopyLoadedContentToGraphicsResource();

//	void FillResourceDesc();

	const CGraphicsResourceDesc *GetDesc() const { return &m_ShaderDesc; }

	bool CopyLoadedContentToGraphicsResource() { return true; }

	bool AcquireResource();

	bool Lock() { return true; }

	bool Unlock() { return true; }

	void OnLoadingCompleted( boost::shared_ptr<CGraphicsResourceLoader> pSelf );

	bool LoadToGraphicsMemoryByRenderThread();
};


#endif  /* __GraphicsResourceLoaders_H__ */
