#ifndef  __GraphicsResourceEntry_H__
#define  __GraphicsResourceEntry_H__


#include "GraphicsResource.h"

#include <sys/stat.h>
#include <string>
#include <d3dx9tex.h>

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
		RT_SHADERMANAGER,
		//RT_FONT,
		NUM_RESOURCE_TYPES
	};

	CGraphicsResourceDesc( int resource_type );

	int ResourceType;

	std::string Filename;

	int MeshType;	///< used by mesh object
};


class CGraphicsResourceEntry
{
protected:

	unsigned int m_OptionFlags;

	std::string m_Filename;

	int m_iRefCount;

	/// stores the time when the file was updated last
	time_t m_LastModifiedTimeOfFile;

	// reference count is not changed in this function?
	virtual void Release() = 0;

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

	/// release texture without changing the reference count
	/// called only from CTextureManager
	/// and from CTextureEntry if Refresh() is used
	virtual void Release();

public:

	CTextureEntry();

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

	int m_MeshType;

	/// release mesh object without changing the reference count
	/// called only from this class and CMeshObjectManager
	virtual void Release();

public:

	CMeshObjectEntry();

	CMeshObjectEntry( int mesh_type );

	virtual ~CMeshObjectEntry();

	virtual int GetResourceType() const { return CGraphicsResourceDesc::RT_MESHOBJECT; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

	inline CD3DXMeshObjectBase *GetMeshObject() { return m_pMeshObject; }

	friend class CGraphicsResourceManager;
};


class CShaderManagerEntry : public CGraphicsResourceEntry
{
	CShaderManager *m_pShaderManager;

	/// Release the shader manager without changing the reference count
	/// - Called only from this class and CMeshObjectManager
	virtual void Release();

public:

	CShaderManagerEntry();

	virtual ~CShaderManagerEntry();

	virtual int GetResourceType() const { return CGraphicsResourceDesc::RT_SHADERMANAGER; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

//	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

	inline CShaderManager *GetShaderManager() { return m_pShaderManager; }

	friend class CGraphicsResourceManager;
};

#endif  /* __GraphicsResourceEntry_H__ */
