#include "MeshFactory.hpp"
#include "ProgressiveMesh.hpp"
#include "SkeletalMesh.hpp"
#include "CustomMesh.hpp"

using namespace std;
using namespace boost;


void SetCustomMesh( CBasicMesh& src_mesh )
{
	src_mesh.m_pImpl.reset( new CCustomMesh );
}


//=============================================================================
// CMeshFactory
//=============================================================================

CBasicMesh *CMeshFactory::CreateMeshInstance( CMeshType::Name mesh_type )
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


shared_ptr<CBasicMesh> CMeshFactory::CreateMesh( CMeshType::Name mesh_type )
{
	return shared_ptr<CBasicMesh>( CreateMeshInstance( mesh_type ) );
}


CBasicMesh *CMeshFactory::CreateBasicMeshInstance() { return new CBasicMesh; }
CProgressiveMesh *CMeshFactory::CreateProgressiveMeshInstance() { return new CProgressiveMesh; } 
CSkeletalMesh *CMeshFactory::CreateSkeletalMeshInstance() { return new CSkeletalMesh; } 

shared_ptr<CBasicMesh> CMeshFactory::CreateBasicMesh() { shared_ptr<CBasicMesh> pMesh( CreateBasicMeshInstance() ); return pMesh; }
shared_ptr<CProgressiveMesh> CMeshFactory::CreateProgressiveMesh() { shared_ptr<CProgressiveMesh> pMesh( CreateProgressiveMeshInstance() ); return pMesh; } 
shared_ptr<CSkeletalMesh> CMeshFactory::CreateSkeletalMesh() { shared_ptr<CSkeletalMesh> pMesh( CreateSkeletalMeshInstance() ); return pMesh; } 


CBasicMesh* CMeshFactory::LoadMeshObjectFromFile( const std::string& filepath,
												  U32 load_option_flags,
												  CMeshType::Name mesh_type )
{
	CBasicMesh* pMesh = CreateMeshInstance( mesh_type );
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


CBasicMesh* CMeshFactory::LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
																    const std::string& filepath,
																    U32 load_option_flags,
																	CMeshType::Name mesh_type )
{
	CBasicMesh* pMesh = CreateMeshInstance( mesh_type );

	bool loaded = pMesh->LoadFromArchive( mesh_archive, filepath, load_option_flags );

	if( loaded )
		return pMesh;
	else
	{
		SafeDelete( pMesh );
		return NULL;
	}
}



#include "MeshFactory.hpp"
#include "Graphics/OpenGL/Mesh/GLBasicMeshImpl.hpp"
#include "Graphics/OpenGL/Mesh/GLProgressiveMeshImpl.hpp"
#include "Graphics/OpenGL/Mesh/GLSkeletalMeshImpl.hpp"


//class CMeshFactoryImpl_GL : public CMeshFactoryImpl
class CGLMeshFactory : public CMeshFactory
{
public:

//	boost::shared_ptr<CBasicMesh> CreateBasicMeshImpl()
	boost::shared_ptr<CMeshImpl> CreateBasicMeshImpl()
	{
		shared_ptr<CGLBasicMeshImpl> pImpl( new CGLBasicMeshImpl );
		return pImpl;
//		shared_ptr<CBasicMesh> p( new CBasicMesh(pImpl) );
//		return p;
	}

//	boost::shared_ptr<CProgressiveMesh> CreateProgressiveMeshImpl()
	boost::shared_ptr<CMeshImpl> CreateProgressiveMeshImpl()
	{
		shared_ptr<CGLProgressiveMeshImpl> pImpl( new CGLProgressiveMeshImpl );
		return pImpl;
//		shared_ptr<CProgressiveMesh> p( new CProgressiveMesh(pImpl) );
//		return p;
	}

//	boost::shared_ptr<CSkeletalMesh> CreateSkeletalMeshImpl()
	boost::shared_ptr<CMeshImpl> CreateSkeletalMeshImpl()
	{
		shared_ptr<CGLSkeletalMeshImpl> pImpl( new CGLSkeletalMeshImpl );
		return pImpl;
//		shared_ptr<CSkeletalMesh> p( new CSkeletalMesh(pImpl) );
//		return p;
	}
};

/*
extern void InitMeshFactory( boost::shared_ptr<CMeshFactory>& pMeshFactory )
{
	if( 
}
*/

