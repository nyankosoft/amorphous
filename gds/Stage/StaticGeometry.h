#ifndef  __SG_STATICGEOMETRY_H__
#define  __SG_STATICGEOMETRY_H__


#include "3DMath/3DStructs.h"
#include "3DMath/AABTree.h"
#include "3DCommon/fwd.h"
#include "3DCommon/FloatRGBColor.h"
#include "3DCommon/MeshObjectHandle.h"
#include "3DCommon/ShaderHandle.h"
#include "3DCommon/Shader/Serialization_ShaderTechniqueHandle.h"
#include "Physics/fwd.h"
#include "Support/StringAux.h"
#include "fwd.h"
#include "StaticGeometryBase.h"
#include "StaticGeometryArchiveFG.h"

#include <vector>
#include <list>
using namespace std;


class CStaticGeometryDBKey
{
public:

	static const char *Main;
	static const char *CollisionGeometryStream;
	static const char *GraphicsMeshArchive;
//	static const char *Shaders;
//	static const char *MeshSubsetTree;
};


inline string GetGraphicsMeshArchiveKey( int index )
{
	return string(CStaticGeometryDBKey::GraphicsMeshArchive) + fmt_string( "[%d]", index );
}


class CShaderContainer : public IArchiveObjectBase
{
public:

//	std::string ShaderFilepath;

//	boost::shared_ptr<CShaderManager> m_pShaderManager;

	CShaderHandle m_ShaderHandle;

	std::vector<CShaderTechniqueHandle> m_vecTechniqueHandle;

public:

	void Serialize( IArchive& ar, const unsigned int version )
	{
//		ar & ShaderFilepath;
		ar & m_ShaderHandle;
		ar & m_vecTechniqueHandle;
	}
};


class CStaticGeometryMeshHolder : public IArchiveObjectBase
{
public:

	CMeshObjectHandle Mesh;

	/// bounding volumes of the mesh
	AABB3 aabb;
//	OBB obb;
//	Sphere Sphere;

	// key string for the database
//	std::string Key;

public:

	CStaticGeometryMeshHolder()
	{
		aabb.Nullify();
	}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & Mesh;
		ar & aabb; // & obb & Sphere
//		ar & Key
	}
};


class CStaticGeometryArchive : public IArchiveObjectBase
{

public:

	std::vector<CShaderContainer> m_vecShaderContainer;

	std::vector<CStaticGeometryMeshHolder> m_vecMesh;

	CNonLeafyAABTree<CMeshSubset> m_MeshSubsetTree;

	SFloatRGBAColor m_AmbientColor;

	SFloatRGBAColor m_FogColor;

	float m_FogStartDist;

	float m_FarClipDist;

public:

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_vecShaderContainer;
		ar & m_vecMesh; // & obb & Sphere
		ar & m_MeshSubsetTree;

		ar & m_AmbientColor;
		ar & m_FogColor;
		ar & m_FogStartDist;
		ar & m_FarClipDist;
	}
};


class CStaticGeometry : public CStaticGeometryBase
{
//	std::vector<boost::shared_ptr<CShaderManager>> m_vecpShaderManager;
//	CNonLeafyAABTree<CMeshSubset> m_MeshSubsetTree;

//	std::vector<CStaticGeometryMeshHolder> m_vecMesh;

//	std::vector<CShaderContainer> m_vecShaderContainer;

	CStaticGeometryArchive m_Archive;

public:

//	CStaticGeometry() {}
	CStaticGeometry( CStage *pStage )
		:
	CStaticGeometryBase( pStage )
	{}

	virtual int GetType() const { return CStaticGeometryBase::TYPE_GENERAL; }

	virtual bool Render( const CCamera& rCam, const unsigned int EffectFlag );

	virtual bool LoadFromFile( const std::string& filename, bool bLoadGraphicsOnly = false );

	virtual void MakeEntityTree( CEntitySet* pEntitySet )
	{
		assert( !__FUNCTION__" - Not implemented!" );
	}

	/// collision detection
	virtual int ClipTrace(STrace& tr) { return 0; }

	virtual short CheckPosition(STrace& tr) { return 0; }

	virtual char IsCurrentlyVisibleCell( short sCellIndex ) { return 0; }

	virtual void SetDynamicLightManager( CEntitySet* pEntitySet ) {}

	/// used for dynamic lighting
	virtual void GetIntersectingPolygons( Vector3& vCenter,
		                                  float fRadius,
										  AABB3& aabb,
										  std::vector<int>& veciLitPolygonIndex ) {}

	/// \param [in] dest scene
	virtual physics::CActor *CreateCollisionGeometry( physics::CScene& physics_scene );

	virtual void ReleaseGraphicsResources() {}

	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam ) {}

};


// ===================================== old header contents =====================================



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


// CSG_   Class of Static Geometry
/*
class CSG_Surface
{
public:

	// texture for surface
	string strTextureFilename;
	LPDIRECT3DTEXTURE9 pTexture;

	// texture for bump mapping
	string strNormalMapFilename;
	LPDIRECT3DTEXTURE9 pNormalMap;

	float fSpecular;

	CSG_Surface()
	{
		pTexture = NULL;
		pNormalMap = NULL;
		fSpecular = 0;
	}
};
*/


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
//#include "StaticGeometry.h"



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



#endif		/*  __SG_STATICGEOMETRY_H__  */
