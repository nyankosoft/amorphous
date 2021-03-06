#ifndef __BSPTREE_COLLISIONMODEL_EXPORTER_H__
#define __BSPTREE_COLLISIONMODEL_EXPORTER_H__

#include "BSPTree_CollisionModel.hpp"


namespace amorphous
{

class CBSPMapCompiler;
class CMapFace;
class CPlaneBuffer;
struct SNode;

#include <vector>
using namespace std;


struct SConcaveEdgeTempFlag
{
	bool bIsConcaveEdge[16];
};


class CBSPTree_CollisionModel_Exporter : public CBSPTree_CollisionModel
{
	CBSPMapCompiler* m_pMapCompiler;

	CBSPTree_CollisionModel_Exporter *m_paCell_CollisionModel_Export;


	void GetNodeIndex_r( int& riDestNodeIndex, int& rType, CMapFace& rFace,	int iNodeIndex = 0 );

	void MakePolygonFromFace(ConvexPolygon& rPolygon, CMapFace& rFace);

public:

	CBSPTree_CollisionModel_Exporter();

	~CBSPTree_CollisionModel_Exporter();

	void WriteToFile(FILE *fp);

	void AddPolygon();

	void SetCollisionModel( CBSPMapCompiler *pMapCompiler );

	void SetCollisionModel( CPlaneBuffer *pPlaneBuffer, vector<SNode> *pvecBSPTreeNode, vector<CMapFace> *pvecMapFace );

	void SetPolygonCollisionInfo();

	void SaveToFile(FILE *fp);

};

} // namespace amorphous



#endif  /*  __BSPTREE_COLLISIONMODEL_EXPORTER_H__  */