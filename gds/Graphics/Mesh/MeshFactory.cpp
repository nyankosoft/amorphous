#include "MeshFactory.hpp"
#include "ProgressiveMesh.hpp"
#include "SkeletalMesh.hpp"
#include "CustomMesh.hpp"


namespace amorphous
{

using namespace boost;


void SetCustomMesh( BasicMesh& src_mesh )
{
	src_mesh.m_pImpl.reset( new CustomMesh );
}


//=============================================================================
// CMeshFactory
//=============================================================================

BasicMesh *CMeshFactory::CreateMeshInstance( CMeshType::Name mesh_type )
{
	switch( mesh_type )
	{
	case CMeshType::BASIC:
		return CreateBasicMeshInstance();
	case CMeshType::PROGRESSIVE:
		return CreateProgressiveMeshInstance();
	case CMeshType::SKELETAL:
		return CreateSkeletalMeshInstance();
	default:
		return NULL;
	}

	return NULL;
}


shared_ptr<BasicMesh> CMeshFactory::CreateMesh( CMeshType::Name mesh_type )
{
	return shared_ptr<BasicMesh>( CreateMeshInstance( mesh_type ) );
}


BasicMesh *CMeshFactory::CreateBasicMeshInstance() { return new BasicMesh; }
ProgressiveMesh *CMeshFactory::CreateProgressiveMeshInstance() { return new ProgressiveMesh; } 
SkeletalMesh *CMeshFactory::CreateSkeletalMeshInstance() { return new SkeletalMesh; } 

shared_ptr<BasicMesh> CMeshFactory::CreateBasicMesh() { shared_ptr<BasicMesh> pMesh( CreateBasicMeshInstance() ); return pMesh; }
shared_ptr<ProgressiveMesh> CMeshFactory::CreateProgressiveMesh() { shared_ptr<ProgressiveMesh> pMesh( CreateProgressiveMeshInstance() ); return pMesh; } 
shared_ptr<SkeletalMesh> CMeshFactory::CreateSkeletalMesh() { shared_ptr<SkeletalMesh> pMesh( CreateSkeletalMeshInstance() ); return pMesh; } 


BasicMesh* CMeshFactory::LoadMeshObjectFromFile( const std::string& filepath,
												  U32 load_option_flags,
												  CMeshType::Name mesh_type )
{
	BasicMesh* pMesh = CreateMeshInstance( mesh_type );
	if( !pMesh )
		return NULL;

	if( load_option_flags & MeshLoadOption::CUSTOM_MESH )
		SetCustomMesh( *pMesh );

	bool loaded = pMesh->LoadFromFile( filepath, load_option_flags );

	if( loaded )
		return pMesh;
	else
	{
		SafeDelete( pMesh );
		return NULL;
	}
}


BasicMesh* CMeshFactory::LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
																    const std::string& filepath,
																    U32 load_option_flags,
																	CMeshType::Name mesh_type )
{
	BasicMesh* pMesh = CreateMeshInstance( mesh_type );

	bool loaded = pMesh->LoadFromArchive( mesh_archive, filepath, load_option_flags );

	if( loaded )
		return pMesh;
	else
	{
		SafeDelete( pMesh );
		return NULL;
	}
}


} // namespace amorphous
