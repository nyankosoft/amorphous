#ifndef  __D3DGraphicsResources_HPP__
#define  __D3DGraphicsResources_HPP__


#include "fwd.hpp"
#include "Graphics/GraphicsResources.hpp"


namespace amorphous
{


class CD3DTextureResource : public TextureResource
{
	LPDIRECT3DTEXTURE9 m_pTexture;

	D3DTEXTUREFILTERTYPE m_MagFilter;
	D3DTEXTUREFILTERTYPE m_MinFilter;

	D3DTEXTUREADDRESS m_TextureAddressU;
	D3DTEXTUREADDRESS m_TextureAddressV;

//	boost::shared_ptr<LockedTexture> m_pLockedTexture;

protected:

	/// Release texture without changing the reference count
	/// called only from GraphicsResourceManager
	/// and from TextureResource if Refresh() is used
	virtual void Release();

	/// returns false if m_TextureDesc has valid width, height, and format 
//	virtual bool IsDiskResource() const;

	/// create an empty texture
	/// - texture settings are read from m_TextureDesc
	bool CreateFromDesc();

//	void UpdateDescForCachedResource( const GraphicsResourceDesc& desc );

	virtual LPDIRECT3DBASETEXTURE9 GetD3DBaseTexture() { return m_pTexture; }

	virtual HRESULT CreateD3DTextureFromFile( const std::string& filepath );

	virtual LPDIRECT3DSURFACE9 GetPrimaryTextureSurface();

	/// Returns true on success.
	bool GetD3DSurfaceDesc( UINT level, D3DSURFACE_DESC& dest );

	virtual HRESULT CreateD3DTexture( const TextureResourceDesc& desc, DWORD usage, D3DPOOL pool );

public:

	CD3DTextureResource( const TextureResourceDesc *pDesc );

	virtual ~CD3DTextureResource();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Texture; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

//	virtual bool CanBeSharedAsSameResource( const GraphicsResourceDesc& desc );

//	int CanBeUsedAsCache( const GraphicsResourceDesc& desc );

	inline LPDIRECT3DTEXTURE9 GetTexture();

	D3DTEXTUREFILTERTYPE GetMagFilter() const { return m_MagFilter; }
	D3DTEXTUREFILTERTYPE GetMinFilter() const { return m_MinFilter; }

	D3DTEXTUREADDRESS GetTextureAddressU() const { return m_TextureAddressU; }
	D3DTEXTUREADDRESS GetTextureAddressV() const { return m_TextureAddressV; }

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

	SDim2 GetSize2D( unsigned int level );

	Result::Name SetSamplingParameter( SamplingParameter::Name param, uint value );

	bool Lock( uint mip_level );

	bool Unlock();

	/// Creates an empty texture from the current desc.
	/// Called by the render thread.
	bool Create();

	Result::Name Accept( TextureResourceVisitor& visitor ) { return visitor.Visit( *this ); }

	friend class GraphicsResourceManager;
};


class CD3DCubeTextureResource : public CD3DTextureResource
{
	LPDIRECT3DCUBETEXTURE9 m_pCubeTexture;

private:

	LPDIRECT3DBASETEXTURE9 GetD3DBaseTexture() { return m_pCubeTexture; }

	HRESULT CreateD3DTexture( const TextureResourceDesc& desc, DWORD usage, D3DPOOL pool );

	HRESULT CreateD3DTextureFromFile( const std::string& filepath );

	LPDIRECT3DSURFACE9 GetPrimaryTextureSurface();

public:

	CD3DCubeTextureResource( const TextureResourceDesc *pDesc );

	inline LPDIRECT3DCUBETEXTURE9 GetCubeTexture();

	Result::Name Accept( TextureResourceVisitor& visitor ) { return visitor.Visit( *this ); }
};


/*
class MeshSubResource
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


class MeshResource : public GraphicsResource
{
	boost::shared_ptr<CD3DXMeshObjectBase> m_pMeshObject;

	MeshResourceDesc m_MeshDesc;

	GraphicsResourceState::Name m_aSubResourceState[MeshSubResource::NUM_SUBRESOURCES];

protected:

	/// release mesh object without changing the reference count
	/// called only from this class and CMeshObjectManager
	virtual void Release();

	/// returns false if there is a mesh generator
	bool IsDiskResource() const;

	/// create a mesh from a mesh generator
	bool CreateFromDesc();

public:

	MeshResource( const MeshResourceDesc *pDesc );

	virtual ~MeshResource();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Mesh; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	virtual bool CanBeSharedAsSameResource( const GraphicsResourceDesc& desc );

	int CanBeUsedAsCache( const GraphicsResourceDesc& desc );

	inline boost::shared_ptr<CD3DXMeshObjectBase> GetMesh();

	inline boost::shared_ptr<CD3DXMeshObjectBase> GetMeshInLoading();

	MeshType::Name GetMeshType() const { return m_MeshDesc.MeshType; }

	const GraphicsResourceDesc& GetDesc() const { return m_MeshDesc; }

	void GetStatus( char *pDestBuffer );

//	bool Lock();

//	bool Unlock();

	bool Create();

	void SetSubResourceState( MeshSubResource::Name subresource, GraphicsResourceState::Name state );

	void CreateMeshAndLoadNonAsyncResources( C3DMeshModelArchive& archive );

	friend class GraphicsResourceManager;
};
*/

class CD3DShaderResource : public ShaderResource
{
protected:

	/// Release the shader manager without changing the reference count
	/// - Called only from this class and CMeshObjectManager
//	virtual void Release();

	ShaderManager *CreateShaderManager();

	ShaderManager *CreateFixedFunctionPipelineManager();

public:

	CD3DShaderResource( const ShaderResourceDesc *pDesc );

	~CD3DShaderResource();
/*
	virtual bool LoadFromFile( const std::string& filepath );
	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );
//	virtual bool CanBeSharedAsSameResource( const GraphicsResourceDesc& desc );

	friend class GraphicsResourceManager;*/
};



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


//================================================================================
// CD3DCubeTextureResource
//================================================================================

inline LPDIRECT3DCUBETEXTURE9 CD3DCubeTextureResource::GetCubeTexture()
{
	if( GetState() == GraphicsResourceState::LOADED )
		return m_pCubeTexture;
	else
		return NULL;
}


} // namespace amorphous



#endif /* __D3DGraphicsResources_HPP__ */
