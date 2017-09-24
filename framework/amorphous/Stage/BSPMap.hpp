#ifndef __STATICGEOMETRY_BSPMAP_H__
#define __STATICGEOMETRY_BSPMAP_H__


#include "amorphous/Stage/fwd.hpp"
#include "amorphous/Stage/StaticGeometryBase.hpp"
#include "amorphous/Stage/BSPStaticGeometry.hpp"
#include "amorphous/Stage/bspstructs.hpp"
#include "amorphous/Stage/bsptree.hpp"

#include "BSPMap_Collision/BSPTree_CollisionModel.hpp"

#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"

#include "amorphous/Support/FixedVector.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Support/macro.h"


namespace amorphous
{

#define NUM_MAX_LIGHTSOURCEPOLYGONS 512
#define NUM_MAX_NONLIGHTSOURCEPOLYGONS 3200

#define NUM_MAX_POLYGONS_PER_TEXTURE 1024


class EntityManager;
class CTriangleMesh;
class CDynamicLightManagerForStaticGeometry;


class CBSPMap : public CStaticGeometryBase
{

public:

	enum eStaticGeometryParam
	{
		NUM_MAX_SURFACES = 64,
		NUM_MAX_TEXTURES = 256,
		NUM_MAX_LIGHTMAPTEXTURES = 64,
	};

private:

	/// collision detection
	BSPTree m_BSPTree;

	CBSPTree_CollisionModel m_BSPCollisionModel;

    /// triangulated mesh for collision (borrowed reference) (experimental)
	CTriangleMesh *m_pTriangleMesh;


	int m_iNumPolygons;
	CSG_Polygon* m_paPolygon;
	SFixedConvexModel* m_paFCModel;

	int m_iNumTriangleSets;
	CSG_TriangleSet *m_paTriangleSet;

	int m_iNumCells;
	CSG_Cell* m_paCellData;

	short* m_pasVisibleCellIndex;
	char* m_pacCurrentlyVisibleCell;	// used to store cell visibility at runtime

	SFog m_Fog;

	LPDIRECT3DVERTEXDECLARATION9 m_pStaticGeometryVertexDecleration;

	/// buffer to hold vertices
	int m_iNumVertices;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;

	/// buffer to hold indices
	int m_iNumIndices;
	LPDIRECT3DINDEXBUFFER9  m_pIB;

	/// used to point the vertex array in the VB. set to null when VB is not locked
	MAPVERTEX *m_pVertex;

	TCFixedVector<CSG_TextureCache, NUM_MAX_TEXTURES> m_vecTextureCache;

	/// holds surface property (texture, normal map texture, specular, etc.)
	TCFixedVector<CSG_Surface, NUM_MAX_SURFACES> m_vecSurface;

	/// holds lightmap textures for static polygons
	TCFixedVector<CSG_Lightmap, NUM_MAX_LIGHTMAPTEXTURES> m_vecLightmap;

	/// the following buffers are used to sort polygons by texture and are updated every frame
	int m_aiPolygonSortedByTexture[NUM_MAX_TEXTURES][NUM_MAX_POLYGONS_PER_TEXTURE];
	int m_aiNumPolygonsSortedByTexture[NUM_MAX_TEXTURES];
	int m_iNumPolygonsWithNoTexture;
	int m_aiPolygonWithNoTexture[NUM_MAX_POLYGONS_PER_TEXTURE];


	/// experiment on glare image
//	int m_iNumLightSourcePolygons;
//	int m_aiLightSourcePolygon[NUM_MAX_LIGHTSOURCEPOLYGONS];	// must be updated every frame
//	int m_iNumNonLightSourcePolygons;
//	int m_aiNonLightSourcePolygon[NUM_MAX_NONLIGHTSOURCEPOLYGONS];	// must be updated every frame

	enum eExtraTextureType
	{
		EXTRA_TEX_TRANSPARENT,
		EXTRA_TEX_TRANSLUCENT,
		EXTRA_TEX_SPECULAR,
		EXTRA_TEX_DEFAULT_COLOR,
		EXTRA_TEX_DEFAULT_NORMALMAP,
		NUM_EXTRA_TEXTURES
	};

	enum ShaderTechniqueType
	{
		ST_BUMP,
		ST_NO_BUMP,
		NUM_SHADER_TECHNIQUE_TYPES
	};

	LPDIRECT3DTEXTURE9 m_apExtraTexture[NUM_EXTRA_TEXTURES];

	CDynamicLightManagerForStaticGeometry *m_pDynamicLightManager;
	LPDIRECT3DTEXTURE9 m_pLightmap_PL;	// lightmap texture for point light
	LPDIRECT3DTEXTURE9 m_pNormalmap_PL;	// normal map texture for point light

	ShaderTechniqueHandle m_aShaderTechHandle[NUM_SHADER_TECHNIQUE_TYPES];

	ShaderHandle m_Shader;

private:

	/// renders skybox
	void RenderSkybox( const Camera& rCamera );

	void LoadTextures();

	void DrawCell( short sCellIndex, Camera& rCam );

	void DrawCell_TS( short sCellIndex, const Camera& rCam );

	void DrawTextureSortedPolygons();

	void DrawTextureSortedPolygons_Shader();

	void DrawTextureSortedPolygons_Shader_TS();

	void ClipTraceToInteriorModel_r(STrace& tr, short nodeindex);

public:

	CBSPMap( CStage *pStage );

	~CBSPMap();

	virtual int GetType() const { return TYPE_BSPMAP; }

	bool Render( const Camera& rCam, const unsigned int EffectFlag );

	bool LoadFromFile( const std::string& filename, bool bLoadGraphicsOnly = false );

	void WriteToFile( const char *pFilename );

	void MakeEntityTree( BSPTree& beptree );

	/// collision detection
	int ClipTrace(STrace& tr);

	// returns the index of the cell('sCellIndex' member of the 'SNode_f' structure)
	// if the input position is a valid area
	// returns CONTENTS_SOLID if the position is in a invalid(solid) area
	short CheckPosition(STrace& tr);

	inline char IsCurrentlyVisibleCell( short sCellIndex ) { return m_pacCurrentlyVisibleCell[sCellIndex]; }

//	void RenderGlareImage();

	void SetDynamicLightManager( EntityManager* pEntitySet );

	inline void GetIntersectingPolygons( Vector3& vCenter, float fRadius, AABB3& aabb,
		                                 vector<int>& veciLitPolygonIndex )
	{	// used for dynamic lighting
		m_BSPCollisionModel.CheckCollision( vCenter, fRadius, aabb, veciLitPolygonIndex );
	}


	// The following three (five?) functions are used for dynamic light mapping

	// allows access to each map vertex
	// LockVertexBuffer() has to be called before calling this function?
	inline MAPVERTEX *GetVertex();

	inline bool LockVertexBuffer();

	inline void UnlockVertexBuffer();

	inline CSG_Polygon *GetPolygon() { return m_paPolygon; }
	inline CSG_Polygon *GetPolygon(int iIndex) { return &m_paPolygon[iIndex]; }


	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const GraphicsParameters& rParam );

};


//================================== inline implementations ==================================

inline MAPVERTEX *CBSPMap::GetVertex()
{
	if( !m_pVertex )
		LockVertexBuffer();

	return m_pVertex;
}


inline bool CBSPMap::LockVertexBuffer()
{
	if( !m_pVertex )
	{
		VOID* pVBData;
		if( FAILED( m_pVB->Lock( 0, 0, (VOID**)&pVBData, 0 ) ) )
		{
			ONCE( LOG_PRINT_WARNING( " Failed to lock the vertex buffer." ) );
			return false;
		}
		m_pVertex = (MAPVERTEX *)pVBData;
	}
	return true;
}


inline void CBSPMap::UnlockVertexBuffer()
{
	if( m_pVertex )
	{
		m_pVB->Unlock();
		m_pVertex = NULL;
	}
}


} // namespace amorphous



#endif  /*  __STATICGEOMETRY_BSPMAP_H__  */
