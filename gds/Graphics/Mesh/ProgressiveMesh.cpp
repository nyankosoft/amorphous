#include "ProgressiveMesh.hpp"


CProgressiveMesh::CProgressiveMesh()
{
//	m_pImpl = MeshFactory().CreateProgressiveMeshImpl();
	CMeshImpl *pImpl = MeshImplFactory()->CreateProgressiveMeshImpl();
	m_pImpl = boost::shared_ptr<CMeshImpl>( pImpl );
}
