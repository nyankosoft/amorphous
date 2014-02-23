#ifndef __BSPSTRUCTS_H__
#define __BSPSTRUCTS_H__


#include <vector>
#include <list>
#include "3DMath/3DStructs.hpp"
#include "BSPMapCompiler/BaseFace.hpp"
#include "BSPMapCompiler/MapFace.hpp"


namespace amorphous
{


#define OUTSIDE_NODE -32767
#define HEAD_NODE 0
enum { CLD_FRONT, CLD_BACK };


struct SNode
{
	CPlaneBuffer* pPlaneBuffer;
	short sPlaneIndex;
	short child[2];
	short sCellIndex;
	AABB3 aabb;
	
	SNode() {memset(this, 0, sizeof(SNode)); sCellIndex = -1; }
	~SNode() {}

	SPlane& GetPlane(){ return pPlaneBuffer->at( sPlaneIndex ); }
	bool operator<(SNode& node){}
	bool operator==(SNode& node){}

};


class CPortalFace : public CFace
{
public:
	short m_asNode[2];
	short m_asCell[2];

	short GetFrontCellIndex() { return m_asCell[PRT_FRONT]; }
	short GetBackCellIndex() { return m_asCell[PRT_BACK]; }
	CPortalFace(){ m_asNode[0] = m_asNode[1] = 0; m_asCell[0] = m_asCell[1] = -1; }
	CPortalFace(const CPortalFace& face);
	CPortalFace operator=(CPortalFace  face);
};


class CBSPTreeBuilder;

class CCell
{
public:
	short m_sNode;
	std::vector<short> m_asPortalIndex;
	std::list<short> m_sVisCellIndex;
	CBSPTreeBuilder* m_pBSPTreeBuilder;
	std::vector<CMapFace> m_aInteriorFace;

	CCell(){ m_sNode = 0; m_pBSPTreeBuilder = NULL; }
	~CCell();
	int GetNumPortals(){ return (int)m_asPortalIndex.size(); }
	short GetPortalIndex(short s){ return m_asPortalIndex[s]; }
	CCell(const CCell& rCell);
	void SortInteriorFacesByTextureID();
	bool operator<(CCell& rCell) { return ( m_sNode < rCell.m_sNode ); }
	bool operator==(CCell& rCell) { return ( m_sNode == rCell.m_sNode ); }
};


//-----------------------------------------------------------------------------
// Global functions
//-----------------------------------------------------------------------------

enum OnPlaneCondition { OPC_IF_NORMAL_SAME_DIRECTION = 1, OPC_DONTCARE_NORMAL_DIRECTION };

extern int	ClassifyFace( const SPlane& plane, CFace& face,
						 int iOnPlaneCondition = OPC_IF_NORMAL_SAME_DIRECTION);
} // namespace amorphous



#endif  /*  __BSPSTRUCTS_H__  */