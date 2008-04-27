#include "CellBuilder.h"
#include "MapCompiler.h"
#include <algorithm>

#include "Stage/BinaryNode.h"


CCellBuilder::CCellBuilder(CBSPTreeBuilder* pBSPTBuilder)
{
	m_pBSPTreeBuilder = pBSPTBuilder;

	pBSPTBuilder->SetCellBuilder( this );
	EnumerateCells_r( 0 );
}


void CCellBuilder::EnumerateCells_r(short sNodeIndex)
{
	SNode& rThisNode = m_pBSPTreeBuilder->GetBSPTree()->at( sNodeIndex );
	short sFrontNode = rThisNode.child[CLD_FRONT];
	short sBackNode = rThisNode.child[CLD_BACK];

	if( 0 <= sFrontNode )
		EnumerateCells_r( sFrontNode );
	else if(sFrontNode != CONTENTS_SOLID)
	{//Here is the cell node
		rThisNode.sCellIndex = m_aCell.size();

		CCell newcell;
		newcell.m_sNode = sNodeIndex;
		m_aCell.push_back( newcell );
	}

	if( 0 <= sBackNode )
		EnumerateCells_r( sBackNode );

}

void CCellBuilder::MakePVS()
{
	//Create 'visibility builder' objects
	CPortalVisibilityBuilder* pPositionVisBuilder
		= new CPositionalVisibilityBuilder(&m_aCell, m_paPortal);
	CPortalVisibilityBuilder* pTraceVisBuilder
		= new CTracingVisibilityBuilder(&m_aCell, m_paPortal, pPositionVisBuilder);
	CPortalVisibilityBuilder* pClippingVisBuilder
		= new CClippingVisibilityBuilder(&m_aCell, m_paPortal, pTraceVisBuilder);
	int i,j,k;
	int iNumPortals;  //How many portals in the current cell
	int iNumTotalPortals = m_paPortal->size();  // How many portals in the current world

	short* psPortalToPortalVis;
	short* psCellToPortalVis = new short [m_paPortal->size()];
	short sFrontCellIndex, sBackCellIndex;

	for(i=0; i<m_aCell.size(); i++)
	{
		memset( psCellToPortalVis, PVS_NOTVIS, sizeof(short) * m_paPortal->size() );
		CCell& rCell = m_aCell[i];
		iNumPortals = rCell.GetNumPortals();
		for(j=0; j<iNumPortals; j++)
		{
			CPortalFace& rPortal = m_paPortal->at( rCell.GetPortalIndex(j) );

			//Which portals are visible when we are in 'rCell' and see outside through 'rPortal'
			psPortalToPortalVis = pClippingVisBuilder->SetVisibility(i,j);

			for(k=0; k<iNumTotalPortals; k++)
				psCellToPortalVis[k] |= psPortalToPortalVis[k];
		}
		
		list<short>& sVCIndex = rCell.m_sVisCellIndex;
		sVCIndex.clear();
		for(j=0; j<iNumTotalPortals; j++)
		{
			if(psCellToPortalVis[j] == PVS_NOTVIS)
				continue;

			sFrontCellIndex = m_paPortal->at(j).GetFrontCellIndex();
			sBackCellIndex = m_paPortal->at(j).GetBackCellIndex();

			if( psCellToPortalVis[j] & PVS_FRONT //we can see the front side of this portal
				&& find(sVCIndex.begin(), sVCIndex.end(), sBackCellIndex) == sVCIndex.end() )  //and the cell index is not listed yet
				sVCIndex.push_back( sBackCellIndex ); //the cell behind this portal is visible and is put on the list

			if( psCellToPortalVis[j] & PVS_BACK	//we can see the back side of this portal
				&& find(sVCIndex.begin(), sVCIndex.end(), sFrontCellIndex) == sVCIndex.end() )  //and the cell index is not listed yet
				sVCIndex.push_back( sFrontCellIndex ); //the cell in front of this portal is visible and is put on the list
		}
		sVCIndex.sort();
	}

	delete [] psCellToPortalVis;
	delete pClippingVisBuilder;
	delete pTraceVisBuilder;
	delete pPositionVisBuilder;
}

void CCellBuilder::WriteCells( const string& filename )
{
	FILE* fp = fopen(filename.c_str(), "w");

	int i,j;
	int iNumCells = m_aCell.size();
	int iNumPortals;
	char acMax[32], acMin[32], acStr[256];
	vector<SNode>* pBSPTree;

	fprintf(fp,"\n Total Cells: %d\n", iNumCells);
	fprintf(fp, "\n[cellnum] nodenum  AABB(vMin, vMax)\n");
	fprintf(fp, "(portal counts) portal indices, ...\n");
	fprintf(fp, "(viscell counts) viscell indices, ...\n");

	string strAABB;
	for(i=0; i<iNumCells; i++)
	{
		fprintf(fp, "\n");

		//Write node index & AABB
		CCell& rCell = m_aCell[i];
		vector<SNode>* paNode = m_pBSPTreeBuilder->GetBSPTree();
		SNode rNode = paNode->at( rCell.m_sNode );
//		D3DXVec3toStr(acMin, rNode.aabb.vMin, 2);
//		D3DXVec3toStr(acMax, rNode.aabb.vMax, 2);
//		fprintf(fp, "[%d] node: %d  %s - %s\n", i, rCell.m_sNode, acMin, acMax);
		fprintf(fp, "[%d] node: %d  %s\n", i, rCell.m_sNode, to_string(rNode.aabb).c_str() );

		//Write the portal indices
		iNumPortals = rCell.GetNumPortals();
		if(!iNumPortals)
			continue;
		fprintf(fp, "ptl(%02d) ", iNumPortals);
		for(j=0; j<iNumPortals; j++)
		{
			fprintf(fp, "%d, ", rCell.GetPortalIndex(j));
		}
		fprintf(fp, "\n");

		//Write PVS cell indices
		int iNumVisCells = rCell.m_sVisCellIndex.size();
		fprintf(fp, "vcl(%02d) ", iNumVisCells);
		list<short>::iterator itrVisCellIndex = rCell.m_sVisCellIndex.begin();
		while( itrVisCellIndex != rCell.m_sVisCellIndex.end() )
		{
			fprintf(fp, "%d, ", *itrVisCellIndex);
			itrVisCellIndex++;
		}
		fprintf(fp, "\n");

		//Write the internal BSP-Tree
		if( rCell.m_pBSPTreeBuilder )
		{
			fprintf(fp, "bsptree:\n");
			pBSPTree = rCell.m_pBSPTreeBuilder->GetBSPTree();
			fputs("[nodenum] -  [CLD_FRONT] [CLD_BACK]  (plane)\n", fp);
			for(j=0; j<pBSPTree->size(); j++)
			{
				rCell.m_pBSPTreeBuilder->GetNodeString(acStr, j);
				fprintf(fp, "%s\n", acStr);
			}
			fprintf(fp, "\n");
		}

		//Write the number of internal faces
		fprintf(fp, "Interior faces: %d\n", rCell.m_aInteriorFace.size());

	}

	fclose(fp);
}

void CCellBuilder::AddFixedModel(vector<CMapFace>* paFace)
{
	m_pBSPTreeBuilder->SortFacesByNode(paFace);
	//  At every element of the 'paFace' , 'm_sNode' is filled with the node number 
	//of the corresponding leaf-node in the parent BSP-Tree 'm_pBSPTreeBuilder'.

	int i,j;
	for(i=0; i<m_aCell.size(); i++)
	{
		CCell& rCell = m_aCell[i];
		rCell.m_aInteriorFace.clear();
		for(j=0; j<paFace->size(); j++)
		{
			if( paFace->at(j).m_sNode == rCell.m_sNode )
				rCell.m_aInteriorFace.push_back( paFace->at(j) );
		}
		if(rCell.m_aInteriorFace.size() == 0)
			continue;  //No fixed models in this cell
		rCell.m_pBSPTreeBuilder = new CBSPTreeBuilder();
		rCell.m_pBSPTreeBuilder->ConstructFrom( &rCell.m_aInteriorFace, ONPLANE_GOTO_BACKCLD );
		//rCell.m_pBSPTreeBuilder->Reduce();
	}

}


void CCellBuilder::AddInteriorModels(vector<CMapFace>* paFace)
{
	m_pBSPTreeBuilder->SortFacesByNode(paFace);
	//  At every element of the 'paFace' , 'm_sNode' is filled with the node number 
	//of the corresponding leaf-node in the parent BSP-Tree 'm_pBSPTreeBuilder'.
	// When a face is crossing over multiple cells, it will be split and sent to each cell.

	vector<CMapFace> aTempFaces;	// temporarily store interior faces to build BSP-Tree

	vector<SNode>* paNode;
	int i,j;
	for(i=0; i<m_aCell.size(); i++)
	{
		CCell& rCell = m_aCell[i];
		rCell.m_aInteriorFace.clear();
		for(j=0; j<paFace->size(); j++)
		{
			if( paFace->at(j).m_sNode == rCell.m_sNode )
				rCell.m_aInteriorFace.push_back( paFace->at(j) );
		}

		if(rCell.m_aInteriorFace.size() == 0)
			continue;  //No fixed models in this cell

		// sort the faces for fewer texture switchings during rendering
		rCell.SortInteriorFacesByTextureID();

		// Copy interior faces to 'aTempFaces'
		aTempFaces.clear();
		for(j=0; j<rCell.m_aInteriorFace.size(); j++)
		{
			if( rCell.m_aInteriorFace[j].ReadTypeFlag(CMapFace::TYPE_NOCLIP) )
				continue;	//no-clip faces are not used in the construction of the interior BSP-Tree
			else
				aTempFaces.push_back( rCell.m_aInteriorFace[j] );
		}

		if( aTempFaces.size() == 0 )
			continue;	//This cell has only the no-clip faces
		
		// Build one BSP-Tree from interior faces
		rCell.m_pBSPTreeBuilder = new CBSPTreeBuilder();
		rCell.m_pBSPTreeBuilder->ConstructFrom( &aTempFaces, ONPLANE_GOTO_BACKCLD );
		paNode = rCell.m_pBSPTreeBuilder->GetBSPTree();
		for(j=0; j<paNode->size(); j++)	//all these nodes are contained in this cell 
			paNode->at(j).sCellIndex = i;	//so, mark them with the index of this cell
	}

}

CCell::CCell(const CCell& rCell)
{
	m_sNode = rCell.m_sNode;
	m_pBSPTreeBuilder = NULL;
	if( rCell.m_pBSPTreeBuilder )
	{
		m_pBSPTreeBuilder = new CBSPTreeBuilder;
		*m_pBSPTreeBuilder= *rCell.m_pBSPTreeBuilder;
	}
	m_aInteriorFace.assign( rCell.m_aInteriorFace.begin(), rCell.m_aInteriorFace.end() );
	m_asPortalIndex.assign( rCell.m_asPortalIndex.begin(), rCell.m_asPortalIndex.end() );
	m_sVisCellIndex.assign( rCell.m_sVisCellIndex.begin(), rCell.m_sVisCellIndex.end() );
}

CCell::~CCell()
{
	if (m_pBSPTreeBuilder)
		delete m_pBSPTreeBuilder;
	m_aInteriorFace.clear();
	m_asPortalIndex.clear();
	m_sVisCellIndex.clear();
}

int CompareTextureID( const void *pFace1, const void *pFace2 );

void CCell::SortInteriorFacesByTextureID()
{
//	qsort( m_aInteriorFace.begin(),	m_aInteriorFace.size(), sizeof(CMapFace),
	qsort( &m_aInteriorFace[0],	m_aInteriorFace.size(), sizeof(CMapFace),
		   CompareTextureID );
}

int CompareTextureID( const void *pFace1, const void *pFace2 )
{
	if( ((CMapFace *)pFace1)->m_sTextureID < ((CMapFace *)pFace2)->m_sTextureID )
		return -1;
	else if( ((CMapFace *)pFace1)->m_sTextureID == ((CMapFace *)pFace2)->m_sTextureID )
		return 0;
	else
		return 1;
}