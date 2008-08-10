#ifndef  __GraphicsResourceEntry_H__
#define  __GraphicsResourceEntry_H__


#include <sys/stat.h>
#include <string>
#include <d3dx9tex.h>
#include <boost/weak_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#include "fwd.h"
#include "GraphicsResource.h"
#include "GraphicsResourceDescs.h"

#include "Support/Serialization/BinaryDatabase.h"
using namespace GameLib1::Serialization;

//template<class T>class CBinaryDatabase<T>;


/**
 - base class of the graphics resources
   - derived classes are,
     - CTextureEntry: texture resource
	 - CMeshObjectEntry: mesh (3D model) resource
	 - CShaderManagerEntry: shader resource
   - each derived class has derived class instance of resource desc as one of their member variables

*/
class CGraphicsResourceEntry
{
public:

protected:

	unsigned int m_OptionFlags;

	std::string m_Filename;

	int m_iRefCount;

	/// stores the time when the file was updated last
	time_t m_LastModifiedTimeOfFile;

	GraphicsResourceState::Name m_State;

	/// If true, kept in the array of cahced resources
	bool m_bIsCachedResource;

	int m_Index;

	boost::mutex m_StateChangeLock;

//	boost::shared_ptr<CGraphicsResource> m_pResource;

protected:

	// reference count is not changed in this function?
	virtual void Release() = 0;

	virtual bool IsDiskResource() const { return true; }

	/// load resource from a file or a binary database
	bool LoadFromDisk();

	/// Added to create empty texture as a graphics resource
	virtual bool CreateFromDesc() { return false; }

	void SetIndex( int index ) { m_Index = index; }

public:

	CGraphicsResourceEntry();

	virtual ~CGraphicsResourceEntry();

	virtual GraphicsResourceType::Name GetResourceType() const = 0;

	int GetIndex() const { return m_Index; }

	bool Load();

	bool LoadAsync();

	/// \param filepath [in] pathname of the file to load
	/// - This is usually m_Filename itself
	virtual bool LoadFromFile( const std::string& filepath ) = 0;

	/// \param db [in] The database which contains the target graphics resource
	/// \param keyname [in] Required to read the graphics resource from the db
	/// - These are the strings obtained by decomposing m_Filename to "(db_filepath)::(keyname)"
	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname ) = 0;

	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

	const std::string& GetFilename() const { return m_Filename; }

	void SetFilename( const std::string& filename ) { m_Filename = filename; }

	int GetRefCount() const { return m_iRefCount; }

	void IncRefCount();

	void DecRefCount();

	void Refresh();

	virtual const CGraphicsResourceDesc& GetDesc() const = 0;

	virtual bool Lock() { return false; }

	virtual bool Unlock() { return false; }

	inline void SetState( GraphicsResourceState::Name state );

	inline GraphicsResourceState::Name GetState();

	friend class CGraphicsResourceManager;
};

/*
class CGraphicsResource
{
public:

	virtual bool Lock() { return false; }

	virtual bool Unlock() { return false; }
};
*/

class CTextureEntry : public CGraphicsResourceEntry
{
	LPDIRECT3DTEXTURE9 m_pTexture;

	CTextureResourceDesc m_TextureDesc;

	boost::shared_ptr<CLockedTexture> m_pLockedTexture;

protected:

	/// release texture without changing the reference count
	/// called only from CTextureManager
	/// and from CTextureEntry if Refresh() is used
	virtual void Release();

	/// returns false if m_TextureDesc has valid width, height, and format 
	virtual bool IsDiskResource() const;

	/// create an empty texture
	/// - texture settings are read from m_TextureDesc
	bool CreateFromDesc();

public:

	CTextureEntry( const CTextureResourceDesc *pDesc );

	virtual ~CTextureEntry();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Texture; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	inline LPDIRECT3DTEXTURE9 GetTexture() { return m_pTexture; }

	const CGraphicsResourceDesc& GetDesc() const { return m_TextureDesc; }

	bool SaveTextureToImageFile( const std::string& image_filepath );

	bool Lock();

	bool Unlock();

	bool Create();

	/// Returns true on success
	/// - Succeeds only between a pair of Lock() and Unlock() calls
	/// - Returns an object that provides access to the locked texture surface
	bool GetLockedTexture( CLockedTexture& texture );

	friend class CGraphicsResourceManager;
};


class CMeshObjectEntry : public CGraphicsResourceEntry
//class CMeshObjectResource : public CGraphicsResource
{
	CD3DXMeshObjectBase *m_pMeshObject;

	CMeshResourceDesc m_MeshDesc;

protected:

	/// release mesh object without changing the reference count
	/// called only from this class and CMeshObjectManager
	virtual void Release();

public:

	CMeshObjectEntry( const CMeshResourceDesc *pDesc );

//	CMeshObjectEntry( int mesh_type );

	virtual ~CMeshObjectEntry();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Mesh; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

	inline CD3DXMeshObjectBase *GetMeshObject() { return m_pMeshObject; }

	CMeshType::Name GetMeshType() const { return m_MeshDesc.MeshType; }

	const CGraphicsResourceDesc& GetDesc() const { return m_MeshDesc; }

//	bool Lock();

//	bool Unlock();

//	bool Create();

	friend class CGraphicsResourceManager;
};


class CShaderManagerEntry : public CGraphicsResourceEntry
{
	CShaderManager *m_pShaderManager;

	CShaderResourceDesc m_ShaderDesc;

protected:

	/// Release the shader manager without changing the reference count
	/// - Called only from this class and CMeshObjectManager
	virtual void Release();

public:

	CShaderManagerEntry( const CShaderResourceDesc *pDesc );

	virtual ~CShaderManagerEntry();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Shader; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

//	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

	inline CShaderManager *GetShaderManager() { return m_pShaderManager; }

	const CGraphicsResourceDesc& GetDesc() const { return m_ShaderDesc; }

	friend class CGraphicsResourceManager;
};


//----------------------------------- inline implementations -----------------------------------

inline void CGraphicsResourceEntry::SetState( GraphicsResourceState::Name state )
{
	boost::mutex::scoped_lock scoped_lock(m_StateChangeLock);

	m_State = state;
}


inline GraphicsResourceState::Name CGraphicsResourceEntry::GetState()
{
	boost::mutex::scoped_lock scoped_lock(m_StateChangeLock);

	GraphicsResourceState::Name current_state = m_State;

	return current_state;
}


#endif  /* __GraphicsResourceEntry_H__ */
