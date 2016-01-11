#ifndef  __MeshObjectHandle_H__
#define  __MeshObjectHandle_H__


#include "fwd.hpp"
#include "GraphicsResourceHandle.hpp"


namespace amorphous
{


class MeshHandle : public GraphicsResourceHandle
{
protected:

	static const MeshHandle ms_NullHandle;

public:

	inline MeshHandle() {}

	~MeshHandle() { Release(); }

	GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Mesh; }

	MeshType::Name GetMeshType();

	inline boost::shared_ptr<BasicMesh> GetMesh();

	inline boost::shared_ptr<const BasicMesh> GetMesh() const;

	boost::shared_ptr<SkeletalMesh> GetSkeletalMesh();

	bool Load( const std::string& resource_path );

	bool Load( const MeshResourceDesc& desc );

	boost::shared_ptr<MeshResource> GetMeshResource();

	inline virtual const MeshHandle &operator=( const MeshHandle& handle );

	static const MeshHandle& Null() { return ms_NullHandle; }
};


//--------------------------------- inline implementations ---------------------------------

inline const MeshHandle &MeshHandle::operator=( const MeshHandle& handle )
{
	GraphicsResourceHandle::operator=(handle);

	return *this;
}


inline boost::shared_ptr<BasicMesh> MeshHandle::GetMesh()
{
	if( GetEntry()
	 && GetEntry()->GetMeshResource() )
		return GetEntry()->GetMeshResource()->GetMesh();
	else
		return boost::shared_ptr<BasicMesh>();
}


inline boost::shared_ptr<const BasicMesh> MeshHandle::GetMesh() const
{
	if( GetEntry()
		&& GetEntry()->GetMeshResource() )
		return GetEntry()->GetMeshResource()->GetMesh();
	else
		return boost::shared_ptr<const BasicMesh>();
}


} // namespace amorphous



#endif  /* __MeshObjectHandle_H__ */
