#include "BSPTree_CollisionModel.hpp"
#include "3DMath/ConvexPolygon.hpp"

#include "Stage/BinaryNode.hpp"
#include "Stage/trace.hpp"

#include "Support/memory_helpers.hpp"

#include <d3d9.h>
#include <d3dx9.h>


// used to collect polygons which overlap with the volume of a point light
void CBSPTree_CollisionModel::CheckCollision( D3DXVECTOR3& vCenter,
											  float fRadius,
											  AABB3& aabb,
											  vector<int>& veciPolygonIndex )
{
	if( m_iNumBSPNodes == 0 )
		return;

	// start collision test from the root node
	CheckCollision_r( 0, vCenter, fRadius, aabb, veciPolygonIndex );
}


void CBSPTree_CollisionModel::CheckCollision_r( int iNodeIndex,
											    D3DXVECTOR3& vCenter,
												float fRadius,
												AABB3& aabb,
												vector<int>& veciPolygonIndex )
{
	int i;
	float d;
//	D3DXVECTOR3 vCenter = tr.GetSweptAABB().GetCenterPosition();
	int iPrevNodeIndex = 0;

	while(1)
	{
		if( iNodeIndex == CONTENTS_EMPTY )
		{	// reached cell
//			m_paCell[].CheckCollision(tr);
			return;
		}
		else if( iNodeIndex == CONTENTS_SOLID )
			return;

		SBSPNode& rCurrentNode = m_paBSPNode[ iNodeIndex ];

		// check the ploygons of this node
		for( i=0; i<rCurrentNode.iNumCrossPolygons; i++ )
		{
			CConvexPolygon& rPolygon = m_paPolygon[rCurrentNode.iCrossPolygonIndex + i];

			if( !aabb.IsIntersectingWith(rPolygon.GetAABB()) )
				continue;

			veciPolygonIndex.push_back( rPolygon.m_iPolygonIndex );

//			CheckCollisionToWorldConvexPolygon_X( tr, rPolygon );
		}

		// check the front child. if this node doesn't have a front child, we need check its cell
		if( rCurrentNode.child[0] == CONTENTS_EMPTY )
		{
			if( 0 < m_iNumCells )
				m_paCell_CollisionModel[rCurrentNode.iCellIndex].CheckCollision( vCenter, fRadius, aabb, veciPolygonIndex );
		}

		d = D3DXVec3Dot( &rCurrentNode.plane.normal, &vCenter ) - rCurrentNode.plane.dist;

//		fRadius = tr.GetSweptAABB().GetRadiusForPlane( rCurrentNode.plane );

		if( fRadius < d )
		{
			iNodeIndex = rCurrentNode.child[0];
		}
		else if( d < -fRadius )
		{
			iNodeIndex = rCurrentNode.child[1];
		}
		else // i.e.( -fRadius <= d && d <= fRadius )
		{
			// check the ploygons on this plane
			for( i=0; i<rCurrentNode.iNumPlanePolygons; i++ )
			{
				CConvexPolygon& rPolygon = m_paPolygon[rCurrentNode.iPlanePolygonIndex + i];

				if( !aabb.IsIntersectingWith(rPolygon.GetAABB()) )
					continue;

				veciPolygonIndex.push_back( rPolygon.m_iPolygonIndex );

//				CheckCollisionToWorldConvexPolygon_X( tr, rPolygon );
			}

			// need to recurse down both of the child nodes

			if( rCurrentNode.child[0] == CONTENTS_EMPTY )
			{	// reached a cell
				int temp=0;//	m_paCell[].CheckCollision(tr);
			}
			else
				CheckCollision_r( rCurrentNode.child[0], vCenter, fRadius, aabb, veciPolygonIndex );

			if( rCurrentNode.child[1] != CONTENTS_SOLID )
				CheckCollision_r( rCurrentNode.child[1], vCenter, fRadius, aabb, veciPolygonIndex );

			return;
		}
	}


}


void CBSPTree_CollisionModel::LoadFromFile(FILE *fp)
{
	Release();

	fread( &m_iNumBSPNodes, sizeof(int), 1, fp );
	if( 0 < m_iNumBSPNodes )
	{
		m_paBSPNode = new SBSPNode [m_iNumBSPNodes];
		fread( m_paBSPNode, sizeof(SBSPNode), m_iNumBSPNodes, fp );
	}

	fread( &m_iNumPolygons, sizeof(int), 1, fp );
	if( 0 < m_iNumPolygons )
	{
		m_paPolygon = new CConvexPolygon [m_iNumPolygons];

		int i;
		for( i=0; i<m_iNumPolygons; i++ )
		{
			m_paPolygon[i].LoadFromFile(fp);
		}
	}

	fread( &m_iNumCells, sizeof(int), 1, fp );

	if( 0 < m_iNumCells )
	{
		m_paCell_CollisionModel = new CBSPTree_CollisionModel [m_iNumCells];

		int iCellIndex;
		for( iCellIndex=0; iCellIndex<m_iNumCells; iCellIndex++ )
		{
			m_paCell_CollisionModel[iCellIndex].LoadFromFile(fp);
		}
	}
}