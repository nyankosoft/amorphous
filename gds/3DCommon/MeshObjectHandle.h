#ifndef  __MeshObjectHandle_H__
#define  __MeshObjectHandle_H__


#include "GraphicsResourceHandle.h"
#include "GraphicsResourceManager.h"

#include "3DCommon/D3DXMeshObjectBase.h"

#include <d3dx9tex.h>
//class CD3DXMeshObjectBase;

#include "Support/Serialization/ArchiveObjectBase.h"
using namespace GameLib1::Serialization;


class CMeshObjectHandle : public CGraphicsResourceHandle
{
protected:

	int m_MeshType;

public:

	inline CMeshObjectHandle( int mesh_type = CD3DXMeshObjectBase::TYPE_MESH ) : m_MeshType(mesh_type) {}

	~CMeshObjectHandle() {}

	GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Mesh; }

	int GetMeshType() const { return m_MeshType; }

	void SetMeshType( int mesh_type ) { m_MeshType = mesh_type; }

	inline CD3DXMeshObjectBase *GetMeshObject() { return GraphicsResourceManager().GetMeshObject(m_EntryID); }

	virtual bool Load();

	inline virtual const CMeshObjectHandle &operator=( const CMeshObjectHandle& handle );

	virtual void Serialize( IArchive& ar, const unsigned int version );
};


inline const CMeshObjectHandle &CMeshObjectHandle::operator=( const CMeshObjectHandle& handle )
{
	CGraphicsResourceHandle::operator=(handle);

	m_MeshType = handle.m_MeshType;

	return *this;
}



#endif  /* __MeshObjectHandle_H__ */
