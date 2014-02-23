
#include "MapCompiler.h"

#include "Stage/BinaryNode.hpp"


namespace amorphous
{


void CPortalBuilder::ConstructFrom( CBSPTreeBuilder* pBSPTBuilder,
								    CCellBuilder* pCellBuilder)
{
	m_pBSPTree = pBSPTBuilder->GetBSPTree();
	m_paCell = pCellBuilder->GetCell();
	MakeBoundingPortals( pBSPTBuilder->GetAABB() );
	MakeNodePortals_r( 0 );		//establish link ( Portal--->Cell )

	WritePortals("before removal process.txt");
	
	RemoveInvalidPortals();
	pCellBuilder->SetPortalBuffer( &m_aPortal );
	AttachPortalToCell();		//establish link ( Cell--->Portal )

}

void CPortalBuilder::MakeNodePortals_r(short sNodeIndex)
{
	SNode& rThisNode = m_pBSPTree->at( sNodeIndex );

	SPlane& rThisPlane = rThisNode.GetPlane();

	//allocate portal
	CPortalFace newportal;
	newportal.SetPlaneFrom(rThisNode);
	newportal.m_asNode[PRT_FRONT] = rThisNode.child[CLD_FRONT];
	newportal.m_asNode[PRT_BACK] = rThisNode.child[CLD_BACK];
	newportal.m_asCell[PRT_FRONT] = rThisNode.sCellIndex;

	//①分断平面に沿う大きなポリゴンを作ります。
	newportal.MakeLargeFaceFromPlane();

	//②現在のノードに属するポータルで、大きなポリゴンをクリップします。
	int i, iNumPortals = m_aPortal.size();
	for(i=0; i<iNumPortals; i++)
	{
		if( m_aPortal[i].m_asNode[PRT_FRONT] != sNodeIndex
		&&	m_aPortal[i].m_asNode[PRT_BACK] != sNodeIndex ) continue; // not attached
		
		SPlane clipplane = m_aPortal[i].GetPlane();
		if( m_aPortal[i].m_asNode[PRT_FRONT] == sNodeIndex )
		{	
			//ポータルの表側ノードが、現在のノードということは、
			//現在のノードに向いている平面ということです。（表が生きる）
		}else{
			//そうでないなら、外向きの平面なので、裏返した平面で切ります（これで、表を残せば良い）
			clipplane.Flip();
		}
		newportal.ClipFaceWithPlane( clipplane );
	}

	//③最後に、現在の分断平面で、属するポータル達を分類し、
	//前後の子ノードに振り分けます。
	short frontchild = rThisNode.child[CLD_FRONT];
	short backchild = rThisNode.child[CLD_BACK];
	for(i=0; i<iNumPortals; i++)
	{
		CPortalFace& rPortal = m_aPortal[i];
		if( rPortal.m_asNode[PRT_FRONT] != sNodeIndex
		&&	rPortal.m_asNode[PRT_BACK] != sNodeIndex )
		continue; // not attached
		switch( ClassifyFace( rThisPlane, rPortal ) )
		{
		// to front child
		case FCE_ONPLANE:
		case FCE_FRONT:
			if( rPortal.m_asNode[PRT_FRONT] == sNodeIndex )
			{
				rPortal.m_asNode[PRT_FRONT] = frontchild;
				rPortal.m_asCell[PRT_FRONT] = rThisNode.sCellIndex;
			}
			else
			{
				rPortal.m_asNode[PRT_BACK] = frontchild;
				rPortal.m_asCell[PRT_BACK] = rThisNode.sCellIndex;
			}
			break;
		// to back child
		case FCE_BACK:
			if( rPortal.m_asNode[PRT_FRONT] == sNodeIndex )
				rPortal.m_asNode[PRT_FRONT] = backchild;
			else
				rPortal.m_asNode[PRT_BACK] = backchild;
			break;
		//need split
		case FCE_SPLIT:
			 CPortalFace frontportal = rPortal;
			 CPortalFace backportal = rPortal;
			rPortal.Split(frontportal, backportal, rThisPlane );		
			if( frontportal.m_asNode[PRT_FRONT] == sNodeIndex )
			{
				frontportal.m_asNode[PRT_FRONT] = frontchild;
				frontportal.m_asCell[PRT_FRONT] = rThisNode.sCellIndex;
			}
			else
			{
				frontportal.m_asNode[PRT_BACK] = frontchild;
				frontportal.m_asCell[PRT_BACK] = rThisNode.sCellIndex;
			}
			if( backportal.m_asNode[PRT_FRONT] == sNodeIndex )
				backportal.m_asNode[PRT_FRONT] = backchild;
			else
				backportal.m_asNode[PRT_BACK] = backchild;
			m_aPortal[i] = frontportal;
			m_aPortal.push_back( backportal );
		}
	}
	m_aPortal.push_back( newportal );

	//the end
	if( 0 < frontchild )
	{
		MakeNodePortals_r(frontchild);		
	}
	if( 0 < backchild )
	{
		MakeNodePortals_r(backchild);				
	}
}


void CPortalBuilder::MakeBoundingPortals( AABB3 aabb )
{
	//Get an Axis Aligned Bounding Box
	D3DXVECTOR3 vMin = aabb.vMin;
	D3DXVECTOR3 vMax = aabb.vMax;
	D3DXVECTOR3 vMergin( 1.0, 1.0, 1.0 );
	vMin -= vMergin;
	vMax += vMergin;

	D3DXVECTOR3 v000( vMin.x, vMin.y, vMin.z );
	D3DXVECTOR3 v100( vMax.x, vMin.y, vMin.z );
	D3DXVECTOR3 v001( vMin.x, vMin.y, vMax.z );
	D3DXVECTOR3 v101( vMax.x, vMin.y, vMax.z );
	D3DXVECTOR3 v010( vMin.x, vMax.y, vMin.z );
	D3DXVECTOR3 v110( vMax.x, vMax.y, vMin.z );
	D3DXVECTOR3 v011( vMin.x, vMax.y, vMax.z );
	D3DXVECTOR3 v111( vMax.x, vMax.y, vMax.z );

	CPortalFace portal;

	//Each of the 6 bounding portals has the root node of the BSP-Tree as its front node
	//	       					     some kind of 'pseudo node' as its back node
	portal.m_asNode[PRT_FRONT] = 0;
	portal.m_asNode[PRT_BACK] = OUTSIDE_NODE;

	//Be careful about the order of the vertices. the front side of a polygon is determined
	//by tracing the vertices in the clock-wise order.

	// bottom - v?0? ( y = vMin.y )
	portal.AddVertex( v000 );
	portal.AddVertex( v001 );
	portal.AddVertex( v101 );
	portal.AddVertex( v100 );
	this->m_aPortalPlane.AddPlaneFromFace(portal);
	m_aPortal.push_back( portal );
	portal.ClearVertices();

	// top - v?1? ( y = vMax.y )
	portal.AddVertex( v111 );
	portal.AddVertex( v011 );
	portal.AddVertex( v010 );
	portal.AddVertex( v110 );
	this->m_aPortalPlane.AddPlaneFromFace(portal);
	m_aPortal.push_back( portal );
	portal.ClearVertices();

	// left - v0?? ( x = vMin.x )
	portal.AddVertex( v010 );
	portal.AddVertex( v011 );
	portal.AddVertex( v001 );
	portal.AddVertex( v000 );
	this->m_aPortalPlane.AddPlaneFromFace(portal);
	m_aPortal.push_back( portal );
	portal.ClearVertices();

	// right - v1?? ( x = vMax.x )
	portal.AddVertex( v100 );
	portal.AddVertex( v101 );
	portal.AddVertex( v111 );
	portal.AddVertex( v110 );
	this->m_aPortalPlane.AddPlaneFromFace(portal);
	m_aPortal.push_back( portal );
	portal.ClearVertices();

	// far - v??1 ( z = vMax.z )
	portal.AddVertex( v111 );
	portal.AddVertex( v101 );
	portal.AddVertex( v001 );
	portal.AddVertex( v011 );
	this->m_aPortalPlane.AddPlaneFromFace(portal);
	m_aPortal.push_back( portal );
	portal.ClearVertices();

	// near - v??0 ( z = vMin.z )
	portal.AddVertex( v110 );
	portal.AddVertex( v010 );
	portal.AddVertex( v000 );
	portal.AddVertex( v100 );
	this->m_aPortalPlane.AddPlaneFromFace(portal);
	m_aPortal.push_back( portal );
	portal.ClearVertices();

}

void CPortalBuilder::RemoveInvalidPortals()
{
	vector<SNode> *pBSPTree = m_pBSPTree;
//	CPortalFace* pPortal = m_aPortal.begin();
	vector<CPortalFace>::iterator pPortal = m_aPortal.begin();
	while( pPortal != m_aPortal.end() )
	{
		//Only the portals that have valid contents on both sides survive
		if(pPortal->m_asNode[PRT_FRONT] == CONTENTS_SOLID
			|| pPortal->m_asNode[PRT_BACK] == CONTENTS_SOLID
			|| pPortal->m_asNode[PRT_FRONT] == OUTSIDE_NODE
			|| pPortal->m_asNode[PRT_BACK] == OUTSIDE_NODE )
			pPortal = m_aPortal.erase( pPortal );
		else
			pPortal++;
	}
}

void CPortalBuilder::AttachPortalToCell()		//establish link ( Cell--->Portal )
{
	int i, iNumPortals = m_aPortal.size();
	vector<CCell>* paCell = m_paCell;
	short sFrontCellIndex, sBackCellIndex;
	for(i=0; i<iNumPortals; i++)
	{
		sFrontCellIndex = m_aPortal[i].GetFrontCellIndex();
		CCell& rFrontCell = paCell->at( sFrontCellIndex );
		rFrontCell.m_asPortalIndex.push_back( i );

		sBackCellIndex = m_aPortal[i].GetBackCellIndex();
		CCell& rBackCell = paCell->at( sBackCellIndex );
		rBackCell.m_asPortalIndex.push_back( i );
	}
}

void CPortalBuilder::WritePortals( const string& filename )
{
	FILE* fp = fopen( filename.c_str(), "wt" );
	char acVec[32];
	int i, iNumPortals = m_aPortal.size();

	fprintf(fp, "----- Total Portals: %d -----\n", iNumPortals);

	fprintf(fp, "\n[portal index] node(front, back) / cell(front, back)\n");
	for(i=0; i<iNumPortals; i++)
	{
		CPortalFace& rPortal = m_aPortal[i];
		fprintf(fp,"[%03d] node( [%03d] ,[%03d] ) / cell( [%03d], [%03d] )  ",
			i, rPortal.m_asNode[PRT_FRONT], rPortal.m_asNode[PRT_BACK],
			rPortal.GetFrontCellIndex(), rPortal.GetBackCellIndex() );
		SPlane& rPlane = rPortal.GetPlane();
//		D3DXVec3toStr(acVec, rPlane.normal, 2);
//		fprintf(fp, "( n%s, d=%.2f )", acVec, rPlane.dist);
		fprintf(fp, "( n%s, d=%.2f )", to_string(rPlane.normal).c_str(), rPlane.dist);
		
		fprintf(fp,"\n");
	}
	fclose(fp);
}

} // amorphous
