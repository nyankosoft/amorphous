#ifndef __MeshFactory_HPP__
#define __MeshFactory_HPP__


#include "BasicMesh.hpp"


class CMeshImplFactory
{
public:

	CMeshImplFactory() {}
	virtual ~CMeshImplFactory() {}

//	virtual CMeshImpl* CreateMeshImpl( CMeshType::Name mesh_type ) = 0;

//	boost::shared_ptr<CMeshImpl> CreateMesh( CMeshType::Name mesh_type = CMeshType::SKELETAL );// = 0; //{ return boost::shared_ptr<CMeshImpl>(); }

	virtual CMeshImpl* CreateBasicMeshImpl() { return NULL; }
	virtual CMeshImpl* CreateProgressiveMeshImpl() { return NULL; }
	virtual CMeshImpl* CreateSkeletalMeshImpl() { return NULL; }

/*
	CMeshImpl* LoadMeshObjectFromFile( const std::string& filepath,
		                               U32 load_option_flags = 0,
		                               CMeshType::Name mesh_type = CMeshType::SKELETAL );

	/// TODO: support PMesh and SMesh
	CMeshImpl*  LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
		                                 const std::string& filepath,
										 U32 load_option_flags = 0,
										 CMeshType::Name mesh_type = CMeshType::SKELETAL );
*/
};


inline boost::shared_ptr<CMeshImplFactory>& MeshImplFactory()
{
	static boost::shared_ptr<CMeshImplFactory> s_pMeshFactory;
	return s_pMeshFactory;
}


class CMeshFactory
{
public:

//	virtual boost::shared_ptr<CBasicMesh> CreateBasicMeshImpl() = 0;
//	virtual boost::shared_ptr<CProgressiveMesh> CreateProgressiveMeshImpl() = 0;
//	virtual boost::shared_ptr<CSkeletalMesh> CreateSkeletalMeshImpl() = 0;
/*	virtual boost::shared_ptr<CMeshImpl> CreateBasicMeshImpl() = 0;
	virtual boost::shared_ptr<CMeshImpl> CreateProgressiveMeshImpl() = 0;
	virtual boost::shared_ptr<CMeshImpl> CreateSkeletalMeshImpl() = 0;
*/
	CBasicMesh *CreateMeshInstance( CMeshType::Name mesh_type = CMeshType::SKELETAL );

	boost::shared_ptr<CBasicMesh> CreateMesh( CMeshType::Name mesh_type = CMeshType::SKELETAL );

	CBasicMesh *CreateBasicMeshInstance();
	CProgressiveMesh *CreateProgressiveMeshInstance();
	CSkeletalMesh *CreateSkeletalMeshInstance();

	boost::shared_ptr<CBasicMesh> CreateBasicMesh();
	boost::shared_ptr<CProgressiveMesh> CreateProgressiveMesh();
	boost::shared_ptr<CSkeletalMesh> CreateSkeletalMesh();

	CBasicMesh* LoadMeshObjectFromFile( const std::string& filepath,
		                                U32 load_option_flags = 0,
		                                CMeshType::Name mesh_type = CMeshType::SKELETAL );

	/// TODO: support PMesh and SMesh
	CBasicMesh* LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
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

	boost::shared_ptr<CBasicMesh> CreateMesh( CMeshType::Name mesh_type = CMeshType::SKELETAL );

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


#endif /* __MeshFactory_HPP__ */
