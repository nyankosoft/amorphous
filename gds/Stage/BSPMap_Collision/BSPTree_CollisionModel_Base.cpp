
#include "BSPTree_CollisionModel.hpp"

#include "Support/memory_helpers.hpp"
#include "3DMath/ConvexPolygon.hpp"


namespace amorphous
{


CBSPTree_CollisionModel::CBSPTree_CollisionModel()
{
	m_iNumBSPNodes = 0;
	m_paBSPNode = NULL;
	m_iNumPolygons = 0;
	m_paPolygon = NULL;
	m_iNumCells = 0;
	m_paCell_CollisionModel = NULL;
}

CBSPTree_CollisionModel::~CBSPTree_CollisionModel()
{
	Release();
}

void CBSPTree_CollisionModel::Release()
{
	SafeDeleteArray( m_paBSPNode );
	m_iNumBSPNodes = 0;
	SafeDeleteArray( m_paPolygon );
	m_iNumPolygons = 0;
	SafeDeleteArray( m_paCell_CollisionModel );
	m_iNumCells = 0;
}

} // namespace amorphous
