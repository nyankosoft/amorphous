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

public:

	inline CMeshObjectHandle() {}

	~CMeshObjectHandle() { Release(); }

	GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Mesh; }

	CMeshType::Name GetMeshType();

	inline boost::shared_ptr<CD3DXMeshObjectBase> GetMesh();

	bool Load( const std::string& resource_path );

	bool Load( const CMeshResourceDesc& desc );

	boost::shared_ptr<CMeshResource> GetMeshResource();

	inline virtual const CMeshObjectHandle &operator=( const CMeshObjectHandle& handle );
};


//--------------------------------- inline implementations ---------------------------------

inline const CMeshObjectHandle &CMeshObjectHandle::operator=( const CMeshObjectHandle& handle )
{
	CGraphicsResourceHandle::operator=(handle);

	return *this;
}


inline boost::shared_ptr<CD3DXMeshObjectBase> CMeshObjectHandle::GetMesh()
{
	if( GetEntry()
	 && GetEntry()->GetMeshResource() )
		return GetEntry()->GetMeshResource()->GetMesh();
	else
		return boost::shared_ptr<CD3DXMeshObjectBase>();
}


#endif  /* __MeshObjectHandle_H__ */
