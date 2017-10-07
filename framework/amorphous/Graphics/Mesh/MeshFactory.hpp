#ifndef __amorphous_MeshFactory_HPP__
#define __amorphous_MeshFactory_HPP__


#include "BasicMesh.hpp"


namespace amorphous
{


class MeshImplFactory
{
public:

	MeshImplFactory() {}
	virtual ~MeshImplFactory() {}

//	virtual MeshImpl* CreateMeshImpl( MeshType::Name mesh_type ) = 0;

	virtual MeshImpl* CreateBasicMeshImpl() { return NULL; }
	virtual MeshImpl* CreateProgressiveMeshImpl() { return NULL; }
	virtual MeshImpl* CreateSkeletalMeshImpl() { return NULL; }
};


inline std::shared_ptr<MeshImplFactory>& GetMeshImplFactory()
{
	static std::shared_ptr<MeshImplFactory> s_pMeshFactory;
	return s_pMeshFactory;
}


class MeshFactory
{
	BasicMesh *InitMeshInstance( MeshTypeName mesh_type, U32 load_option_flags );

public:

//	virtual std::shared_ptr<BasicMesh> CreateBasicMeshImpl() = 0;
//	virtual std::shared_ptr<ProgressiveMesh> CreateProgressiveMeshImpl() = 0;
//	virtual std::shared_ptr<SkeletalMesh> CreateSkeletalMeshImpl() = 0;
/*	virtual std::shared_ptr<MeshImpl> CreateBasicMeshImpl() = 0;
	virtual std::shared_ptr<MeshImpl> CreateProgressiveMeshImpl() = 0;
	virtual std::shared_ptr<MeshImpl> CreateSkeletalMeshImpl() = 0;
*/
	BasicMesh *CreateMeshInstance( MeshTypeName mesh_type = MeshTypeName::SKELETAL );

	std::shared_ptr<BasicMesh> CreateMesh( MeshTypeName mesh_type = MeshTypeName::SKELETAL );

	BasicMesh *CreateBasicMeshInstance();
	ProgressiveMesh *CreateProgressiveMeshInstance();
	SkeletalMesh *CreateSkeletalMeshInstance();

	std::shared_ptr<BasicMesh> CreateBasicMesh();
	std::shared_ptr<ProgressiveMesh> CreateProgressiveMesh();
	std::shared_ptr<SkeletalMesh> CreateSkeletalMesh();

	BasicMesh* LoadMeshObjectFromFile( const std::string& filepath,
		                                U32 load_option_flags = 0,
		                                MeshTypeName mesh_type = MeshTypeName::SKELETAL );

	/// TODO: support PMesh and SMesh
	BasicMesh* LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
		                                    const std::string& filepath,
											U32 load_option_flags = 0,
											MeshTypeName mesh_type = MeshTypeName::SKELETAL );
};


} // namespace amorphous


#endif /* __amorphous_MeshFactory_HPP__ */
