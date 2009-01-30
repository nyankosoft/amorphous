#ifndef __BSPMAPFILEEXPORTER_H__
#define __BSPMAPFILEEXPORTER_H__


#include <string>

#include "../Graphics/FVF_MapVertex.h"

#include "BSPStaticGeometry.hpp"

#include "BinaryNode.hpp"

class CBSPMapCompiler;
class CSkybox;
class CBSPTree_CollisionModel_Exporter;



class CBSPMapFileExporter
{
	vector<SNode_f> m_aBSPTree_f;

	vector<CSG_Polygon> m_vecPolygon;

	vector<CSG_TriangleSet> m_vecTriangleSet;

	vector<short> m_asIndexBuffer;

	vector<MAPVERTEX> m_aVertexBuffer;
	vector<CSG_Cell> m_aCellData;
	list<short> m_asVisibleCellIndex;


	CSkybox* m_pSkybox;

	vector<SFixedConvexModel> m_aFCModel;

	CBSPTree_CollisionModel_Exporter* m_pBSPCollisionModelExporter;

	string m_strTextureFilePath;

	CBSPMapCompiler* m_pMapCompiler;

private:

	void CreateTriangleSetsFromCellPolygons( CSG_Cell& rCell );

	void WriteTextures(FILE* fp);

	void WriteSurfaces(FILE* fp);

public:

	CBSPMapFileExporter( CBSPMapCompiler* pMapCompiler ) { m_pMapCompiler = pMapCompiler; }

	~CBSPMapFileExporter();

	void OutputBSPFile( const std::string filename );

	void SetBSPTree( vector<SNode>* pBSPTree );
	void AddFaceToPolygonBuffer(CMapFace& rFace);
	short AddVertexAndGetIndex(MAPVERTEX v);
	void SetCells();
	void AddFixedConvexModel_r(short sNodeIndex, vector<SNode>* pBSPTree, CCell& rCell);
	void WriteLightmapTexture(FILE* fp);
	void SetSkybox();

	// output triangle mesh used for collision detection
	void OutputTriangleMesh( FILE *fp );


	void OutputVertexDataToFile( const std::string& filename );

};

#endif  /*  __BSPMAPFILEEXPORTER_H__  */