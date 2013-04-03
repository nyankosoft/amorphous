#ifndef __CELLBUILDER_H__
#define __CELLBUILDER_H__


#include <vector>
#include <string>
using namespace std;

#include "fwd.hpp"

#include "3DMath/3DStructs.hpp"
#include "Stage/bspstructs.hpp"


namespace amorphous
{


class CCellBuilder
{
	vector<CCell> m_aCell;
	CBSPTreeBuilder* m_pBSPTreeBuilder;
	vector<CPortalFace>* m_paPortal;

public:

	CCellBuilder() : m_pBSPTreeBuilder(NULL) {}
	CCellBuilder(CBSPTreeBuilder* pBSPTBuilder);// {m_pBSPTree = pBSPTBuilder->GetBSPTree(); }
	void EnumerateCells_r(short sNodeIndex);
	vector<CCell>* GetCell() { return &m_aCell; }
	CCell* GetCell(int index) { return &m_aCell.at(index); }
	void SetPortalBuffer(vector<CPortalFace>* paPortal) { m_paPortal = paPortal; }
	void MakePVS();
	void WriteCells( const std::string& filename );
	void AddFixedModel(vector<CMapFace>* paFace);
	void AddInteriorModels(vector<CMapFace>* paFace);
};

} // amorphous



#endif  /*  __CELLBUILDER_H__  */
