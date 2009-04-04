#ifndef  __GraphicsResourceEntry_H__
#define  __GraphicsResourceEntry_H__


#include <sys/stat.h>
#include <string>
#include <d3dx9tex.h>
#include <boost/weak_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#include "fwd.hpp"
#include "GraphicsResource.hpp"
#include "GraphicsResourceDescs.hpp"

#include "Support/Serialization/BinaryDatabase.hpp"
using namespace GameLib1::Serialization;

//template<class T>class CBinaryDatabase<T>;


/**
 - base class of the graphics resources
   - derived classes are,
     - CTextureEntry: texture resource
	 - CMeshResource: mesh (3D model) resource
	 - CShaderManagerEntry: shader resource
   - each derived class has derived class instance of resource desc as one of their member variables

*/
class CGraphicsResourceEntry
{
public:

	enum State
	{
		STATE_RESERVED,
		STATE_RELEASED,
		NUM_STATES
	};

private:

	State m_State;

	int m_iRefCount;

	boost::shared_ptr<CGraphicsResource> m_pResource;

	/// cache derived class pointers to avoid dynamic casting during runtime
	boost::shared_ptr<CTextureResource> m_pTextureResource;
	boost::shared_ptr<CMeshResource> m_pMeshResource;
	boost::shared_ptr<CShaderResource> m_pShaderResource;

	/// holds a copy of desc
	boost::shared_ptr<CGraphicsResourceDesc> m_pDesc;

public:

	inline CGraphicsResourceEntry();

	inline void IncRefCount();

	inline void DecRefCount();

	int GetRefCount() const { return m_iRefCount; }

	State GetState() const { return m_State; }

	boost::shared_ptr<CGraphicsResource> GetResource() { return m_pResource; }

	inline void SetResource( boost::shared_ptr<CGraphicsResource> pResource );

	boost::shared_ptr<CTextureResource> GetTextureResource() { return m_pTextureResource; }
	boost::shared_ptr<CMeshResource> GetMeshResource() { return m_pMeshResource; }
	boost::shared_ptr<CShaderResource> GetShaderResource() { return m_pShaderResource; }

	const boost::shared_ptr<CTextureResource> GetTextureResource() const { return m_pTextureResource; }
	const boost::shared_ptr<CMeshResource> GetMeshResource() const { return m_pMeshResource; }
	const boost::shared_ptr<CShaderResource> GetShaderResource() const { return m_pShaderResource; }

	void GetStatus( char *pDestBuffer );

	friend class CGraphicsResourceManager;

//	boost::shared_ptr<T> GetResource() { return m_pResource; }
//	void SetResource( boost::shared_ptr<T> pResource ) { m_pResource = pResource; }

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
	LPDIRECT3DTEXTURE9 m_pTexture;

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
	bool CreateFromDesc();

	void UpdateDescForCachedResource( const CGraphicsResourceDesc& desc );

public:

	CTextureResource( const CTextureResourceDesc *pDesc );

	virtual ~CTextureResource();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Texture; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

	int CanBeUsedAsCache( const CGraphicsResourceDesc& desc );

	inline LPDIRECT3DTEXTURE9 GetTexture()
	{
		if( GetState() == GraphicsResourceState::LOADED )
			return m_pTexture;
		else
			return NULL;
	}

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
	bool SaveTextureToImageFile( const std::string& image_filepath );

	bool Lock();

	bool Unlock();

	/// Creates an empty texture from the current desc.
	/// Called by the render thread.
	bool Create();

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

public:

	CMeshResource( const CMeshResourceDesc *pDesc );

	virtual ~CMeshResource();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Mesh; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

	int CanBeUsedAsCache( const CGraphicsResourceDesc& desc );

	inline boost::shared_ptr<CD3DXMeshObjectBase> GetMesh() { return m_pMeshObject; }

	CMeshType::Name GetMeshType() const { return m_MeshDesc.MeshType; }

	const CGraphicsResourceDesc& GetDesc() const { return m_MeshDesc; }

	void GetStatus( char *pDestBuffer );

//	bool Lock();

//	bool Unlock();

	bool Create();

	void SetSubResourceState( CMeshSubResource::Name subresource, GraphicsResourceState::Name state );

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
// CGraphicsResourceEntry
//================================================================================

inline CGraphicsResourceEntry::CGraphicsResourceEntry()
:
m_State(STATE_RELEASED),
m_iRefCount(0)
{
}


/**
 Load the resource when the reference count is incremented from 0 to 1
*/
inline void CGraphicsResourceEntry::IncRefCount()
{
	m_iRefCount++;

/*	if( m_iRefCount == 0 )
	{
		bool res = Load();
		if( res )
			m_iRefCount = 1;
	}
	else
	{
		// resource has already been loaded - just increment the reference count
		m_iRefCount++;
	}*/
}


/**
 Release the resource when the reference count is decremented from 1 to 0
*/
inline void CGraphicsResourceEntry::DecRefCount()
{
	if( m_iRefCount == 0 )
	{
		std::string resource_path_info;
		if( GetResource() )
			resource_path_info = "(resource name: " + GetResource()->GetDesc().ResourcePath + ")";
		
		LOG_PRINT_WARNING( " - A redundant call: ref count is already 0 " + resource_path_info );
		return;	// error
	}

	m_iRefCount--;

	if( m_iRefCount == 0 )
	{
		if( GetResource() ) // Do not release cached resources
			GetResource()->ReleaseNonChachedResource();

		m_State = STATE_RELEASED;
	}
}


inline void CGraphicsResourceEntry::SetResource( boost::shared_ptr<CGraphicsResource> pResource )
{
	m_pResource = pResource;

	m_pTextureResource = boost::dynamic_pointer_cast<CTextureResource,CGraphicsResource>(pResource);
	m_pMeshResource    = boost::dynamic_pointer_cast<CMeshResource,CGraphicsResource>(pResource);
	m_pShaderResource  = boost::dynamic_pointer_cast<CShaderResource,CGraphicsResource>(pResource);
}


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


#endif  /* __GraphicsResourceEntry_H__ */
