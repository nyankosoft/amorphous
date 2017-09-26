#include "MeshFactory.hpp"
#include "ProgressiveMesh.hpp"
#include "SkeletalMesh.hpp"
#include "CustomMesh.hpp"


namespace amorphous
{

using namespace std;


void SetCustomMesh( BasicMesh& src_mesh )
{
	src_mesh.m_pImpl.reset( new CustomMesh );
}


//=============================================================================
// MeshFactory
//=============================================================================

BasicMesh *MeshFactory::InitMeshInstance( MeshType::Name mesh_type, U32 load_option_flags )
{
	BasicMesh* pMesh = CreateMeshInstance( mesh_type );
	if( !pMesh )
		return nullptr;

	if( load_option_flags & MeshLoadOption::CUSTOM_MESH )
		SetCustomMesh( *pMesh );

	return pMesh;
}


BasicMesh *MeshFactory::CreateMeshInstance( MeshType::Name mesh_type )
{
	switch( mesh_type )
	{
	case MeshType::BASIC:
		return CreateBasicMeshInstance();
	case MeshType::PROGRESSIVE:
		return CreateProgressiveMeshInstance();
	case MeshType::SKELETAL:
		return CreateSkeletalMeshInstance();
	default:
		return nullptr;
	}

	return nullptr;
}


shared_ptr<BasicMesh> MeshFactory::CreateMesh( MeshType::Name mesh_type )
{
	return shared_ptr<BasicMesh>( CreateMeshInstance( mesh_type ) );
}


BasicMesh *MeshFactory::CreateBasicMeshInstance() { return new BasicMesh; }
ProgressiveMesh *MeshFactory::CreateProgressiveMeshInstance() { return new ProgressiveMesh; } 
SkeletalMesh *MeshFactory::CreateSkeletalMeshInstance() { return new SkeletalMesh; } 

shared_ptr<BasicMesh> MeshFactory::CreateBasicMesh() { shared_ptr<BasicMesh> pMesh( CreateBasicMeshInstance() ); return pMesh; }
shared_ptr<ProgressiveMesh> MeshFactory::CreateProgressiveMesh() { shared_ptr<ProgressiveMesh> pMesh( CreateProgressiveMeshInstance() ); return pMesh; } 
shared_ptr<SkeletalMesh> MeshFactory::CreateSkeletalMesh() { shared_ptr<SkeletalMesh> pMesh( CreateSkeletalMeshInstance() ); return pMesh; } 


BasicMesh* MeshFactory::LoadMeshObjectFromFile( const std::string& filepath,
												  U32 load_option_flags,
												  MeshType::Name mesh_type )
{
	BasicMesh* pMesh = InitMeshInstance( mesh_type, load_option_flags );

	bool loaded = pMesh->LoadFromFile( filepath, load_option_flags );

	if( loaded )
		return pMesh;
	else
	{
		SafeDelete( pMesh );
		return nullptr;
	}
}


BasicMesh* MeshFactory::LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
																    const std::string& filepath,
																    U32 load_option_flags,
																	MeshType::Name mesh_type )
{
	BasicMesh* pMesh = InitMeshInstance( mesh_type, load_option_flags );

	bool loaded = pMesh->LoadFromArchive( mesh_archive, filepath, load_option_flags );

	if( loaded )
		return pMesh;
	else
	{
		SafeDelete( pMesh );
		return nullptr;
	}
}


} // namespace amorphous
