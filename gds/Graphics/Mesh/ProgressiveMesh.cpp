#include "ProgressiveMesh.hpp"
#include "MeshFactory.hpp" // ctor of CProgressiveMesh needs the mesh impl factory 


namespace amorphous
{


CProgressiveMesh::CProgressiveMesh()
{
//	m_pImpl = MeshFactory().CreateProgressiveMeshImpl();
	CMeshImpl *pImpl = MeshImplFactory()->CreateProgressiveMeshImpl();
	m_pImpl = boost::shared_ptr<CMeshImpl>( pImpl );
}


} // namespace amorphous
