#ifndef __MeshFactory_HPP__
#define __MeshFactory_HPP__


#include "BasicMesh.hpp"


namespace amorphous
{


class MeshImplFactory
{
public:

	MeshImplFactory() {}
	virtual ~MeshImplFactory() {}

//	virtual MeshImpl* CreateMeshImpl( CMeshType::Name mesh_type ) = 0;

//	boost::shared_ptr<MeshImpl> CreateMesh( CMeshType::Name mesh_type = CMeshType::SKELETAL );// = 0; //{ return boost::shared_ptr<MeshImpl>(); }

	virtual MeshImpl* CreateBasicMeshImpl() { return NULL; }
	virtual MeshImpl* CreateProgressiveMeshImpl() { return NULL; }
	virtual MeshImpl* CreateSkeletalMeshImpl() { return NULL; }

/*
	MeshImpl* LoadMeshObjectFromFile( const std::string& filepath,
		                               U32 load_option_flags = 0,
		                               CMeshType::Name mesh_type = CMeshType::SKELETAL );

	/// TODO: support PMesh and SMesh
	MeshImpl*  LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
		                                 const std::string& filepath,
										 U32 load_option_flags = 0,
										 CMeshType::Name mesh_type = CMeshType::SKELETAL );
*/
};


inline boost::shared_ptr<MeshImplFactory>& GetMeshImplFactory()
{
	static boost::shared_ptr<MeshImplFactory> s_pMeshFactory;
	return s_pMeshFactory;
}


class CMeshFactory
{
public:

//	virtual boost::shared_ptr<BasicMesh> CreateBasicMeshImpl() = 0;
//	virtual boost::shared_ptr<ProgressiveMesh> CreateProgressiveMeshImpl() = 0;
//	virtual boost::shared_ptr<SkeletalMesh> CreateSkeletalMeshImpl() = 0;
/*	virtual boost::shared_ptr<MeshImpl> CreateBasicMeshImpl() = 0;
	virtual boost::shared_ptr<MeshImpl> CreateProgressiveMeshImpl() = 0;
	virtual boost::shared_ptr<MeshImpl> CreateSkeletalMeshImpl() = 0;
*/
	BasicMesh *CreateMeshInstance( CMeshType::Name mesh_type = CMeshType::SKELETAL );

	boost::shared_ptr<BasicMesh> CreateMesh( CMeshType::Name mesh_type = CMeshType::SKELETAL );

	BasicMesh *CreateBasicMeshInstance();
	ProgressiveMesh *CreateProgressiveMeshInstance();
	SkeletalMesh *CreateSkeletalMeshInstance();

	boost::shared_ptr<BasicMesh> CreateBasicMesh();
	boost::shared_ptr<ProgressiveMesh> CreateProgressiveMesh();
	boost::shared_ptr<SkeletalMesh> CreateSkeletalMesh();

	BasicMesh* LoadMeshObjectFromFile( const std::string& filepath,
		                                U32 load_option_flags = 0,
		                                CMeshType::Name mesh_type = CMeshType::SKELETAL );

	/// TODO: support PMesh and SMesh
	BasicMesh* LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
		                                    const std::string& filepath,
											U32 load_option_flags = 0,
											CMeshType::Name mesh_type = CMeshType::SKELETAL );
};

/*
inline CMeshFactory& MeshFactory()
{
	static boost::shared_ptr<CMeshFactory> s_pMeshFactory;
	if( s_pMeshFactory == boost::shared_ptr<CMeshFactory>() )
	{
		InitMeshFactory( s_pMeshFactory );
	}

	return *(MeshFactoryPtr().get());
}*/



inline boost::shared_ptr<CMeshFactory>& MeshFactoryPtr()
{
	static boost::shared_ptr<CMeshFactory> s_pMeshFactory;
	return s_pMeshFactory;
}


// MeshFactoryPtr() must be initialized before calling this
inline CMeshFactory& MeshFactory()
{
	return *(MeshFactoryPtr().get());
}


/*
class CMeshObjectFactory
{
public:

	CMeshObjectFactory() {}
	virtual ~CMeshObjectFactory() {}

	boost::shared_ptr<BasicMesh> CreateMesh( CMeshType::Name mesh_type = CMeshType::SKELETAL );

	CD3DXMeshObjectBase* LoadMeshObjectFromFile( const std::string& filepath,
		                                         U32 load_option_flags = 0,
		                                         CMeshType::Name mesh_type = CMeshType::SKELETAL );

	/// TODO: support PMesh and SMesh
	CD3DXMeshObjectBase*  LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
		                                             const std::string& filepath,
													 U32 load_option_flags = 0,
													 CMeshType::Name mesh_type = CMeshType::SKELETAL );
};
*/

} // namespace amorphous



#endif /* __MeshFactory_HPP__ */
