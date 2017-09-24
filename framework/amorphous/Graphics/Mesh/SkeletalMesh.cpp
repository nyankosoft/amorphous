#include "SkeletalMesh.hpp"
#include "MeshFactory.hpp" // ctor of SkeletalMesh needs the mesh impl factory 


namespace amorphous
{


SkeletalMesh::SkeletalMesh()
{
//	m_pImpl = MeshFactory()->CreateSkeletalMeshImpl();
	MeshImpl *pImpl = GetMeshImplFactory()->CreateSkeletalMeshImpl();
	m_pImpl = std::shared_ptr<MeshImpl>( pImpl );
}


} // namespace amorphous
