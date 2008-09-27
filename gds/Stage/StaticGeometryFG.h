#ifndef  __STATICGEOMETRYFG_H__
#define  __STATICGEOMETRYFG_H__


#include <vector>
#include <string>

#include "Stage/fwd.h"
#include "Stage/StaticGeometryBase.h"
#include "Stage/StaticGeometryArchiveFG.h"

#include "3DCommon/fwd.h"
#include "3DCommon/MeshObjectHandle.h"
#include "3DCommon/TextureHandle.h"

#include "3DCommon/MeshModel/3DMeshModelArchive.h"
using namespace MeshModel;


class CTriangleMesh;


/**
 loaded from binary database file
 - "key" and archive object types stored in db file
   - "Main": CStaticGeometryArchiveFG
   - "CollisionMesh": CBSPTreeForTriangleMesh
*/
class CStaticGeometryFG : public CStaticGeometryBase
{
//	std::string m_strArchiveFilename;

	/// terrain and other static geometry
	std::vector<CMeshObjectHandle> m_vecMesh;

	std::vector<CStaticGeometryMeshGroup> m_vecMeshGroup;

	/// somewhat deprecated
	/// - skybox is now loaded as separate entity
	CD3DXMeshObjectBase *m_pSkyboxMesh;

	CShaderManager *m_pShaderManager;

	CTextureHandle m_FadeTex;

	SFloatRGBAColor m_AmbientColor;

	SFloatRGBAColor m_FogColor;

	float m_FogStartDist;
	float m_FogEndDist;
	float m_FarClipDist;

	/// global overlay texture
	CTextureHandle m_TileTexture;	/// experimental

	CTextureHandle m_NormalMapForWaterSurface;	/// experimental

	/// holds triangle mesh registered to physics simulator
	CTriangleMesh* m_pTriangleMesh;


private:

	void RenderSkybox( const CCamera& rCamera );

	void SetDepthFogParams();

	void RenderTerrainMesh( const CCamera& rCamera );

	bool LoadCollisionMesh( CStaticGeometryArchiveFG& archive );

public:

	CStaticGeometryFG( CStage *pStage );

	~CStaticGeometryFG();

	virtual int GetType() const { return TYPE_FLIGHTGAME; }

	virtual void ReleaseGraphicsResources();

	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam );

	virtual bool Render( const CCamera& rCamera, const unsigned int EffectFlag );

	// load static geometry from binary database file
	virtual bool LoadFromFile( const std::string& db_filename, bool bLoadGraphicsOnly = false );

	/// collision detection
	/// NOT IMPLEMENTED
	virtual int ClipTrace(STrace& tr);

	/// this method originally meant to return the cell index,
	/// but in static geometry for flight game, we don't have any cells,
	/// so just return 0
	virtual short CheckPosition(STrace& tr) { return 0; }

	/// in static geometry for flight game, we don't have PVS information,
	/// so always return true
	virtual char IsCurrentlyVisibleCell( short sCellIndex ) { return 1; }

	bool LoadShaderFromFile( const char *pFilename );

	void SetAmbientColor( SFloatRGBAColor& ambient_color ) { m_AmbientColor = ambient_color; }

	void SetFogColor( SFloatRGBAColor& color ) { m_FogColor = color; }

	void SetFogStartDist( float dist ) { m_FogStartDist = dist; }

	void SetFogEndDist( float dist ) { m_FogEndDist = dist; }

	void Release();
};


#endif		/*  __STATICGEOMETRYFG_H__  */
