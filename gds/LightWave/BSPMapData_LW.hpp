#ifndef __BSPMAPDATA_LW_H__
#define __BSPMAPDATA_LW_H__


#include "../BSPMapCompiler/BSPMapData.hpp"
#include "LWO2_Object.hpp"


namespace amorphous
{


//==================================================================================
// CBSPMapData_LW
//   | map data class designed to load a "*.lwo" ( LWO2 - lightwave object) file  |
//   | as a map file                                                             |
//==================================================================================

class CLightWaveSceneLoader;

class CBSPMapData_LW : public CBSPMapData
{
	LWO2_Object m_LWO2Object;

private:

	int GetMaterialIndex( LWO2_Surface& rSurf );

	void SetSurface();

	void SetFakeBumpTextures();

	void SetTextureFilename();

	/// for occlusion testing
	void CreateTriangleMesh();

public:

	int LoadSpecificMapDataFromFile( const char* pFilename );

	void SetFace(vector<CMapFace>* pFace, list<LWO2_Layer>::iterator itr);

	void SetVertexColor( MAPVERTEX& rvDestVertex, DWORD dwPointIndex, DWORD dwPolygonIndex, LWO2_VertexColorMap *pVertexColorMap );

	void SetLight( CLightWaveSceneLoader& rLightWaveScene );

//	void SetUVsForLightmaps();


};
} // amorphous



#endif	/*  __BSPMAPDATA_LW_H__  */