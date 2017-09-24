#include "ProgressiveMesh.hpp"
#include "MeshFactory.hpp" // ctor of ProgressiveMesh needs the mesh impl factory 


namespace amorphous
{


ProgressiveMesh::ProgressiveMesh()
{
//	m_pImpl = MeshFactory().CreateProgressiveMeshImpl();
	MeshImpl *pImpl = GetMeshImplFactory()->CreateProgressiveMeshImpl();
	m_pImpl = std::shared_ptr<MeshImpl>( pImpl );
}


} // namespace amorphous
