#ifndef  __GraphicsResourceEntry_H__
#define  __GraphicsResourceEntry_H__


#include "GraphicsResource.h"

#include <sys/stat.h>
#include <string>
#include <d3dx9tex.h>
#include <boost/weak_ptr.hpp>

#include "Support/Serialization/BinaryDatabase.h"
using namespace GameLib1::Serialization;

class CD3DXMeshObjectBase;
class CShaderManager;
class CGraphicsResourceManager;

//template<class T>class CBinaryDatabase<T>;


class CGraphicsResourceDesc
{
public:

	enum eResourceTypes
	{
		RT_TEXTURE,
		RT_MESHOBJECT,
		RT_SHADER,
		//RT_FONT,
		NUM_RESOURCE_TYPES
	};

	std::string Filename;

public:

	CGraphicsResourceDesc();

	virtual int GetResourceType() const = 0;
};


class CMeshResourceDesc : public CGraphicsResourceDesc
{
public:

	int MeshType;	///< used by mesh object

public:

	CMeshResourceDesc();

	virtual int GetResourceType() const { return RT_MESHOBJECT; }
};


class CTextureResourceDesc : public CGraphicsResourceDesc
{
public:

	int Width;
	int Height;
	int MipLevels; ///< 0 is set to create complete mipmap chain. (default: 0)
	TextureFormat::Format Format;

	boost::weak_ptr<CTextureLoader> pLoader;

public:

	CTextureResourceDesc()
		:
	Width(0),
	Height(0),
	MipLevels(0),
	Format(TextureFormat::Invalid)
	{}

	virtual int GetResourceType() const { return RT_TEXTURE; }
};


class CShaderResourceDesc : public CGraphicsResourceDesc
{
public:

	virtual int GetResourceType() const { return RT_SHADER; }
};


class CGraphicsResourceEntry
{
protected:

	unsigned int m_OptionFlags;

	std::string m_Filename;

	int m_iRefCount;

	/// stores the time when the file was updated last
	time_t m_LastModifiedTimeOfFile;

protected:

	// reference count is not changed in this function?
	virtual void Release() = 0;

	virtual bool IsDiskResource() const { return true; }

	/// load resource from a file or a binary database
	bool LoadFromDisk();

	/// Added to create empty texture as a graphics resource
	virtual bool CreateFromDesc() { return false; }

public:

	CGraphicsResourceEntry();

	virtual ~CGraphicsResourceEntry();

	virtual int GetResourceType() const = 0;

	bool Load();

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

	friend class CGraphicsResourceManager;
};


class CTextureEntry : public CGraphicsResourceEntry
{
	LPDIRECT3DTEXTURE9 m_pTexture;

	CTextureResourceDesc m_TextureDesc;

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

	virtual int GetResourceType() const { return CGraphicsResourceDesc::RT_TEXTURE; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	inline LPDIRECT3DTEXTURE9 GetTexture() { return m_pTexture; }

	friend class CGraphicsResourceManager;
};


class CMeshObjectEntry : public CGraphicsResourceEntry
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

	virtual int GetResourceType() const { return CGraphicsResourceDesc::RT_MESHOBJECT; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

	inline CD3DXMeshObjectBase *GetMeshObject() { return m_pMeshObject; }

	int GetMeshType() const { return m_MeshDesc.MeshType; }

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

	virtual int GetResourceType() const { return CGraphicsResourceDesc::RT_SHADER; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

//	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

	inline CShaderManager *GetShaderManager() { return m_pShaderManager; }

	friend class CGraphicsResourceManager;
};


#endif  /* __GraphicsResourceEntry_H__ */
