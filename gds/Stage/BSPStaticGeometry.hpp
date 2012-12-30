#ifndef  __BSPSTATICGEOMETRY_H__
#define  __BSPSTATICGEOMETRY_H__


#include "3DMath/3DStructs.hpp"
#include "3DMath/AABTree.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/FloatRGBColor.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/ShaderHandle.hpp"
#include "Graphics/Shader/Serialization_ShaderTechniqueHandle.hpp"
#include "Physics/fwd.hpp"
#include "Support/StringAux.hpp"
#include "fwd.hpp"
#include "StaticGeometryBase.hpp"
#include "StaticGeometryArchiveFG.hpp"

#include <vector>
#include <list>


namespace amorphous
{
using namespace std;


// ===================================== deprecated =====================================


#define BSPFILE_VERSION 0.37f


enum eDataLumps
{
	TAG_HEADER = 0,
	TAG_FVFVERTEX,
	TAG_FVFINDEX,
	TAG_POLYGON,
	TAG_TRIANGLESET,
	TAG_PLANE,
	TAG_BSPTREENODE,
	TAG_FIXEDCONVEXMODEL,
	TAG_BSPCELL,
	TAG_VISCELLINDEX,
	TAG_SURFACE,
	TAG_TEXTUREFILE,
	TAG_LIGHTMAPTEXTURE,
	TAG_LIGHTDIRECTIONTEXTURE,
	TAG_SKYBOX,
	TAG_FOG,
	TAG_BSPTREE_COLLISIONMODEL,
	TAG_TRIANGLEMESH
};


struct SDataTAGLump
{
	DWORD dwTAG;
	DWORD dwSize;
};


class CSG_Surface
{
public:

	int ColorTexIndex;
	int NormalMapTexIndex;
	int FakeBumpTexIndex;

	CSG_Surface() { ColorTexIndex = NormalMapTexIndex = FakeBumpTexIndex = 0; }
};


class CSGA_TextureFile
{
public:
	char acFilename[256];

	CSGA_TextureFile() { memset( acFilename, 0, sizeof(char) * 256 ); }
};



// holds surface texture / normal map texture for static geometry
class CSG_TextureCache
{
public:
	std::string strFilename;
	LPDIRECT3DTEXTURE9 pTexture;

	CSG_TextureCache() : pTexture(NULL) {}
};


class CSG_Lightmap
{
public:
	LPDIRECT3DTEXTURE9 pTexture;
	LPDIRECT3DTEXTURE9 pLightDirMap;	// texture that represents light directions

	CSG_Lightmap() { pTexture = pLightDirMap = NULL; }
};


struct STrace;

class CBSPTree;


//class TCFixedVector<CSG_Surface, CBSPMap::NUM_MAX_SURFACES>;

#define SG_NUM_MAX_DYNAMIC_LIGHTS_PER_POLYGON	3


class CSG_Polygon
{
public:

	/// index offset holds the direct index of a vertex in vertex buffer
	short sIndexOffset;
	short sNumVertices;

	short sTextureID;
	short sLightMapID;

	bool bVisible;
	bool bNoClip;
	bool bLightSource;
	bool bDraw;		// used at runtime. set if the polygon should be drawn at a frame

	char acDynamicLightIndex[SG_NUM_MAX_DYNAMIC_LIGHTS_PER_POLYGON];
	char acDynamicLightIntensity[SG_NUM_MAX_DYNAMIC_LIGHTS_PER_POLYGON];

	char cPlaneType;	// plane type - 0/1/2: perpendicular to x/y/z-axis / 5: others
	
//	static LPDIRECT3DDEVICE9 s_pd3dDevice; // Our rendering device

//	static TCFixedVector<CSG_Surface, CBSPMap::NUM_MAX_SURFACES>* s_pvecMapSurface;

public:
	
	inline void Draw( LPDIRECT3DDEVICE9 pd3dDevice );

	/// draw polygon without changing any rendering state
	/// called after textures are set
	inline void DrawWithoutTextureSettings( LPDIRECT3DDEVICE9 pd3dDevice )
	{
		if( !bVisible )
			return;	// invisible polygons are not rendered

		pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, sIndexOffset, sNumVertices - 2);
	}

};


/// holds triangle lists
class CSG_TriangleSet
{
public:
	short sTextureID;
	short sLightMapID;

	bool bLightSource;

	int iStartIndex;
	int iNumTriangles;
	int iMinIndex;
	int iNumVertexBlocksToCover;

public:

	CSG_TriangleSet() { iStartIndex = 0; iNumTriangles = 0; sTextureID = sLightMapID = -1;}
	~CSG_TriangleSet() {}

};


class CSG_Cell
{
public:
	// index into the polygons which compose this cell
	// holds a first polygon index of a block of successive indices
	short sCellPolygonIndex;

	short sNumCellPolygons;

	short sInteriorPolygonIndex;	// index into the polygons which represent the interior models in this cell.
	short sNumInteriorPolygons;

	int iTriangleSetIndex;
	int iNumTriangleSets;

	short sBSPTreeIndex;	//offset in the array of 'SNode_f' which includes the BSP-Tree of the FCM in this cell
	short sFixedModelIndex;  //index into the stationary objects array
	short sNumFixedModels;

	Sphere sphere;	//bounding sphere which contains this cell
	AABB3 aabb;	//AxisAligned-BoundingBox which contains this cell

	int iVisCellIndexOffset;
	short sNumVisCells;

public:

	CSG_Cell() { memset(this, 0, sizeof(CSG_Cell)); }
};


class CSG_LightmapTextureInfo
{
public:
	int iNumLightmapTextures;
	int iLightmapTextureWidth;
};


class CSG_TriMeshHeader
{
public:
	int iNumVertices;
	int iNumTriangles;
};



struct SBSPFileHeader
{
	char ID[12];
	float version;
};



/* LWS file fog type
FogType 0 Off
		1 Linear
		2 Nonliner1
		3 Nonliner2
*/
struct SFog
{
	char cFogType;
	float fMinDist;
	float fMaxDist;
	float fMinAmount;
	float fMaxAmount;
	SFloatRGBColor color;
};



//	Stationary, unbreakable solid objects, such as stairs and columns, are 'Fixed Convex Models'
//	The 'FCM' has to be convex volume
//	represented by the world coordinate
struct SFixedConvexModel
{
	short sPolygonIndex;
	short sNumPolygons;
	
	AABB3 aabb;
};



/*
class CSGA_StaticGeometryArchive : public IArchiveObjectBase
{
public:
}
*/


//----------------------------------- cut here -----------------------------------
//#include "StaticGeometry.hpp"



//'sTextureID' or 'sLightMapID' is -1 if no texture is assigned to the polygon
inline void CSG_Polygon::Draw( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( !bVisible )
		return;	// invisible polygons are not rendered

	// texture setting
	if(0 <= sTextureID /* &&s_pvecMapSurface[ sTextureID ].pTexture*/)
	{
//		s_pd3dDevice->SetTexture( 0, s_pvecMapSurface[ sTextureID ].pTexture );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
	}
	else
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );

/*	if( s_paLightmap && 0 <= sLightMapID )
	{
		pd3dDevice->SetTexture( 1, s_paLightmap[ sLightMapID ].pTexture );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
	}
	else*/
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );

	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, sIndexOffset, sNumVertices - 2);

}



} // amorphous



#endif /* __BSPSTATICGEOMETRY_H__*/

