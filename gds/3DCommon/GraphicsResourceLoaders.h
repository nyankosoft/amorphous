#ifndef  __GraphicsResourceLoaders_H__
#define  __GraphicsResourceLoaders_H__


#include "3DCommon/fwd.h"
using namespace MeshModel;
#include "GraphicsResource.h"
#include "GraphicsResourceEntries.h"
#include "Support/fnop.h"
#include "Support/SafeDelete.h"
#include "Support/BitmapImage.h"
#include "Support/Serialization/BinaryDatabase.h"
using namespace GameLib1::Serialization;

/*
template<typename T>
class heap_array
{
	int size;
	T *pArray
	
public:
	
	heap_array() : size(0), pArray(NULL) {}

	heap_array( int _size ) : size(_size), pArray(new T [size]) {}
	
	~heap_array() { SafeDeleteArray(pArray); }

	void resize( int _size )
	{
		SafeDeleteArray(pArray);
		size = _size;
		
		if( 0 < size)
			pArray = new T [size];
	}
	
	void clear() { resize(0); }

	int size() const { return size; }
	
	T* Buffer() { return pArray; }
}
*/

inline bool is_db_filepath_and_keyname( const std::string& filepath )
{
	return ( filepath.find("::") != std::string::npos );
}

inline bool decompose_into_db_filepath_and_keyname( const std::string& src_filepath,
												    std::string& db_filepath,
												    std::string& keyname
												    )
{
	const size_t pos = src_filepath.find( "::" );
	if( pos == std::string::npos )
		return false;

	// found "::" in filename
	// - "(binary database filename)::(key)"
	db_filepath = src_filepath.substr( 0, pos );
	keyname     = src_filepath.substr( pos + 2, src_filepath.length() );

	return true;
}



const std::string g_NullString = "";


class CGraphicsResourceLoader
{
protected:

	virtual const std::string& GetSourceFilepath() = 0;

public:

	bool LoadFromDisk();

	virtual bool LoadFromFile( const std::string& filepath ) = 0;

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname ) = 0;

	/// Called by the resource IO thread
	/// - copy the loaded resource to locked buffer
	virtual bool CopyTo( CGraphicsResourceEntry *pDestEntry ) { return false; }

	virtual bool Lock() = 0;

	virtual bool Unlock() = 0;
};


/// loads a texture from disk
class CDiskTextureLoader : public CGraphicsResourceLoader
{
	/// entry to that stores the loaded resource
	boost::weak_ptr<CTextureEntry> m_pTextureEntry;

//	CTextureResourceDesc m_Desc;

	/// stores texture data loaded from disk
	CBitmapImage m_Image;

protected:

	const std::string& GetSourceFilepath()
	{
		boost::shared_ptr<CTextureEntry> pEntry = m_pTextureEntry.lock();
		if( pEntry )
			return pEntry->GetFilename();
		else
			return g_NullString;
	}

public:

	CDiskTextureLoader( boost::weak_ptr<CTextureEntry> pTextureEntry )
		:
	m_pTextureEntry(pTextureEntry)
	{}

	bool LoadFromFile( const std::string& filepath );

	/// load image from the db as an image archive
	bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	/// copy the bitmap image to the locked texture surface
	bool CopyTo( CGraphicsResourceEntry *pDestEntry );

	bool Lock()
	{
		boost::shared_ptr<CTextureEntry> pEntry = m_pTextureEntry.lock();
		if( pEntry )
			return pEntry->Lock();
		else
			return false;
	}

	bool Unlock()
	{
		boost::shared_ptr<CTextureEntry> pEntry = m_pTextureEntry.lock();
		if( pEntry )
			return pEntry->Unlock();
		else
			return false;
	}

	/// called by the system
	/// - called inside CopyTo()
	void FillTexture( CLockedTexture& texture );
};


class CMeshLoader : public CGraphicsResourceLoader
{
	/// entry to that stores the loaded resource
	boost::weak_ptr<CMeshObjectEntry> m_pMeshEntry;

//	CMeshResourceDesc m_Desc;

	boost::shared_ptr<C3DMeshModelArchive> m_pArchive;

protected:

	const std::string& GetSourceFilepath()
	{
		boost::shared_ptr<CMeshObjectEntry> pEntry = m_pMeshEntry.lock();
		if( pEntry )
			return pEntry->GetFilename();
		else
			return g_NullString;
	}

public:

	CMeshLoader( boost::weak_ptr<CMeshObjectEntry> pMeshEntry );

	bool LoadFromFile( const std::string& filepath );

	bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	bool CopyTo( CGraphicsResourceEntry *pDestEntry );

	/// Does not lock the mesh resource
	/// - Loaders for each component of the mesh (vertices, indices, and attribute buffers)
	///   do the lock & unlock, and copy each resource to the mesh
	bool Lock() { return false; }

	/// Does not unlock the mesh resource
	/// - See the comment of Lock()
	bool Unlock() { return false; }

	void CreateLockRequests()
	{
//		lock_unlock_queue.add( LockUnlockRequest( CD3DXMeshVerticesLoader, Lock ) );
//		lock_unlock_queue.add( LockUnlockRequest( CD3DXMeshIndicesLoader, Lock ) );
//		lock_unlock_queue.add( LockUnlockRequest( CD3DXMeshAttributeTableLoader, Lock ) );
	}
};


class CD3DXMeshLoaderBase : public CGraphicsResourceLoader
{
protected:
	/// entry to that stores the loaded resource
	boost::weak_ptr<CMeshObjectEntry> m_pMeshEntry;

//	CMeshResourceDesc m_Desc;

	boost::shared_ptr<C3DMeshModelArchive> m_pArchive;

protected:

	CD3DXMeshObjectBase *GetMesh()
	{
		boost::shared_ptr<CMeshObjectEntry> pMeshEntry = m_pMeshEntry.lock();
		if( pMeshEntry )
		{
			return pMeshEntry->GetMeshObject();
		}
		else
			return NULL;
	}

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

	CD3DXMeshVerticesLoader() : m_pLockedVertexBuffer(NULL), m_pVertexBufferContent(NULL) {}

	~CD3DXMeshVerticesLoader() { SafeDelete(m_pVertexBufferContent); }

	bool LoadFromArchive();

	bool CopyTo( CGraphicsResourceEntry *pDestEntry );

	// Lock the index buffer and save the pointer to the locked buffer
	bool Lock();

	bool Unlock();
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

	CD3DXMeshIndicesLoader() : m_pLockedIndexBuffer(NULL), m_pIndexBufferContent(NULL) {}

	~CD3DXMeshIndicesLoader() { SafeDelete(m_pIndexBufferContent); }

	bool LoadFromArchive();

	bool CopyTo( CGraphicsResourceEntry *pDestEntry );

	// Lock the index buffer and save the pointer to the locked buffer
	bool Lock();

	bool Unlock();
};


class CD3DXMeshAttributeTableLoader : public CD3DXMeshLoaderBase
{
	DWORD *m_pLockedAttributeBuffer;

protected:

//	const std::string& GetSourceFilepath() { return m_Desc.Filename; }

public:

	CD3DXMeshAttributeTableLoader() : m_pLockedAttributeBuffer(NULL) {}

	~CD3DXMeshAttributeTableLoader() {}

	bool LoadFromArchive();

	bool CopyTo( CGraphicsResourceEntry *pDestEntry );

	// Lock the index buffer and save the pointer to the locked buffer
	bool Lock();

	bool Unlock();
};


#endif  /* __GraphicsResourceLoaders_H__ */
