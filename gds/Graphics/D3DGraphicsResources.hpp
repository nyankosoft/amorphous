#ifndef  __D3DGraphicsResources_HPP__
#define  __D3DGraphicsResources_HPP__


#include "fwd.hpp"
#include "Graphics/GraphicsResources.hpp"


class CD3DTextureResource : public CTextureResource
{
	LPDIRECT3DTEXTURE9 m_pTexture;

//	boost::shared_ptr<CLockedTexture> m_pLockedTexture;

protected:

	/// Release texture without changing the reference count
	/// called only from CGraphicsResourceManager
	/// and from CTextureResource if Refresh() is used
	virtual void Release();

	/// returns false if m_TextureDesc has valid width, height, and format 
//	virtual bool IsDiskResource() const;

	/// create an empty texture
	/// - texture settings are read from m_TextureDesc
	bool CreateFromDesc();

//	void UpdateDescForCachedResource( const CGraphicsResourceDesc& desc );

public:

	CD3DTextureResource( const CTextureResourceDesc *pDesc );

	virtual ~CD3DTextureResource();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Texture; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

//	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

//	int CanBeUsedAsCache( const CGraphicsResourceDesc& desc );

	inline LPDIRECT3DTEXTURE9 GetTexture();

	/// Save the texture to disk as an image file
	/// - For debugging
	/// - Use this if the texture is loaded from file and filename is stored in GetDesc().ResourcePath
	///   - Wait! It will overwrite the original image file!
//	bool SaveTextureToImageFile(); commented out

	/// Save the texture to disk as an image file
	/// - For debugging
	/// - NOTE: image_filepath should be different from GetDesc().ResourcePath,
	///         or it will overwrite the original image file
	bool SaveTextureToImageFile( const std::string& image_filepath );

	bool Lock();

	bool Unlock();

	/// Creates an empty texture from the current desc.
	/// Called by the render thread.
	bool Create();

	friend class CGraphicsResourceManager;
};

/*
class CMeshSubResource
{
public:
	enum Name
	{
		VERTEX,
		INDEX,
		ATTRIBUTE_TABLE,
		NUM_SUBRESOURCES,
	};
};


class CMeshResource : public CGraphicsResource
{
	boost::shared_ptr<CD3DXMeshObjectBase> m_pMeshObject;

	CMeshResourceDesc m_MeshDesc;

	GraphicsResourceState::Name m_aSubResourceState[CMeshSubResource::NUM_SUBRESOURCES];

protected:

	/// release mesh object without changing the reference count
	/// called only from this class and CMeshObjectManager
	virtual void Release();

	/// returns false if there is a mesh generator
	bool IsDiskResource() const;

	/// create a mesh from a mesh generator
	bool CreateFromDesc();

public:

	CMeshResource( const CMeshResourceDesc *pDesc );

	virtual ~CMeshResource();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Mesh; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

	int CanBeUsedAsCache( const CGraphicsResourceDesc& desc );

	inline boost::shared_ptr<CD3DXMeshObjectBase> GetMesh();

	inline boost::shared_ptr<CD3DXMeshObjectBase> GetMeshInLoading();

	CMeshType::Name GetMeshType() const { return m_MeshDesc.MeshType; }

	const CGraphicsResourceDesc& GetDesc() const { return m_MeshDesc; }

	void GetStatus( char *pDestBuffer );

//	bool Lock();

//	bool Unlock();

	bool Create();

	void SetSubResourceState( CMeshSubResource::Name subresource, GraphicsResourceState::Name state );

	void CreateMeshAndLoadNonAsyncResources( MeshModel::C3DMeshModelArchive& archive );

	friend class CGraphicsResourceManager;
};


class CShaderResource : public CGraphicsResource
{
	CShaderManager *m_pShaderManager;

	CShaderResourceDesc m_ShaderDesc;

protected:

	/// Release the shader manager without changing the reference count
	/// - Called only from this class and CMeshObjectManager
	virtual void Release();

public:

	CShaderResource( const CShaderResourceDesc *pDesc );

	virtual ~CShaderResource();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Shader; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

//	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

	inline CShaderManager *GetShaderManager() { return m_pShaderManager; }

	const CGraphicsResourceDesc& GetDesc() const { return m_ShaderDesc; }

	friend class CGraphicsResourceManager;
};
*/


//---------------------------- inline implementations ----------------------------

//================================================================================
// CD3DTextureResource
//================================================================================

inline LPDIRECT3DTEXTURE9 CD3DTextureResource::GetTexture()
{
	if( GetState() == GraphicsResourceState::LOADED )
		return m_pTexture;
	else
		return NULL;
}



#endif /* __D3DGraphicsResources_HPP__ */
