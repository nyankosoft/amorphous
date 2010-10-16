#include "SkeletalMesh.hpp"
#include "MeshFactory.hpp" // ctor of CSkeletalMesh needs the mesh impl factory 


CSkeletalMesh::CSkeletalMesh()
{
//	m_pImpl = MeshFactory()->CreateSkeletalMeshImpl();
	CMeshImpl *pImpl = MeshImplFactory()->CreateSkeletalMeshImpl();
	m_pImpl = boost::shared_ptr<CMeshImpl>( pImpl );
}
