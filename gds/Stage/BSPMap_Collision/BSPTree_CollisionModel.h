#ifndef __BSPMAP_COLLISIONMODEL_H__
#define __BSPMAP_COLLISIONMODEL_H__

#include "3DMath/3DStructs.h"


struct SBSPNode
{
	SPlane plane;
	AABB3 aabb;
	int child[2];	// index to child nodes - child[0]: front node / child[1]: back node

	int iNumPlanePolygons;
	int iPlanePolygonIndex;
	int iNumCrossPolygons;
	int iCrossPolygonIndex;
	int iCellIndex;

	SBSPNode()
	{
		child[0] = child[1] = 0;
		iNumPlanePolygons = 0;
		iPlanePolygonIndex = 0;
		iNumCrossPolygons = 0;
		iCrossPolygonIndex = 0;
		iCellIndex = 0;
	}
};


class CConvexPolygon;

struct SNode;
class CPlaneBuffer;
class CMapFace;

#include <vector>
using namespace std;


class CBSPTree_CollisionModel
{
protected:
	int m_iNumBSPNodes;
	SBSPNode *m_paBSPNode;

	int m_iNumPolygons;
	CConvexPolygon *m_paPolygon;

//	CCell_CollisionModel *m_paCell;
	int m_iNumCells;
	CBSPTree_CollisionModel *m_paCell_CollisionModel;

	void CheckCollision_r( int iIndex, D3DXVECTOR3& vCenter, float fRadius, AABB3& aabb,
												  vector<int>& veciPolygonIndex );

public:
	CBSPTree_CollisionModel();
	~CBSPTree_CollisionModel();
	void Release();
	void LoadFromFile(FILE *fp);

	void CheckCollision( D3DXVECTOR3& vCenter, float fRadius, AABB3& aabb,
												  vector<int>& veciPolygonIndex );

	// The following two functions are intended to be implemented in collision model exporter
	virtual void SetCollisionModel(CPlaneBuffer *pPlaneBuffer,
		                           vector<SNode> *pvecBSPTreeNode,
								   vector<CMapFace> *pvecMapFace) {}
	virtual void SaveToFile(FILE *fp) {} 
};


#endif  /*  __BSPMAP_COLLISIONMODEL_H__  */