#include "BSPTreeBuilder.h"
#include "MapCompiler.h"

#include "Stage/BinaryNode.h"
#include "Support/Log/DefaultLog.h"

#include <string>
#include <algorithm>
using namespace std;


CBSPTreeBuilder::CBSPTreeBuilder()
{
	memset(this, 0, sizeof(CBSPTreeBuilder));
	m_pFace = NULL;
	m_pCellBuilder = NULL;
	m_fOnplaneWeight = DEFAULT_ONPLANE_WEIGHT;
	m_fSplitWeight = DEFAULT_SPLIT_WEIGHT;
	m_fBalanceWeight = DEFAULT_BALANCE_WEIGHT;
	m_iTreeStyle = ONPLANE_GOTO_FRONTCLD;
}

CBSPTreeBuilder CBSPTreeBuilder::operator=(CBSPTreeBuilder _BSPTBuilder)
{
	m_pCellBuilder = _BSPTBuilder.m_pCellBuilder;
	m_fOnplaneWeight = _BSPTBuilder.m_fOnplaneWeight;
	m_fSplitWeight = _BSPTBuilder.m_fSplitWeight;
	m_fBalanceWeight = _BSPTBuilder.m_fBalanceWeight;
	m_iTreeStyle = _BSPTBuilder.m_iTreeStyle;
	m_pFace = _BSPTBuilder.m_pFace;
	m_aabb = _BSPTBuilder.m_aabb;
	m_iNumInitialFaces = _BSPTBuilder.m_iNumInitialFaces;

	m_aNode.clear();
	m_aNode.assign( _BSPTBuilder.m_aNode.begin(), _BSPTBuilder.m_aNode.end() );

	return *this;
}


bool CBSPTreeBuilder::ConstructFrom(vector<CMapFace>* _pFace, int iTreeStyle)
{
	if( _pFace->size() == 0 )
	{
		LOG_PRINT_ERROR( "Cannot build a BSP-Tree from nothing. We need some polygons." );
		return false;
	}

	m_pFace = _pFace;
	m_iTreeStyle = iTreeStyle;
	m_iNumInitialFaces = m_pFace->size();

	//Iniitalization
	int i;
	for(i=0; i<m_iNumInitialFaces; i++)
	{
		_pFace->at(i).m_sNode = 0;
		_pFace->at(i).m_bFlag = false;
	}

	m_aNode.reserve( DEFAULT_NUM_NODES );

	SNode headnode;
	headnode.child[CLD_FRONT] = CONTENTS_EMPTY;
	headnode.child[CLD_BACK] = CONTENTS_SOLID;
	m_aNode.push_back( headnode );
	Build_r(0);

	MakeAABB_r(0);

	m_aabb = MakeAABB();

	// MakeAABB_r() sometimes fails to construct proper AABBs
	// The following line is added to make sure that at least the root node of the bsp-tree has a proper aabb.
	m_aNode[0].aabb = m_aabb;

	return true;
}


static AABB3 s_NodeArea;


//currently not capable of handling backtrack
void CBSPTreeBuilder::Build_r(short sNode)
{
	int i;
	int front, back, split, onplane;
	int c;
	CMapFace face, frontface, backface;
	static vector<int> onpln_faces;
	static vector<int> front_faces;
	static vector<int> back_faces;
	bool bNoMoreFronts, bNoMoreBacks;
	short frontnode, backnode;

	// Get the pointer to the face whose plane is appropriate as the splitter
	// in the cerrent sub-space
	s_NodeArea.Nullify();
	CMapFace* pFace = SelectPartitionPlane(sNode);

	if(!pFace)
	{
		char acStr[256];
		const AABB3& area = s_NodeArea;
		sprintf( acStr, "Partition Plane selection failed. check the following area:\nmin(%.2f,%.2f,%.2f)\nmax(%.2f,%.2f,%.2f)",
			area.vMin.x, area.vMin.y, area.vMin.z, area.vMax.x, area.vMax.y, area.vMax.z );
		LOG_PRINT_ERROR( string(acStr) );
		return;
	}

	CMapFace& rPartFace = *pFace;

	rPartFace.m_bFlag = true;

	SPlane& rPartPlane = rPartFace.GetPlane();

	//If a face is belonging to the same sub-space (node) and is on the same plane
	// with the rPartFace, turn the flag to true;
	for(i=0; i<m_pFace->size(); i++)
	{
		CMapFace& rFace = m_pFace->at(i);
		if(rFace.m_sNode != sNode) continue;
		SPlane& rPlane = rFace.GetPlane();
		if( AlmostSamePlanes(rPartPlane,rPlane) )
			rFace.m_bFlag = true;
	}
	
	rPartFace.SetPlaneTo( m_aNode[sNode] ); //Set the plane to the current node

	front=0; back=0; split=0; onplane=0;
	bNoMoreFronts = true; bNoMoreBacks = true;
	for(i=0; i<m_pFace->size(); i++)
	{
		CMapFace& rFace = m_pFace->at(i);

		if(rFace.m_sNode != sNode)
			continue; //If the rFace is in a different sub-space, skip it

		c = ClassifyFace(rPartPlane, rFace);
		switch(c)
		{
		case FCE_ONPLANE:
			onpln_faces.push_back( i );  //store the index of rFace
			break;
		case FCE_FRONT:
			if(!rFace.m_bFlag)  //If the rFace has not been selected yet,
				bNoMoreFronts = false;
			front++;
			front_faces.push_back( i );
			break;
		case FCE_BACK:
			if(!rFace.m_bFlag)
				bNoMoreBacks = false;
			back++;
			back_faces.push_back( i );
			break;
		case FCE_SPLIT:
			//First, check if the further recursion is necessary
			if(!rFace.m_bFlag)
			{ bNoMoreFronts = false; bNoMoreBacks = false; }
			//Next, split the face and push the resulting 2 faces to the face buffer 'm_pFace'
			split++;
			//frontface = backface = rFace;
			rFace.Split(frontface, backface, rPartPlane);
			front_faces.push_back( i );
			m_pFace->at(i) = frontface;
			back_faces.push_back( m_pFace->size() );
			m_pFace->push_back(backface);
			break;
		}
	}

	SNode newnode;
	newnode.child[CLD_FRONT] = CONTENTS_EMPTY;
	newnode.child[CLD_BACK] = CONTENTS_SOLID;
	vector<int>::iterator itr;
	if(!bNoMoreFronts) //there are some unused faces in front of 'rPartPlane'.
	{
		frontnode = m_aNode.size();
		m_aNode[sNode].child[CLD_FRONT] = frontnode;
		m_aNode.push_back(newnode);
		for(itr = front_faces.begin(); itr != front_faces.end(); itr++)
			m_pFace->at( *itr ).m_sNode = frontnode;
		if(m_iTreeStyle == ONPLANE_GOTO_FRONTCLD)
		{
			for(itr = onpln_faces.begin(); itr != onpln_faces.end(); itr++)
				m_pFace->at( *itr ).m_sNode = frontnode;
		}
	}
	if(!bNoMoreBacks)  //there are still some unused faces behind 'rPartPlane'.
	{
		backnode = m_aNode.size();
		m_aNode[sNode].child[CLD_BACK] = backnode;
		m_aNode.push_back(newnode);
		for(itr = back_faces.begin(); itr != back_faces.end(); itr++)
			m_pFace->at( *itr ).m_sNode = backnode;
		if(m_iTreeStyle == ONPLANE_GOTO_BACKCLD)
		{
			for(itr = onpln_faces.begin(); itr != onpln_faces.end(); itr++)
				m_pFace->at( *itr ).m_sNode = backnode;
		}
	}

	front_faces.resize(0);
	back_faces.resize(0);
	onpln_faces.resize(0);

	//Recursive Call
	if(!bNoMoreFronts)
		Build_r(frontnode);
	if(!bNoMoreBacks)
		Build_r(backnode);

}

CMapFace* CBSPTreeBuilder::SelectPartitionPlane(short sNode)
{
	int maxweight = -99999999;
	int i,j,k, stop, iBestFaceNum = -1;
	for(i=0; i<m_pFace->size(); i++)
	{
		CMapFace& rFace = m_pFace->at(i);

		if(rFace.m_bFlag)
			continue;			//使用済み

		if(rFace.m_sNode != sNode) 
			continue;	//このノードに無いポリゴン

		// store the approximate current location
		s_NodeArea.MergeAABB( rFace.GetAABB() );

		SPlane& rPlane = rFace.GetPlane();

		//  If there is another face belonging to the same node && 
		//overlapping but facing the opposite, give up adopting this 
		//face as a partface.
		SPlane RevPlane = rPlane;
		RevPlane.normal *= (-1);
		RevPlane.dist *= (-1);
		stop = 0;
		for(k=0; k<m_pFace->size(); k++)
		{
			CMapFace& rFace2 = m_pFace->at(k);
			if( sNode != rFace2.m_sNode ) continue;
			SPlane& rPlane2 = rFace2.GetPlane();
			if( AlmostSamePlanes( RevPlane, rPlane2 ) )
			{
				stop = 1;
				break;
			}
		}
		if(stop)
			continue;

		int onplane = 0;
		int front = 0;
		int back = 0;
		int split = 0;
		//この（ポリゴン）平面を用いると、どんな分断が行われるか？
		for(j=0; j<m_pFace->size(); j++)
		{
			if(i==j)
				continue;	//自分は無視
			if(m_pFace->at(j).m_sNode != sNode) continue;	//このノードに無いポリゴン
			int c = ClassifyFace( rPlane, m_pFace->at(j) );
			switch( c )
			{
			case FCE_ONPLANE:	onplane++;	break;
			case FCE_FRONT:		front++;	break;
			case FCE_BACK:		back++;		break;
			case FCE_SPLIT:		split++;	break;
			}
		}
		// なるべく同一平面で、かといって分断が多いならいやで、前後のバランスが悪いのもちょっといや
		int weight = m_fOnplaneWeight * onplane
			- m_fSplitWeight * split
			- m_fBalanceWeight * abs( front - back );

		// favor axis-aligned plane - added on 070305
		if( fabsf(rPlane.normal.x) == 1.0f || fabsf(rPlane.normal.y) == 1.0f || fabsf(rPlane.normal.z) == 1.0f )
			weight += abs(weight) * 0.5f;

		if( maxweight < weight )
		{
			maxweight = weight;
			iBestFaceNum = i;
		}
	}
	if( iBestFaceNum != -1 )
		return &m_pFace->at( iBestFaceNum );
	else
		return NULL;

}

AABB3 CBSPTreeBuilder::MakeAABB()
{
	D3DXVECTOR3 vMax(-99999, -99999, -99999);
	D3DXVECTOR3 vMin( 99999,  99999,  99999);
	int i,j;
	for(i=0; i<m_pFace->size(); i++)
	{
		CFace& rFace = m_pFace->at(i);
		for(j=0; j<rFace.GetNumVertices(); j++)
		{
			D3DXVECTOR3& v = rFace.GetVertex( j );
			if (v.x < vMin.x) vMin.x = v.x;
			if (v.y < vMin.y) vMin.y = v.y;
			if (v.z < vMin.z) vMin.z = v.z;
			if (vMax.x < v.x) vMax.x = v.x;
			if (vMax.y < v.y) vMax.y = v.y;
			if (vMax.z < v.z) vMax.z = v.z;

		}
	}
	AABB3 aabb;
	aabb.vMax = vMax;
	aabb.vMin = vMin;

	return aabb;
}

void CBSPTreeBuilder::WriteBSPTree( const string& filename )
{
	FILE* fp = fopen(filename.c_str(), "w");

	int i = 0;
	int iNumNodes= m_aNode.size();
	char acStr[256];

	fprintf(fp,
		"\n Total Nodes: %d\n Faces before building: %d\n Faces after building: %d\n",
		iNumNodes, m_iNumInitialFaces, m_pFace->size() );

	fputs("\n[nodenum] -  [CLD_FRONT] [CLD_BACK]  (plane)\n", fp);
	for(i=0; i<iNumNodes; i++)
	{
		GetNodeString(acStr, i);
		fprintf(fp, "%s\n", acStr);
	}
	fclose(fp);
}

void CBSPTreeBuilder::GetNodeString(char* pcStr, int iNodeIndex)
{
//	char acVec[32];
	SNode& rNode = m_aNode[ iNodeIndex ];
	SPlane& rPlane = rNode.GetPlane();
//	D3DXVec3toStr(acVec, rPlane.normal, 2);
	sprintf(pcStr, "[%03d] - [%03d] [%03d] ( n%s, d=%.2f )  cell[%03d]",
			iNodeIndex, rNode.child[CLD_FRONT], rNode.child[CLD_BACK],
//			acVec, rPlane.dist, rNode.sCellIndex );
			to_string(rPlane.normal).c_str(), rPlane.dist, rNode.sCellIndex );
}


AABB3 CBSPTreeBuilder::MakeAABB_r(short s)
{
	SNode& rThisNode = m_aNode[s];

	short sFrontNode = rThisNode.child[CLD_FRONT];
	short sBackNode = rThisNode.child[CLD_BACK];
//	CMapFace* pFace;
	size_t i, num_faces = m_pFace->size();
	AABB3 aabb_front;
	AABB3 aabb_back;
	if( 0 <= sFrontNode && 0 <= sBackNode ) //diverging node
	{
		aabb_front = MakeAABB_r( sFrontNode );
		aabb_back = MakeAABB_r( sBackNode );
		rThisNode.aabb.Merge2AABBs(aabb_front, aabb_back);
		return rThisNode.aabb;
	}
	else if(sFrontNode < 0 && sBackNode < 0 ) //leaf node
	{
		aabb_front.vMax = D3DXVECTOR3(-999999, -999999, -999999);
		aabb_front.vMin = D3DXVECTOR3( 999999,  999999,  999999);

//		for(pFace = m_pFace->begin(); pFace != m_pFace->end(); pFace++)
//			if( pFace->m_sNode == s) pFace->AddToAABB(aabb_front);
		for( i=0; i<num_faces; i++ )
			if( m_pFace->at(i).m_sNode == s ) m_pFace->at(i).AddToAABB(aabb_front);

		rThisNode.aabb = aabb_front;
		return rThisNode.aabb;
	}
	else if( 0 <= sFrontNode )
	{
		aabb_front = MakeAABB_r( sFrontNode );
		rThisNode.aabb = aabb_front;
		return rThisNode.aabb;
	}
	else if( 0 <= sBackNode )
	{
		// This should not happen
		aabb_front.vMax = D3DXVECTOR3(-999999, -999999, -999999);
		aabb_front.vMin = D3DXVECTOR3( 999999,  999999,  999999);
//		for( pFace = m_pFace->begin(); pFace != m_pFace->end(); pFace++)
//			if( pFace->m_sNode == s) pFace->AddToAABB(aabb_front);
		for( i=0; i<num_faces; i++ )
			if( m_pFace->at(i).m_sNode == s ) m_pFace->at(i).AddToAABB(aabb_front);

		rThisNode.aabb.Merge2AABBs( aabb_front, MakeAABB_r(sBackNode) );
		return rThisNode.aabb;
	}

	return rThisNode.aabb;
}


void CBSPTreeBuilder::SortFacesByNode( vector<CMapFace>* paFace )
{
	int i, iNumFaces = paFace->size();
	for(i=0; i<iNumFaces; i++)
	{
		SortFaceToNode_r(0, i, paFace);
	}

	for(i=0; i<paFace->size(); i++)
	{
		if(paFace->at(i).m_sNode == CONTENTS_SOLID)
			paFace->erase( paFace->begin() + i );
	}


	sort( paFace->begin(), paFace->end() );

}

void CBSPTreeBuilder::SortFaceToNode_r(short sNodeIndex,
									   int iFaceIndex,
									   vector<CMapFace>* paFace )
{
	int c;
	float d;
	SNode& rThisNode = m_aNode[ sNodeIndex ];
	CMapFace& rFace = paFace->at( iFaceIndex );

	int index;
	short sFrontChild = rThisNode.child[CLD_FRONT];
	short sBackChild = rThisNode.child[CLD_BACK];
	SPlane& rPlane = rThisNode.GetPlane();

	c = ClassifyFace(rPlane, rFace, OPC_DONTCARE_NORMAL_DIRECTION);

	if(c == FCE_ONPLANE)
	{
		d = D3DXVec3Dot( &rPlane.normal, &(rFace.GetPlane().normal) );
		if(d < 0)	// if the 2 normals are facing the opposite directions
			c = FCE_FRONT;	//The adjacent faces are likely to be in the front sub-space of this node. We should send this face to the same side
		else
			c = FCE_BACK;	//The adjacent faces are likely to be in the back sub-space of this node. We should send this face to the same side
	}

	if(c == FCE_FRONT)
	{
		if(sFrontChild < 0)
			rFace.m_sNode = sNodeIndex;  //Set the node where this face is attached
		else
			SortFaceToNode_r( sFrontChild, iFaceIndex, paFace );
	}
	else if(c == FCE_BACK)
	{	
		if(sBackChild == CONTENTS_SOLID)
			rFace.m_sNode = CONTENTS_SOLID;
		else
			SortFaceToNode_r( sBackChild, iFaceIndex, paFace );
	}
	else  // c == FCE_SPLIT
	{
		CMapFace frontface, backface;
		rFace.Split(frontface, backface, rPlane);
		paFace->at( iFaceIndex ) = frontface;
		if(sFrontChild < 0)
			rFace.m_sNode = sNodeIndex;  //Set the node where this face is attached
		else
			SortFaceToNode_r( sFrontChild, iFaceIndex, paFace );
		index = paFace->size();
		paFace->push_back( backface );
		if(sBackChild == CONTENTS_SOLID)
//			rFace.m_sNode = CONTENTS_SOLID;	// error: after pushed back, the vector may have re-allocated the memory. In that case 'rFace' is no longer valid
			paFace->at( iFaceIndex ).m_sNode = CONTENTS_SOLID;
		else
			SortFaceToNode_r( sBackChild, index, paFace );
	}

	return;
}

void CBSPTreeBuilder::Reduce()
{
	vector<SNode> aTempBSPTree;
	ReduceNodes_r(0, &aTempBSPTree);

	this->m_aNode.clear();
	this->m_aNode.assign(aTempBSPTree.begin(), aTempBSPTree.end() );
	aTempBSPTree.clear();
}

short CBSPTreeBuilder::ReduceNodes_r(short sNodeIndex, vector<SNode>* pDestBSPTree)
{
	SNode& rNode = this->m_aNode.at( sNodeIndex );
	short sFrontChild = rNode.child[CLD_FRONT];
	short sBackChild = rNode.child[CLD_BACK];
	SNode newnode;

	if( 0 <= sFrontChild && 0 <= sBackChild )
	{	//diverging node
		pDestBSPTree->push_back( newnode );
		SNode& rNewNode = pDestBSPTree->back();
		rNewNode.aabb = rNode.aabb;
		rNewNode.sCellIndex = rNode.sCellIndex;
		rNewNode.child[CLD_FRONT] = ReduceNodes_r(sFrontChild, pDestBSPTree);
		rNewNode.child[CLD_BACK] = ReduceNodes_r(sBackChild, pDestBSPTree);
		return pDestBSPTree->size() - 1;
	}
	else if( sFrontChild < 0 && sBackChild < 0 )
	{	//leaf-node
		pDestBSPTree->push_back( newnode );
		SNode& rNewNode = pDestBSPTree->back();
		rNewNode.child[CLD_FRONT] = sFrontChild;
		rNewNode.child[CLD_BACK] = sBackChild;
		rNewNode.aabb = rNode.aabb;
		rNewNode.sCellIndex = rNode.sCellIndex;
		return pDestBSPTree->size() - 1;
	}
	else if(0 <= sFrontChild)
		return ReduceNodes_r(sFrontChild, pDestBSPTree);
	else if(0 <= sBackChild)
		return ReduceNodes_r(sBackChild, pDestBSPTree);

	return 0;
}

//Make a simple line-segment check in BSPTree.
//Returns CONTENTNS_SOLID if the given line-segment is blocked
//Returns CONTENTNS_EMPTY if the given line-segment is not blocked and ends up in an open area
short CBSPTreeBuilder::LineCheck_r( D3DXVECTOR3& vP1, D3DXVECTOR3& vP2,
								   short sPrevNodeIndex, short sNodeIndex)
{
//	SNode* pBSPTree = this->m_aNode.begin();
	vector<SNode>& pBSPTree = this->m_aNode;
	float dist1, dist2, f;
	D3DXVECTOR3 vMid;
	short forelinenode, rearlinenode;

	if(sNodeIndex == CONTENTS_SOLID)
		return CONTENTS_SOLID;	// the ray is blocked.
	else if(sNodeIndex == CONTENTS_EMPTY)
	{
		if( this->m_pCellBuilder )	//If this is a world BSP-Tree and has cells in it
		{
			CCell* pCell = m_pCellBuilder->GetCell( pBSPTree[sPrevNodeIndex].sCellIndex );
			if( pCell->m_pBSPTreeBuilder )	//If the cell has any internal fixed model
				return pCell->m_pBSPTreeBuilder->LineCheck_r( vP1, vP2 );	//line check against the model
			else
				return CONTENTS_EMPTY;	//no fixed model in this cell, and there is no obstacle between ray & face
		}
		else
			return CONTENTS_EMPTY;	//there is no obstacle between ray & face
	}
	//Further recursive checks are necessary
	SPlane& rPlane = pBSPTree[sNodeIndex].GetPlane();

	dist1 = D3DXVec3Dot(&rPlane.normal, &vP1) - rPlane.dist;
	dist2 = D3DXVec3Dot(&rPlane.normal, &vP2) - rPlane.dist;

	if(0 <= dist1 && 0 <= dist2)
		return LineCheck_r(vP1, vP2, sNodeIndex, pBSPTree[sNodeIndex].child[CLD_FRONT]);
	if(dist1 <= 0 && dist2 <= 0)
		return LineCheck_r(vP1, vP2, sNodeIndex, pBSPTree[sNodeIndex].child[CLD_BACK]);

	//Need to split the ray
	f = dist1 / (dist1 - dist2);
	if(f<0)
		f = 0;
	else if(1<f)
		f = 1;

	vMid = vP1 + (vP2 - vP1) * f;

	if(0 <= dist1)
	{	//'vP1' is in front of the 'rPlane'
		forelinenode = pBSPTree[sNodeIndex].child[CLD_FRONT];
		rearlinenode = pBSPTree[sNodeIndex].child[CLD_BACK];
	}
	else
	{	//'vP1' is behind the 'rPlane'
		forelinenode = pBSPTree[sNodeIndex].child[CLD_BACK];
		rearlinenode = pBSPTree[sNodeIndex].child[CLD_FRONT];
	}

	if( LineCheck_r(vP1, vMid, sNodeIndex, forelinenode) == CONTENTS_SOLID )
		return CONTENTS_SOLID;	//If the fore-line is blocked, there is no need to check the rear-line. The recursive process stops here.
	else
		return LineCheck_r(vMid, vP2, sNodeIndex, rearlinenode);

}
