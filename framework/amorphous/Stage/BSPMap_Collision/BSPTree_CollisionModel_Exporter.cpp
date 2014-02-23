
#include "BSPTree_CollisionModel_Exporter.hpp"

#include "../bspstructs.hpp"
#include "BSPMapCompiler/BSPMapCompiler.hpp"

#include "Support/memory_helpers.hpp"

#include "3DMath/ConvexPolygon.hpp"

#include <vector>


namespace amorphous
{
using namespace std;



CBSPTree_CollisionModel_Exporter::CBSPTree_CollisionModel_Exporter()
{
	m_paCell_CollisionModel_Export = NULL;
	m_pMapCompiler = NULL;
}


CBSPTree_CollisionModel_Exporter::~CBSPTree_CollisionModel_Exporter()
{
	SafeDeleteArray( m_paCell_CollisionModel_Export );
}


void CBSPTree_CollisionModel_Exporter::SetCollisionModel( CBSPMapCompiler *pMapCompiler )
{
	m_pMapCompiler = pMapCompiler;

	CPlaneBuffer *pPlaneBuffer = pMapCompiler->GetPlaneBuffer();
	vector<SNode> *pvecBSPTreeNode = pMapCompiler->GetBSPTree();
	vector<CMapFace> *pvecMapFace = pMapCompiler->GetFace(); // get polygons which compose this map

	// collision model for the map
	SetCollisionModel( pPlaneBuffer, pvecBSPTreeNode, pvecMapFace );

	// collision model for the cells in the map
	vector<CCell> *pvecCell = pMapCompiler->GetCell();
	m_iNumCells = pvecCell->size();

	if( 0 < m_iNumCells )
	{
		m_paCell_CollisionModel_Export = new CBSPTree_CollisionModel_Exporter [m_iNumCells];

		int iCellIndex;
		for( iCellIndex=0; iCellIndex<m_iNumCells; iCellIndex++ )
		{
			CCell& rCell = pvecCell->at(iCellIndex);

			if( !rCell.m_pBSPTreeBuilder )
				continue;	// 'rCell' is empty

			m_paCell_CollisionModel_Export[iCellIndex].SetCollisionModel( pPlaneBuffer,
				                                                          rCell.m_pBSPTreeBuilder->GetBSPTree(),
						                                                  &rCell.m_aInteriorFace );
		}
	}

}

void CBSPTree_CollisionModel_Exporter::SetCollisionModel( CPlaneBuffer *pPlaneBuffer,
														  vector<SNode> *pvecBSPTreeNode,
														  vector<CMapFace> *pvecMapFace )
{
	Release();

	m_iNumBSPNodes = pvecBSPTreeNode->size();

	if( m_iNumBSPNodes == 0 )
		return;

	m_paBSPNode = new SBSPNode [m_iNumBSPNodes];

	int i;
	for( i=0; i<m_iNumBSPNodes; i++ )
	{
		SNode& rSrcNode = pvecBSPTreeNode->at(i);

		m_paBSPNode[i].plane      = pPlaneBuffer->at( rSrcNode.sPlaneIndex );
		m_paBSPNode[i].aabb       = rSrcNode.aabb;
		m_paBSPNode[i].child[0]   = rSrcNode.child[0];
		m_paBSPNode[i].child[1]   = rSrcNode.child[1];
		m_paBSPNode[i].iCellIndex = rSrcNode.sCellIndex;
	}

	m_iNumPolygons = pvecMapFace->size();

	m_paPolygon = new ConvexPolygon [m_iNumPolygons];

	vector<int> *paveciOnPlaneFaceIndex    = new vector<int> [m_iNumBSPNodes]; // store indices to faces that are on partition planes
	vector<int> *paveciCrossPlaneFaceIndex = new vector<int> [m_iNumBSPNodes]; // store indices to faces that cross partition planes

	int iNodeIndex, iFaceIndex, type;
	for( iFaceIndex=0; iFaceIndex<m_iNumPolygons; iFaceIndex++ )
	{
		// set indices to faces for each node in the bsp-tree
		CMapFace& rFace = pvecMapFace->at(iFaceIndex);

		if( rFace.ReadTypeFlag(CMapFace::TYPE_NOCLIP) )
			continue;	// non-collidable face

		GetNodeIndex_r( iNodeIndex, type, rFace );

		if( type == FCE_ONPLANE )
			paveciOnPlaneFaceIndex[iNodeIndex].push_back( iFaceIndex );
		else	// type == FCE_SPLIT
			paveciCrossPlaneFaceIndex[iNodeIndex].push_back( iFaceIndex );
	}


	int k=0;
	for( iNodeIndex=0; iNodeIndex<m_iNumBSPNodes; iNodeIndex++ )
	{	// get the array of indices to faces belonging to the current node (plane)
		vector<int>& rveciFaceIndex = paveciOnPlaneFaceIndex[iNodeIndex];

		m_paBSPNode[iNodeIndex].iPlanePolygonIndex = k;
		m_paBSPNode[iNodeIndex].iNumPlanePolygons = rveciFaceIndex.size();

		// convert 'CMapFace' into 'ConvexPolygon'
		for( i=0; i<rveciFaceIndex.size(); i++ )
		{
			CMapFace& rFace = pvecMapFace->at( rveciFaceIndex[i] );
			ConvexPolygon& rPolygon = m_paPolygon[k++];
			MakePolygonFromFace( rPolygon, rFace );
		}

		// store polygons which are crossing partition planes
		vector<int>& rveciCrossFaceIndex = paveciCrossPlaneFaceIndex[iNodeIndex];

		m_paBSPNode[iNodeIndex].iCrossPolygonIndex = k;
		m_paBSPNode[iNodeIndex].iNumCrossPolygons = rveciCrossFaceIndex.size();

		// convert 'CMapFace' into 'ConvexPolygon'
		for( i=0; i<rveciCrossFaceIndex.size(); i++ )
		{
			CMapFace& rFace = pvecMapFace->at( rveciCrossFaceIndex[i] );
			ConvexPolygon& rPolygon = m_paPolygon[k++];
			MakePolygonFromFace( rPolygon, rFace );
		}
	}

	delete [] paveciOnPlaneFaceIndex;
	delete [] paveciCrossPlaneFaceIndex;


	// set the collision information for vertices and edges
	SetPolygonCollisionInfo();
}


void CBSPTree_CollisionModel_Exporter::MakePolygonFromFace(ConvexPolygon& rPolygon, // [out]
														   CMapFace& rFace)			 // [in]
{
	Vector3 avVertex[128];
	int i;
	for( i=0; i<rFace.GetNumVertices(); i++ )
		avVertex[i] = rFace.GetVertex(i);

	rPolygon.SetVertices( avVertex, rFace.GetNumVertices() );
	rPolygon.SetNormal( rFace.GetPlane().normal );
	rPolygon.SetDistance( rFace.GetPlane().dist );

//	if( rFace.m_iPolygonIndex < 0 )
//		MessageBox( NULL, "invalid polygon index", "error", MB_OK|MB_ICONWARNING );

	// set the index to the corresponding 'SPolygon'
	rPolygon.m_iPolygonIndex = rFace.m_iPolygonIndex;
}


void CBSPTree_CollisionModel_Exporter::GetNodeIndex_r( int& riDestNodeIndex, // [out] node which 'rFace' belongs to
		                                               int& rType,			 // [out] indicates whether 'rFace' is on or crossing the plane
						                               CMapFace& rFace,  // [in]
						                               int iNodeIndex )	 	 // [in]

{
	int c;
	SPlane& rPlane = m_paBSPNode[iNodeIndex].plane;
	c = ClassifyFace( rPlane, rFace, 0 );

	switch(c)
	{
	case FCE_ONPLANE:
		riDestNodeIndex = iNodeIndex;
		rType = FCE_ONPLANE;
		return;	// 'rFace' is on 'rPlane' and it should be linked to this node

	case FCE_SPLIT:
		riDestNodeIndex = iNodeIndex;
		rType = FCE_SPLIT;
		return;	// unexpected case

	case FCE_FRONT:
		GetNodeIndex_r( riDestNodeIndex, rType, rFace, m_paBSPNode[iNodeIndex].child[0] );
		return;

	case FCE_BACK:
		GetNodeIndex_r( riDestNodeIndex, rType, rFace, m_paBSPNode[iNodeIndex].child[1] );
		return;
	}

}


void CBSPTree_CollisionModel_Exporter::SaveToFile(FILE *fp)
{
	fwrite( &m_iNumBSPNodes, sizeof(int), 1, fp );

	fwrite( m_paBSPNode, sizeof(SBSPNode), m_iNumBSPNodes, fp );

	fwrite( &m_iNumPolygons, sizeof(int), 1, fp );

	int i;
	for( i=0; i<m_iNumPolygons; i++ )
	{
		m_paPolygon[i].WriteToFile(fp);
	}

	// write cell to file
	fwrite( &m_iNumCells, sizeof(int), 1, fp );

	for( i=0; i<m_iNumCells; i++ )
	{
		m_paCell_CollisionModel_Export[i].SaveToFile(fp);
	}

}


#define POLYGONANGLE_INVALID	0
#define POLYGONANGLE_COPLANER	1
#define POLYGONANGLE_CONVEX		2
#define POLYGONANGLE_CONCAVE	3

void CBSPTree_CollisionModel_Exporter::SetPolygonCollisionInfo()
{
	// find concave edges and interior edges
	int i, j, m, n;
	Vector3 vDist0, vDist1, vEdge0, vEdge1, v;
	float d;
	Vector3 vNormal0;	// plane normal of polygon 0
	float fDist0;	// plane distance of polygon 0
	char cPolygonAngle = POLYGONANGLE_INVALID;
	AABB3 aabb0;

	SConcaveEdgeTempFlag *concave_edge_info = new SConcaveEdgeTempFlag [m_iNumPolygons];
	memset( concave_edge_info, 0, sizeof(SConcaveEdgeTempFlag) * m_iNumPolygons );

	for( i=0; i<m_iNumPolygons; i++ )
	{
		ConvexPolygon& rPolygon0 = m_paPolygon[i];
		aabb0 = rPolygon0.GetAABB();
		vNormal0 = rPolygon0.GetNormal();
		fDist0 =   rPolygon0.GetDistance();

		// grow aabb slightly
		aabb0.vMin -= Vector3(0.1f, 0.1f, 0.1f);
		aabb0.vMax += Vector3(0.1f, 0.1f, 0.1f);

		for( j=0; j<m_iNumPolygons; j++ )
		{
			ConvexPolygon& rPolygon1 = m_paPolygon[j];

			if( !aabb0.IsIntersectingWith(rPolygon1.GetAABB()) )
				continue;

			// need to check edges and vertices

			// check whether the two polygons make convex or concave angle
			cPolygonAngle = POLYGONANGLE_INVALID;
			for( n=0; n< rPolygon1.GetNumVertices()-1; n++ )
			{
				d = Vec3Dot( rPolygon1.GetVertex(n), vNormal0 ) - fDist0;
				if( d < -0.0001f )
				{	// convex angle (polygons are facing away each other)
					cPolygonAngle = POLYGONANGLE_CONVEX;
					break;
				}
				else if( 0.0001f < d )
				{	// concave angle (polygons are facing toward each other)
					cPolygonAngle = POLYGONANGLE_CONCAVE;
					break;
				}
			}
			if( cPolygonAngle == POLYGONANGLE_INVALID )
			{	// coplaner (polygons are on the same plane)
				cPolygonAngle = POLYGONANGLE_COPLANER;
			}

			// check if there are two edges which share the same two vertices
			for( m=0; m<rPolygon0.GetNumVertices()-1; m++ )
			{
				vEdge0 = rPolygon0.GetVertex(m) - rPolygon0.GetVertex(m+1);

				for( n=0; n < rPolygon1.GetNumVertices()-1; n++ )
				{
					vEdge1 = rPolygon0.GetVertex(n) - rPolygon0.GetVertex(n+1);
					vDist0 = vEdge1 - vEdge0;
					vDist1 = vEdge0 - vEdge1;
					if( Vec3LengthSq( vDist0 ) < 0.00001f || Vec3LengthSq( vDist1 ) < 0.00001f )
					{
						// the two edges are sharing the same two vertices
						switch( cPolygonAngle )
						{
						case POLYGONANGLE_COPLANER:
							// interior edges
							rPolygon0.SetEdgeCollisionFlag( m, false );
							rPolygon1.SetEdgeCollisionFlag( n, false );
							break;
						case POLYGONANGLE_CONVEX:
							// convex edges - mark only one of the two edges
							rPolygon0.SetEdgeCollisionFlag( m, true  );
							rPolygon0.SetVertexCollisionFlag( m,   true  );
							rPolygon0.SetVertexCollisionFlag( m+1, true  );
//							rPolygon1.SetEdgeCollisionFlag( n + EDGE_FLAG_OFFSET, false );
							break;
						case POLYGONANGLE_CONCAVE:
							// concave edges - mark as concave and skip
							concave_edge_info[i].bIsConcaveEdge[m] = true;
							concave_edge_info[j].bIsConcaveEdge[n] = true;
							break;
						}
					}
				}
			}
		}
	}

	int iNumVertices;
	for( i=0; i<m_iNumPolygons; i++ )
	{
		ConvexPolygon& rPolygon = m_paPolygon[i];
		iNumVertices = rPolygon.GetNumVertices();

		for( m=0; m<iNumVertices; m++ )
		{
			if( concave_edge_info[i].bIsConcaveEdge[m] )
			{
				rPolygon.SetVertexCollisionFlag( m, false );
				rPolygon.SetVertexCollisionFlag( (m+1) % iNumVertices, false );
			}
		}
	}
}


} // namespace amorphous
