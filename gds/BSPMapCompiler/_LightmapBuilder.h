#ifndef __LIGHTMAPBUILDER_H__
#define __LIGHTMAPBUILDER_H__


#include <vector>
using namespace std;

#include "3DCommon/LightStructs.h"
#include "3DCommon/MeshModel/3DMeshModelArchive.h"
#include "3DCommon/MeshModel/General3DMesh.h"
using namespace MeshModel;

#include "3DMath/Vector3.h"
#include "3DMath/Matrix34.h"
#include "Graphics/Rect.h"
#include "Graphics/RectTree.h"
using namespace Graphics;

#include "Support/TextFileScanner.h"
#include "Support/FixedVector.h"

#include "fwd.h"
#include "LightmapTexture.h"
//#include "LightingForLightmap_SimpleRaytrace.h"


#define LMB_NUM_MAX_LIGHTMAPTEXTURES	64


#define LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE	1

//enum { LFALLOFF_LINEAR, LFALLOFF_INVDIST, LFALLOFF_INVDISTSQD };

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
//	vector<C3DMeshModelArchive> m_MeshArchive;

	/// [in] mesh for environment light
	/// rendered with texture color only?
	C3DMeshModelArchive m_EnvLightMesh;

	int RenderTargetTexWidth;
	int RenderTargetTexHeight;

public:

	AOLightmapDesc()
		:
//	pvecLightmap(NULL),
	RenderTargetTexWidth(32),
	RenderTargetTexHeight(32)
	{}
};


class CLightmapOption
{
public:

	float fTexelSize;
	int TextureWidth;
	int TextureHeight;
	bool bCreateLightDirectionMap;
//	bool bUseRayCastLightmap;	// simple lightmap technique for point & directional lighting
	bool bRayCastDirLight;
	bool bRayCastPointLight;

	int AO_SceneResolution;

public:

	CLightmapOption()
		:
	fTexelSize(1.0f),
	TextureWidth(256),
	TextureHeight(256),
	bCreateLightDirectionMap(false),
	AO_SceneResolution(32)
	{}

	void LoadFromFile( CTextFileScanner& scanner );
};


class LightmapDesc
{
public:

	/// target geometry
	CGeneral3DMesh *m_pMesh;

	/// controls whether lightmaps are created for a surface or not
	std::vector<int> m_vecEnableLightmapForSurface;

	std::vector<boost::shared_ptr<CLight>> *m_pvecpLight;

	/// texture coordinates index for lightmap texture
	/// - valid range: [0,3]
	/// - index 0 is usually used for color channel texture, so this
	///   value is probably 1 or higher
	/// - default value: 1
	int m_LightmapTextureCoordsIndex;

	/// texture index in CMMA_Material
	int m_LightmapTextureArchiveIndex;

	/// Used by lightmap textures, which require (db filename) + (key name)
	std::string m_OutputDatabaseFilepath;

//	CPolygonMesh m_RayTraceGeometry;

	CLightmapOption m_Option;

	AOLightmapDesc AOLightmap;

public:

	LightmapDesc()
		:
	m_LightmapTextureCoordsIndex( 1 )
	{
	}

};


//==========================================================================================
// CLightmapBuilder
//==========================================================================================


/**
 input:
   1. general 3d mesh
   - create lightmaps for polygons
   - also create geometry for ray tracing
   2. lights

 output:
   general 3d mesh with texture coordinates for lightmap textures
   lightmap textures

*/
class CLightmapBuilder
{
private:

	/// stores that following properties
	/// - texel size on lightmaps (in meters)
	/// - texture size
	/// - etc.
	LightmapDesc m_Desc;

//	CLightingForLightmap<CMapFace>* m_pLightingSimulator;

///	CLightingForLightmap_SimpleRaytrace<CMapFace>* m_pLightingSimulator;

	float m_fMaxAllowedLightmapArea;

	/// Resolution of the lightmaps
	/// - size of each texel in meter
	/// 5/10/2008 - moved to m_Desc
//	float m_TexelSize;

	/// size configuraiton variables about 'lightmap texture' and 'lightmap'
	/// - 128, 256, 512 or higher...
	/// 5/10/2008 - moved to m_Desc
//	int m_iTextureWidth, m_iTextureHeight;

	/// each lightmap texture has marginal texels so that it could be used with linear texture filtering
	int m_iMargin;

	std::vector<CLightmap> m_vecLightmap;

//	vector<CLightmapTexture> m_vecLightmapTexture;
	TCFixedVector<CLightmapTexture, LMB_NUM_MAX_LIGHTMAPTEXTURES> m_vecLightmapTexture;

	int m_iLightmapCreationFlag;


private:

	void Clear();

	void GroupFaces();

	void CalculateLightMapPosition( CLightmap& lightmap );

	// calculate vectors which point to each texel on the lightmap
	void SetUpLightMapPoints( CLightmap& lightmap );

	void PackLightmaps();

	void UpdateLightmapTextures();

	/// transform the light direction vectors on each lightmap into local space
	void TransformLightDirectionToLocalFaceCoord();

	void FillMarginRegions();

	void ApplySmoothing( float fCenterWeight );

	void ApplySmoothingToLightmaps();

	void UpdateMeshMaterials();

	void SetTextureWidth( const int iTextureWidth )  { m_Desc.m_Option.TextureWidth   = iTextureWidth; }
	void SetTextureHeight( const int iTextureHieght ) { m_Desc.m_Option.TextureHeight = iTextureHieght; }

	float GetTexelSize() const { return m_Desc.m_Option.fTexelSize; }

	int GetLightmapTextureWidth() const { return m_Desc.m_Option.TextureWidth; }
	int GetLightmapTextureHeight() const { return m_Desc.m_Option.TextureHeight; }

	void SetOption( const CLightmapOption& option );


//	void ComputeNormalsOnLightmap(vector<CMapFace>& rvecFace);
//	void SetLightmapTextureIndicesToPolygons();
//	void SetTextureCoords();
/*
	void CalculateLightmapTexelIntensity(CLightmap& rLightmap);	//determine how bright each texel is according to the intensity of lights pointed to the texel
*/

public:

	CLightmapBuilder();

	~CLightmapBuilder();

	void Init();

	/// desc - [in,out] polygons for which the lightmaps will be created
	/// desc - [in] static geometry used for ray tracing
	virtual bool CreateLightmapTexture( LightmapDesc& desc );

	/// access to lightmap textures

	int GetNumLightmapTextures() { return m_vecLightmapTexture.size(); }
	CLightmapTexture& GetLightmapTexture( int i ) { return m_vecLightmapTexture[i]; }

	int GetCreationFlag() { return m_iLightmapCreationFlag; }

	void SaveLightmapTexturesToImageFiles( const std::string& dirpath_and_bodyname,
										   const std::string& img_file_suffix );

};


#endif  /*  __LIGHTMAPBUILDER_H__  */
