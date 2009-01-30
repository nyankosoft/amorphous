#ifndef  __BSPTREEBUILDER_H__
#define  __BSPTREEBUILDER_H__

#include <vector>
#include <string>
using namespace std;

#include "3DMath/3DStructs.hpp"
#include "../Stage/bspstructs.hpp"


#define DEFAULT_ONPLANE_WEIGHT	4
#define DEFAULT_SPLIT_WEIGHT	6
#define DEFAULT_BALANCE_WEIGHT	1
enum {ONPLANE_GOTO_FRONTCLD, ONPLANE_GOTO_BACKCLD};

class CCellBuilder;

class CBSPTreeBuilder
{
	vector<CMapFace>* m_pFace;  //The source faces from which the BSP-Tree is made
	CCellBuilder* m_pCellBuilder;

	AABB3 m_aabb;
	int m_iTreeStyle;
	int m_iNumInitialFaces;
	float m_fOnplaneWeight;
	float m_fSplitWeight;
	float m_fBalanceWeight;

	vector<SNode> m_aNode;	//The BSP-Tree made by this builder

public:
	CBSPTreeBuilder();
	bool ConstructFrom(vector<CMapFace>* _pFace, int iTreeStyle = ONPLANE_GOTO_FRONTCLD);
	void Build_r(short sNode);
	CMapFace* SelectPartitionPlane(short sNode);

	vector<SNode>* GetBSPTree() { return &m_aNode; }
	AABB3 MakeAABB_r(short s);
	AABB3 MakeAABB();  //Get an AxisAlignedBoundingBox out of this BSP-Tree
	AABB3 GetAABB(){ return m_aabb; }
	void Reduce();
	short ReduceNodes_r(short sNodeIndex, vector<SNode>* pDestBSPTree);
	void SortFacesByNode( vector<CMapFace>* paFace );
	void SortFaceToNode_r(short sNodeIndex, int iFaceIndex, vector<CMapFace>* paFace );

	void SetCellBuilder(CCellBuilder* pCellBuilder){m_pCellBuilder = pCellBuilder;}

	void WriteBSPTree( const std::string& filename );
	void GetNodeString(char* pcStr, int iNodeIndex);
	short LineCheck_r(D3DXVECTOR3& vP1, D3DXVECTOR3& vP2,
		short sPrevIndex = 0, short sNodeIndex = 0);

	CBSPTreeBuilder operator=(CBSPTreeBuilder _BSPTBuilder);
};


#endif		/*  __BSPTREEBUILDER_H__  */

