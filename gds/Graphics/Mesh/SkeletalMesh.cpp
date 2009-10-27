#include "SkeletalMesh.hpp"


CSkeletalMesh::CSkeletalMesh()
{
//	m_pImpl = MeshFactory()->CreateSkeletalMeshImpl();
	CMeshImpl *pImpl = MeshImplFactory()->CreateSkeletalMeshImpl();
	m_pImpl = boost::shared_ptr<CMeshImpl>( pImpl );
}
