#ifndef __LIGHTMAPBUILDER_H__
#define __LIGHTMAPBUILDER_H__

#include "BSPMapCompiler.h"

#include "../3DCommon/LightStructs.h"
#include "../Stage/bspstructs.h"

#include "LightmapTexture.h"

#include "3DMath/Vector3.h"
#include "3DMath/Matrix34.h"

#include "Graphics/Rect.h"
#include "Graphics/RectTree.h"
using namespace Graphics;

#include "LightingForLightmap_SimpleRaytrace.h"

#include <vector>
using namespace std;

#include "Support/FixedVector.h"

#define LMB_NUM_MAX_LIGHTMAPTEXTURES	64


#define LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE	1

//enum { LFALLOFF_LINEAR, LFALLOFF_INVDIST, LFALLOFF_INVDISTSQD };

class CMapFace;
class CBSPMapCompiler;
class CLightmapOption;

/*
template<class CMapFace>
class CLightingForLightmap;

template<class CMapFace>
class CPolygonMesh;
*/


class AOLightmapDesc
{
public:

	/// [in, out]
//	vector<CLightmap> *pvecLightmap;

	/// [in] occluding geometry
	vector<C3DMeshModelArchive> m_MeshArchive;

	/// [in] mesh for environment light
	/// rendered with texture color only?
	C3DMeshModelArchive m_EnvLightMesh;

	int RenderTargetTexWidth;
	int RenderTargetTexHeight;

	AOLightmapDesc()
		:
//	pvecLightmap(NULL),
	RenderTargetTexWidth(32),
	RenderTargetTexHeight(32)
	{}
};


class LightmapDesc
{
public:

	/// [in,out] polygons for which the lightmaps will be created
	vector<CMapFace>* pvecFace;

	/// [in, out]
//	vector<CLightmap> *pvecLightmap;

	CPolygonMesh<CMapFace>* pPolygonMesh;
	/// rvecFace - [in,out] polygons for which the lightmaps will be created

	CLightmapOption Option;

	AOLightmapDesc AOLightmap;
};


//==========================================================================================
// CLightmapBuilder
//==========================================================================================

class CLightmapBuilder
{
private:

	CBSPMapCompiler* m_pMapCompiler;

//	CLightingForLightmap<CMapFace>* m_pLightingSimulator;
	CLightingForLightmap_SimpleRaytrace<CMapFace>* m_pLightingSimulator;

	float m_fMaxAllowedLightmapArea;

	int m_iNumCurrentLightmaps;	//How many lightmaps have been created

	/// controlles the scaling of the lightmap texture
	/// how many texels corresponds to one meter of the model data
//	float m_TexelsPerMeter;

	float m_TexelSize;

	/// size configuraiton variables about 'lightmap texture' and 'lightmap'
	/// 128, 256, 512 or higher...
	int m_iTextureWidth, m_iTextureHeight;

	/// each lightmap texture has marginal texels so that it could be used with linear texture filtering
	int m_iMargin;

	vector<CLightmap> m_vecLightmap;

//	vector<CLightmapTexture> m_vecLightmapTexture;
	TCFixedVector<CLightmapTexture, LMB_NUM_MAX_LIGHTMAPTEXTURES> m_vecLightmapTexture;

	int m_iLightmapCreationFlag;


private:

	void Clear();

	void GroupFaces( vector<CMapFace>& rvecFaces );
//	void SelectCloseFaces( vector<int>* paiGroupedFacesIndex, vector<CMapFace>* paFaces);

	void CalculateLightMapPosition(CLightmap& lightmap, vector<CMapFace>& rvecFaces);

	// calculate vectors which point to each texel on the lightmap
	void SetUpLightMapPoints(CLightmap& rLightmap);

	void ComputeNormalsOnLightmap(vector<CMapFace>& rvecFace);

	void PackLightmaps();

	void SetLightmapTextureIndicesToPolygons( vector<CMapFace>& rvecFace );

//	void SetTextureCoords();

	void UpdateLightmapTextures();

	/// transform the light direction vectors on each lightmap into local space
	void TransformLightDirectionToLocalFaceCoord( vector<CMapFace>& rvecFace );

	void FillMarginRegions();

	void ApplySmoothing( float fCenterWeight );

	void ApplySmoothingToLightmaps();

	void SetTextureWidth( const int iTextureWidth )  { m_iTextureWidth = iTextureWidth; }
	void SetTextureHeight( const int iTextureHieght ) { m_iTextureHeight = iTextureHieght; }

	int GetLightmapTextureWidth() { return m_iTextureWidth; }
	int GetLightmapTextureHeight() { return m_iTextureHeight; }

	void SetOption( const CLightmapOption& option );

/*
	void CalculateLightmapTexelIntensity(CLightmap& rLightmap);	//determine how bright each texel is according to the intensity of lights pointed to the texel
*/

public:

	CLightmapBuilder();

	~CLightmapBuilder();

	void Init( CBSPMapCompiler* pMapCompiler );

	/// rvecFace - [in,out] polygons for which the lightmaps will be created
	/// rPolygonMesh - [in] static geometry used for ray tracing
//	virtual bool CreateLightmapTexture( vector<CMapFace>& rvecFace, CPolygonMesh<CMapFace>& rPolygonMesh );
	virtual bool CreateLightmapTexture( LightmapDesc& desc );

	/// access to lightmap textures

	int GetNumLightmapTextures() { return m_vecLightmapTexture.size(); }
	CLightmapTexture& GetLightmapTexture( int i ) { return m_vecLightmapTexture[i]; }

//	void SetTexelsPerMeter( int iNumTexelsPerMeter ) { m_TexelsPerMeter = iNumTexelsPerMeter; }

	int GetCreationFlag() { return m_iLightmapCreationFlag; }

	void OutputLightmapTexturesToBMPFiles( const char *pcBodyFileName );

};


#endif  /*  __LIGHTMAPBUILDER_H__  */