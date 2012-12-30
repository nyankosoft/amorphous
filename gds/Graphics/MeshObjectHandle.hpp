#ifndef  __MeshObjectHandle_H__
#define  __MeshObjectHandle_H__


#include "fwd.hpp"
#include "GraphicsResourceHandle.hpp"


namespace amorphous
{


class CMeshObjectHandle : public CGraphicsResourceHandle
{
protected:

	static const CMeshObjectHandle ms_NullHandle;

public:

	inline CMeshObjectHandle() {}

	~CMeshObjectHandle() { Release(); }

	GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Mesh; }

	CMeshType::Name GetMeshType();

	inline boost::shared_ptr<CBasicMesh> GetMesh();

	boost::shared_ptr<CSkeletalMesh> GetSkeletalMesh();

	bool Load( const std::string& resource_path );

	bool Load( const CMeshResourceDesc& desc );

	boost::shared_ptr<CMeshResource> GetMeshResource();

	inline virtual const CMeshObjectHandle &operator=( const CMeshObjectHandle& handle );

	static const CMeshObjectHandle& Null() { return ms_NullHandle; }
};


//--------------------------------- inline implementations ---------------------------------

inline const CMeshObjectHandle &CMeshObjectHandle::operator=( const CMeshObjectHandle& handle )
{
	CGraphicsResourceHandle::operator=(handle);

	return *this;
}


inline boost::shared_ptr<CBasicMesh> CMeshObjectHandle::GetMesh()
{
	if( GetEntry()
	 && GetEntry()->GetMeshResource() )
		return GetEntry()->GetMeshResource()->GetMesh();
	else
		return boost::shared_ptr<CBasicMesh>();
}

} // namespace amorphous



#endif  /* __MeshObjectHandle_H__ */
