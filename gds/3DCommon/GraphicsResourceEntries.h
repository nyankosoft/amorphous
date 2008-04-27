#ifndef  __GraphicsResourceEntry_H__
#define  __GraphicsResourceEntry_H__


#include <sys/stat.h>

#include <string>

#include <d3dx9tex.h>

class CD3DXMeshObjectBase;

//#include "3DCommon/D3DXMeshObjectBase.h"


class CGraphicsResourceManager;


class CGraphicsResourceDesc
{
public:

	enum eResourceTypes
	{
		RT_TEXTURE,
		RT_MESHOBJECT,
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

	virtual bool Load() = 0;

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

	virtual bool Load();

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

//	CMeshObjectEntry() : m_pMeshObject(NULL), m_MeshType(CD3DXMeshObjectBase::TYPE_MESH) {}
//	CMeshObjectEntry( const char *pMeshObjectFilename, int meshtype = CD3DXMeshObjectBase::TYPE_MESH );

	CMeshObjectEntry();

	CMeshObjectEntry( int mesh_type );

	virtual ~CMeshObjectEntry();

	virtual int GetResourceType() const { return CGraphicsResourceDesc::RT_MESHOBJECT; }

	virtual bool Load();

	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

	inline CD3DXMeshObjectBase *GetMeshObject() { return m_pMeshObject; }

	friend class CGraphicsResourceManager;
};


#endif  /* __GraphicsResourceEntry_H__ */
