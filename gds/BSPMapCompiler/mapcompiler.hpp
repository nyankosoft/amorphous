#ifndef __MAPCOMPILER_H__
#define __MAPCOMPILER_H__

#include <string>

#include "../Stage/bspstructs.h"
#include "../BSPMapCompiler/BSPTreeBuilder.h"
#include "../BSPMapCompiler/CellBuilder.h"
#include "../BSPMapCompiler/BSPMapData.h"

#include "../LightWave/LWO2_Object.h"

#include "JigLib/TriangleMesh.h"

#include "Support/Vec3_StringAux.h"


#define DEFAULT_NUM_VERTICES	4096
#define DEFAULT_NUM_MAINFACES	1024
#define DEFAULT_NUM_PLANES		2048
#define DEFAULT_NUM_NODES		2048


//class CBSPMapData;
class CBaseLight;
struct SFog;

class CPortalBuilder
{
	vector<CPortalFace> m_aPortal;
	CPlaneBuffer m_aPortalPlane;
	vector<SNode>* m_pBSPTree;
	vector<CCell>* m_paCell;
	
public:
	void ConstructFrom( CBSPTreeBuilder* pBSPTBuilder, CCellBuilder* pCellBuilder );

	void MakeNodePortals_r(short sNodeIndex);
	void MakeBoundingPortals(AABB3 aabb);
	void RemoveInvalidPortals();
	void AttachPortalToCell();
	void WritePortals( const std::string& filename );

};

//あるセル内から、そのセルに接続している１つのポータルを通して、そのセルの
//外を覗いたときに、どのポータルが見えるかを計算するクラス
//This class determines which portals are visible when we see the world 
//through a portal in a cell.
class CPortalVisibilityBuilder
{
protected:
	short* m_psPortalVisibility;
	vector<CCell> *m_paCell;
	vector<CPortalFace> *m_paPortal;

	CPortalVisibilityBuilder* m_pPrevVisBuilder;
	short* m_psPrevVisibility;

public:
	CPortalVisibilityBuilder(){}
	CPortalVisibilityBuilder(vector<CCell>* paCell, vector<CPortalFace> *paPortal,
		CPortalVisibilityBuilder* pPtrlVisBuilder = NULL);
	~CPortalVisibilityBuilder();
	virtual short* SetVisibility(int iCellIndex, int iPortalIndex2) = 0;
	short* GetVisibilityInfo(){ return m_psPortalVisibility; }
};

class CPositionalVisibilityBuilder : public CPortalVisibilityBuilder
{
public:
	CPositionalVisibilityBuilder(vector<CCell>* paCell, vector<CPortalFace> *paPortal,
		CPortalVisibilityBuilder* pPtrlVisBuilder = NULL)
		: CPortalVisibilityBuilder(paCell, paPortal, pPtrlVisBuilder) {}
	short* SetVisibility(int iCellIndex, int iPortalIndex2);
	void SetPositionPortalVisibility(CCell& rCell, CPortalFace& rPortal);
};

class CTracingVisibilityBuilder : public CPortalVisibilityBuilder
{
public:
	CTracingVisibilityBuilder(vector<CCell>* paCell, vector<CPortalFace> *paPortal,
		CPortalVisibilityBuilder* pPtrlVisBuilder)
		: CPortalVisibilityBuilder(paCell, paPortal, pPtrlVisBuilder) {}
	short* SetVisibility(int iCellIndex, int iPortalIndex2);
	void SetTraversePortalVis_r(CCell& rCell);
};

class CClippingVisibilityBuilder : public CPortalVisibilityBuilder
{
public:
	CClippingVisibilityBuilder(vector<CCell>* paCell, vector<CPortalFace> *paPortal,
		CPortalVisibilityBuilder* pPtrlVisBuilder)
		: CPortalVisibilityBuilder(paCell, paPortal, pPtrlVisBuilder) {}
	short* SetVisibility(int iCellIndex, int iPortalIndex2);
	void SetClippingPortalVis_r( short sCellIndex, CPortalFace* pSrcPortal, CPortalFace* pDestPortal );
	bool VisibilityClipping(CPortalFace& rSrcPortal, CPortalFace& rDestPortal, CPortalFace& rTgtPortal);
};


#endif		/*  __MAPCOMPILER_H__  */
