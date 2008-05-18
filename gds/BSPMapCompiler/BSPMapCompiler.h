#ifndef __BSPMAPCOMPILER_H__
#define __BSPMAPCOMPILER_H__


#include "fwd.h"
#include "BSPMapData.h"
#include "MapCompiler.h"

#include "LightmapBuilder.h"

#include "Stage/bspstructs.h"
#include "3DCommon/FloatRGBColor.h"
#include "3DCommon/LightStructs.h"
#include "Support/TextFileScanner.h"

#include <string>


struct SFog;
class CTriangleMesh;


//#include "3DMath/PolygonMesh.h"
/*
template<class CMapFace>
class CPolygonMesh;
*/


class CMapCompileOption
{
public:

	std::string SrcFilename;
//	std::string DestDirectory;
	std::string DestFilename;

	CLightmapOption Lightmap;

	void LoadFromFile( const std::string& filename );

};


class CBSPMapCompiler
{
private:

	std::string m_Filename;

	CBSPMapData* m_pMapData;

	CBSPTreeBuilder* m_pBSPTreeBuilder;

	CPortalBuilder* m_pPortalBuilder;

	CCellBuilder* m_pCellBuilder;

	CLightmapBuilder* m_pLightmapBuilder;

//	CPolygonMesh<CMapFace> *m_pPolygonMesh;
///	CPolygonMesh<CMapFace> m_PolygonMesh;

	CMapCompileOption m_CompileOption;

private:

	std::string MakeFilenameForDebugFile( const std::string& debug_info );

	void CreatePolygonMesh();

	void CreateLightmapTextures();

//	void GetOutputFilename( string& strDestFilename, const char *pcSrcFilename );

public:

	CBSPMapCompiler();

	CBSPMapData* GenerateMapdataContainer( const char* pFilename );

	int Compile( const std::string& desc_filename );

	vector<SNode>* GetBSPTree() { return m_pBSPTreeBuilder->GetBSPTree(); }

	vector<CCell>* GetCell() { return m_pCellBuilder->GetCell(); }

	vector<CMapFace>* GetFace() { return &m_pMapData->m_aMainFace; }

	vector<CMapFace>* GetSkyboxFace() { return &m_pMapData->m_aSkyboxFace; }

	CPlaneBuffer* GetPlaneBuffer() { return &m_pMapData->m_aPlane; }

	vector<CSGA_TextureFile>& GetTextureFile() { return m_pMapData->m_vecTextureFile; }

	vector<CSG_Surface>& GetSurface() { return m_pMapData->m_vecSurface; }

	vector<CBaseLight*>* GetLight() { return &m_pMapData->m_vecpLight; }

	SFog* GetFogData();

	// return info which represents a single triangle mesh of the map
	void GetTriangleMeshForCollision( vector<D3DXVECTOR3>& rvecvTriangleVertex,
		                              vector<int>& rveciTriangleIndex,
								      vector<int>& rveciMaterialIndex);

	vector<SFloatRGBColor*>* GetLightmapTexture();

	int GetLightmapTextureWidth();

	CLightmapBuilder *GetLightmapBuilder() { return m_pLightmapBuilder; }

	// check whether a line segment is blocked by obstacles
	// used to create lightmaps
	short LineCheck( Vector3& vP1, Vector3& vP2 );

};


#endif  /*  __BSPMAPCOMPILER_H__  */
