#ifndef __STATICGEOMETRYBASE_H__
#define __STATICGEOMETRYBASE_H__


#include "fwd.h"
#include "3DCommon/fwd.h"
#include "3DCommon/GraphicsComponentCollector.h"
#include "3DMath/Vector3.h"
#include "3DMath/AABB3.h"

#include <vector>
#include <string>


class CStaticGeometryBase : public CGraphicsComponent
{

public:

//	enum eBaseStaticGeometryParam	{	};

protected:

	std::string m_strFilename;

	/// axis-aligned bounding box that contains the entire geometry
	AABB3 m_AABB;

	/// stage object that uses this static geometry
	CStage* m_pStage;

	// collision detection
/*	CBSPTree m_BSPTree;		//2 pointers, 'm_paNode' and 'm_paPlane' is connected to 'm_BSPTree', which itself doesn't do any memory allocation such as new/delete

	int m_iNumTriangleSets;
	CSG_TriangleSet *m_paTriangleSet;

	int m_iNumCells;
	CSG_Cell* m_paCellData;

	short* m_pasVisibleCellIndex;
	char* m_pacCurrentlyVisibleCell;	// used to store cell visibility at runtime


	enum eExtraTextureType
	{
		EXTRA_TEX_TRANSPARENT,
		EXTRA_TEX_TRANSLUCENT,
		EXTRA_TEX_SPECULAR,
		EXTRA_TEX_DEFAULT_COLOR,
		EXTRA_TEX_DEFAULT_NORMALMAP,
		NUM_EXTRA_TEXTURES
	};

	LPDIRECT3DTEXTURE9 m_apExtraTexture[NUM_EXTRA_TEXTURES];

	CDynamicLightManagerForStaticGeometry *m_pDynamicLightManager;
	LPDIRECT3DTEXTURE9 m_pLightmap_PL;	// lightmap texture for point light
	LPDIRECT3DTEXTURE9 m_pNormalmap_PL;	// normal map texture for point light
*/

public:

	CStaticGeometryBase( CStage *pStage ) : m_pStage(pStage) { m_AABB.Nullify(); }

	virtual ~CStaticGeometryBase() {}

	virtual int GetType() const = 0;

	const AABB3& GetAABB() const { return m_AABB; }

	virtual bool Render( const CCamera& rCam, const unsigned int EffectFlag ) = 0;

	virtual bool LoadFromFile( const std::string& filename, bool bLoadGraphicsOnly = false ) = 0;

	virtual void WriteToFile( const char *pFilename ) {}

	virtual void MakeEntityTree( CBSPTree& bsptree );

	/// collision detection
	virtual int ClipTrace(STrace& tr) = 0;

	virtual short CheckPosition(STrace& tr) = 0;

	virtual char IsCurrentlyVisibleCell( short sCellIndex ) = 0;

	virtual void SetDynamicLightManager( CEntitySet* pEntitySet ) {}

	/// used for dynamic lighting
	virtual void GetIntersectingPolygons( Vector3& vCenter,
		                                  float fRadius,
										  AABB3& aabb,
										  std::vector<int>& veciLitPolygonIndex ) {}

	virtual void ReleaseGraphicsResources() {}
	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam ) {}


	enum type
	{
		TYPE_BSPMAP,
		TYPE_FLIGHTGAME,
		TYPE_GENERAL,
		NUM_TYPES
	};

	// The following three (five?) functions are used for dynamic light mapping

	// allows access to each map vertex
	// LockVertexBuffer() has to be called before calling this function?
/*	inline MAPVERTEX *GetVertex();

	inline bool LockVertexBuffer();

	inline void UnlockVertexBuffer();

	inline CSG_Polygon *GetPolygon() {	return m_paPolygon; }
	inline CSG_Polygon *GetPolygon(int iIndex) { return &m_paPolygon[iIndex]; }
*/

};


#endif  /*  __STATICGEOMETRYBASE_H__  */