#ifndef  __GraphicsResources_HPP__
#define  __GraphicsResources_HPP__


#include <memory.h>
#include <string>
#include <sys/stat.h>
#include <boost/weak_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include "fwd.hpp"
#include "Graphics/GraphicsResourceDescs.hpp"
#include "Graphics/FloatRGBAColor.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"


class GraphicsResourceState
{
public:
	enum Name
	{
		CREATED,                  ///< Memory for the resource has been allocated. The content has not been loaded on memory. i.e.) empty state
		LOADING_SYNCHRONOUSLY,
		LOADING_ASYNCHRONOUSLY,
		LOADED,                   ///< The resource is ready to use
		RELEASING_SYNCHRONOUSLY,
		RELEASING_ASYNCHRONOUSLY,
		RELEASED,
		NUMSTATES
	};
};


class GraphicsResourceFlag
{
public:
	enum Flags
	{
		DontShare = ( 1 << 0 ),
		Flag1     = ( 1 << 1 ),
		Flag2     = ( 1 << 2 ),
	};
};


class CLockedTexture
{
	void *m_pBits;

	int m_Width;

	int m_Height;

public:

	CLockedTexture() : m_pBits(NULL) {}

	int GetWidth();

	virtual void SetPixelARGB32( int x, int y, U32 argb_color ) { ((U32 *)m_pBits)[ y * m_Width + x ] = argb_color; }

	/// \param alpha [0,255]
	inline void SetAlpha( int x, int y, U8 alpha )
	{
		((U32 *)m_pBits)[ y * m_Width + x ] &= ( (alpha << 24) | 0x00FFFFFF );
	}

	virtual void Clear( U32 argb_color )
	{
		const int num_bytes_per_pixel = sizeof(U32);
		if( argb_color == 0 )
			memset( m_pBits, 0, m_Width * m_Height * num_bytes_per_pixel );
		else if( argb_color == 0xFFFFFFFF )
			memset( m_pBits, 0xFF, m_Width * m_Height * num_bytes_per_pixel );
		else
		{
			int w = m_Width;
			int h = m_Height;
			for( int y=0; y<h; y++ )
			{
				for( int x=0; x<w; x++ )
				{
					SetPixelARGB32( x, y, argb_color );
				}
			}
		}
	}

	virtual void Clear( const SFloatRGBAColor& color ) { Clear( color.GetARGB32() ); }

	friend class CD3DTextureResource;
};


/// Used to fill the texture content when
/// - A texture resource is created.
/// - A texture resource is released and recreated after the graphics device is lost
class CTextureFillingAlgorithm
{
public:

	virtual ~CTextureFillingAlgorithm() {}

	/// called by the system after the texture resource is created
	virtual void FillTexture( CLockedTexture& texture ) = 0;
};


class CSignleColorTextureFilling : public CTextureFillingAlgorithm
{
	SFloatRGBAColor m_Color;

public:

	CSignleColorTextureFilling( const SFloatRGBAColor& color )
		:
	m_Color(color)
	{
	}

	void FillTexture( CLockedTexture& texture )
	{
		texture.Clear( m_Color );

/*		const int w = texture.GetWidth();
		const int h = texture.GetHeight();
		for( int y=0; y<h; y++ )
		{
			for( int x=0; x<w; x++ )
			{
				texture.SetPixelColor( x, y, m_Color );
			}
		}*/
	}
};


class CGraphicsResource
{
protected:

	unsigned int m_OptionFlags;

	/// stores the time when the file was updated last
	time_t m_LastModifiedTimeOfFile;

	GraphicsResourceState::Name m_State;

	/// If true, kept in the array of cahced resources
	bool m_IsCachedResource;

	int m_Index;

	boost::mutex m_StateChangeLock;

protected:

	/// reference count is not changed in this function?
	virtual void Release() = 0;

	virtual bool IsDiskResource() const { return true; }

	/// load resource from a file or a binary database
	bool LoadFromDisk();

	/// Added to create empty texture as a graphics resource
	virtual bool CreateFromDesc() { return false; }

	void SetIndex( int index ) { m_Index = index; }

	virtual void UpdateDescForCachedResource( const CGraphicsResourceDesc& desc ) {}

public:

	CGraphicsResource();

	virtual ~CGraphicsResource();

	virtual GraphicsResourceType::Name GetResourceType() const = 0;

	int GetIndex() const { return m_Index; }

	bool Load();

//	bool LoadAsync();

	void ReleaseNonChachedResource();

	void ReleaseCachedResource();

	bool IsCachedResource() const { return m_IsCachedResource; }

	/// \param filepath [in] pathname of the file to load
	/// - This is usually m_Filename itself
	virtual bool LoadFromFile( const std::string& filepath ) = 0;

	/// \param db [in] The database which contains the target graphics resource
	/// \param keyname [in] Required to read the graphics resource from the db
	/// - These are the strings obtained by decomposing m_Filename to "(db_filepath)::(keyname)"
	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname ) = 0;

	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

	virtual int CanBeUsedAsCache( const CGraphicsResourceDesc& desc ) { return 0; }

	void Refresh();

	virtual const CGraphicsResourceDesc& GetDesc() const = 0;

	virtual bool Lock() { return false; }

	virtual bool Unlock() { return false; }

	virtual bool Create() { return false; }

	inline void SetState( GraphicsResourceState::Name state );

	inline GraphicsResourceState::Name GetState();

	virtual void GetStatus( char *pDestBuffer );

	friend class CGraphicsResourceManager;
	friend class CGraphicsResourceCacheManager;
};


class CTextureResource : public CGraphicsResource
{
protected:

	CTextureResourceDesc m_TextureDesc;

	boost::shared_ptr<CLockedTexture> m_pLockedTexture;

protected:

	/// Release texture without changing the reference count
	/// called only from CGraphicsResourceManager
	/// and from CTextureResource if Refresh() is used
	virtual void Release();

	/// returns false if m_TextureDesc has valid width, height, and format 
	virtual bool IsDiskResource() const;

	/// create an empty texture
	/// - texture settings are read from m_TextureDesc
	virtual bool CreateFromDesc() = 0;

	void UpdateDescForCachedResource( const CGraphicsResourceDesc& desc );

public:

	CTextureResource( const CTextureResourceDesc *pDesc );

	virtual ~CTextureResource();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Texture; }

	virtual bool LoadFromFile( const std::string& filepath ) = 0;

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname ) = 0;

	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

	int CanBeUsedAsCache( const CGraphicsResourceDesc& desc );

	inline virtual LPDIRECT3DTEXTURE9 GetTexture() { return NULL; }

	const CGraphicsResourceDesc& GetDesc() const { return m_TextureDesc; }

	/// Save the texture to disk as an image file
	/// - For debugging
	/// - Use this if the texture is loaded from file and filename is stored in GetDesc().ResourcePath
	///   - Wait! It will overwrite the original image file!
//	bool SaveTextureToImageFile(); commented out

	/// Save the texture to disk as an image file
	/// - For debugging
	/// - NOTE: image_filepath should be different from GetDesc().ResourcePath,
	///         or it will overwrite the original image file
	virtual bool SaveTextureToImageFile( const std::string& image_filepath ) { return false; }

	virtual bool Lock() { return false; }

	virtual bool Unlock() { return false; }

	/// Creates an empty texture from the current desc.
	/// Called by the render thread.
	virtual bool Create() = 0;

	/// Returns true on success
	/// - Succeeds only between a pair of Lock() and Unlock() calls
	/// - Returns an object that provides access to the locked texture surface
	bool GetLockedTexture( CLockedTexture& texture );

	void GetStatus( char *pDestBuffer );

	friend class CGraphicsResourceManager;
};


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

	GraphicsResourceState::Name GetSubResourceState( CMeshSubResource::Name subresource ) const { return m_aSubResourceState[subresource]; }

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


//---------------------------- inline implementations ----------------------------

//================================================================================
// CGraphicsResource
//================================================================================

inline void CGraphicsResource::SetState( GraphicsResourceState::Name state )
{
	boost::mutex::scoped_lock scoped_lock(m_StateChangeLock);

	m_State = state;
}


inline GraphicsResourceState::Name CGraphicsResource::GetState()
{
	boost::mutex::scoped_lock scoped_lock(m_StateChangeLock);

	GraphicsResourceState::Name current_state = m_State;

	return current_state;
}


//================================================================================
// CGraphicsResource
//================================================================================

inline boost::shared_ptr<CD3DXMeshObjectBase> CMeshResource::GetMesh()
{
	if( GetState() == GraphicsResourceState::LOADED )
		return m_pMeshObject;
	else
		return boost::shared_ptr<CD3DXMeshObjectBase>();
}


inline boost::shared_ptr<CD3DXMeshObjectBase> CMeshResource::GetMeshInLoading()
{
	return m_pMeshObject;
}



#endif /* __GraphicsResources_HPP__ */
