#ifndef  __MeshObjectHandle_H__
#define  __MeshObjectHandle_H__


#include "fwd.h"
#include "GraphicsResourceHandle.h"
#include "GraphicsResourceManager.h"

#include "Support/Serialization/ArchiveObjectBase.h"
using namespace GameLib1::Serialization;


class CMeshObjectHandle : public CGraphicsResourceHandle
{
protected:

	virtual void IncResourceRefCount();
	virtual void DecResourceRefCount();

public:

	inline CMeshObjectHandle() {}

	~CMeshObjectHandle() { Release(); }

	inline void Release();

	GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Mesh; }

	CMeshType::Name GetMeshType() const;

//	void SetMeshType( int mesh_type ) { m_MeshType = mesh_type; }

	inline CD3DXMeshObjectBase *GetMeshObject() { return GraphicsResourceManager().GetMeshObject(m_EntryID); }

	bool Load( const std::string& resource_path );

	bool Load( const CMeshResourceDesc& desc );

	inline virtual const CMeshObjectHandle &operator=( const CMeshObjectHandle& handle );
};


//--------------------------------- inline implementations ---------------------------------

inline const CMeshObjectHandle &CMeshObjectHandle::operator=( const CMeshObjectHandle& handle )
{
	CGraphicsResourceHandle::operator=(handle);

	return *this;
}


inline void CMeshObjectHandle::Release()
{
	if( 0 <= m_EntryID )
	{
		DecResourceRefCount();
		m_EntryID = -1;
	}
}


#endif  /* __MeshObjectHandle_H__ */
