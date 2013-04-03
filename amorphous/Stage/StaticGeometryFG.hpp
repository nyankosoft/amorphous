#ifndef  __STATICGEOMETRYFG_H__
#define  __STATICGEOMETRYFG_H__


#include "gds/Stage/fwd.hpp"
#include "gds/Stage/StaticGeometryBase.hpp"
#include "gds/Stage/StaticGeometryArchiveFG.hpp"
#include "gds/Physics/fwd.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/MeshModel/3DMeshModelArchive.hpp"


namespace amorphous
{


class CTriangleMesh;


/**
 loaded from binary database file
 - "key" and archive object types stored in db file
   - "Main": CStaticGeometryArchiveFG
   - "CollisionMesh": BSPTreeForTriangleMesh
*/
class CStaticGeometryFG : public CStaticGeometryBase
{
//	std::string m_strArchiveFilename;

	/// terrain and other static geometry
	std::vector<MeshHandle> m_vecMesh;

	std::vector<CStaticGeometryMeshGroup> m_vecMeshGroup;

	/// somewhat deprecated
	/// - skybox is now loaded as separate entity
	boost::shared_ptr<BasicMesh> m_pSkyboxMesh;

	ShaderManager *m_pShaderManager;

	TextureHandle m_FadeTex;

	SFloatRGBAColor m_AmbientColor;

	SFloatRGBAColor m_FogColor;

	float m_FogStartDist;
	float m_FogEndDist;
	float m_FarClipDist;

	/// global overlay texture
	TextureHandle m_TileTexture;	/// experimental

	TextureHandle m_NormalMapForWaterSurface;	/// experimental

	/// holds triangle mesh registered to physics simulator
	physics::CActor *m_pTriangleMesh;


private:

	void RenderSkybox( const Camera& rCamera );

	void SetDepthFogParams();

	void RenderTerrainMesh( const Camera& rCamera );

	bool LoadCollisionMesh( CStaticGeometryArchiveFG& archive );

public:

	CStaticGeometryFG( CStage *pStage );

	~CStaticGeometryFG();

	virtual int GetType() const { return TYPE_FLIGHTGAME; }

	virtual void ReleaseGraphicsResources();

	virtual void LoadGraphicsResources( const GraphicsParameters& rParam );

	virtual bool Render( const Camera& rCamera, const unsigned int EffectFlag );

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

	void SetAmbientColor( const SFloatRGBAColor& ambient_color ) { m_AmbientColor = ambient_color; }

	void SetFogColor( const SFloatRGBAColor& color ) { m_FogColor = color; }

	void SetFogStartDist( float dist ) { m_FogStartDist = dist; }

	void SetFogEndDist( float dist ) { m_FogEndDist = dist; }

	void Release();
};

} // namespace amorphous



#endif		/*  __STATICGEOMETRYFG_H__  */