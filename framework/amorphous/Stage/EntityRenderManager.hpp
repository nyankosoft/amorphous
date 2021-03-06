#ifndef  __EntityRenderManager_H__
#define  __EntityRenderManager_H__


#include "amorphous/base.hpp"
#include "amorphous/3DMath/AABB3.hpp"
#include "fwd.hpp"
#include "CopyEntity.hpp"
#include "EntityNode.hpp"
#include "ScreenEffectManager.hpp"

#include "amorphous/Graphics/GraphicsComponentCollector.hpp"
#include "amorphous/Graphics/CubeMapManager.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"


namespace amorphous
{

class CEntityEnvMapRenderTask;
class CEntitySceneRenderTask;
class CubeTextureParamsLoader;
class CEntityShadowMapRenderer;
class PlanarReflectionGroup;


class EnvMapTarget
{
public:

	EnvMapTarget() : m_CubeMapTextureSize(128), m_EntityID(0) {}

public:

	int m_CubeMapTextureSize; ///< edge length of the cube texture

	CCopyEntity *m_pEntity;

	U32 m_EntityID; ///< target of the env map

	TextureHandle m_Texture; ///< stores env map texture

	std::shared_ptr<CubeTextureParamsLoader> m_pCubeMapTextureLoader;
};


/**
 *
 *
 */
//class EntityRenderManager : public GraphicsComponent, public CCubeMapSceneRenderer
class EntityRenderManager : public CScreenEffectTargetSceneRenderer, public CCubeMapSceneRenderer
{
private:

	enum params
	{
		SIZE_ZSORTTABLE	= 256,
		NUM_MAX_ENVMAP_TARGETS = 8,
	};

	EntityManager *m_pEntitySet;

	int m_NumEntityNodes;
	EntityNode* m_paEntityTree;	///< pointer to the first node of a binary tree that holds copy-entities


	std::vector<char> m_EntityNodeRendered;

	/// entities that have transparent parts are z-sorted
	CCopyEntity* m_apZSortTable[SIZE_ZSORTTABLE];

	/// world pose of camera
	/// - updated at the start of Render()
	/// - used for z-sort
	Matrix34 m_CameraPose;

	Camera *m_pCurrentCamera;
	
	/// used for z-sort
	float m_fCameraFarClipDist;

	std::vector<BaseEntity *> m_vecpSweepRenderBaseEntity;

	std::string m_FallbackShaderFilepath;
	ShaderHandle m_FallbackShader;

	//
	// shadow
	//

	ShadowMapManager *m_pShadowManager;

	std::shared_ptr<CEntityShadowMapRenderer> m_pShadowMapSceneRenderer;

	bool m_bOverrideShadowMapLight;

	Vector3 m_vOverrideShadowMapPosition;
	Vector3 m_vOverrideShadowMapDirection;

	std::vector< EntityHandle<> > m_vecLightForShadow;

	int m_NumMaxLightsForShadow;

	std::vector<CCopyEntity *> m_vecpEntityBuffer;


	//
	// environment mapping
	//

	/// When on, environment map texture is updated whenever the camera moves
	bool m_bEnableEnvironmentMap;

	CubeMapManager *m_pCubeMapManager;

	// stores enitities which need env map texture
	std::vector<EnvMapTarget> m_vecEnvMapTarget;

	U32 m_CurrentEnvMapTargetEntityID;

	// planar reflection

	int m_CurrentlyRenderedPlanarReflectionSceneID;

	bool m_IsRenderingMirroredScene;

private:

	void RenderEntityNodeUp_r( short sEntNodeIndex, Camera& rCam );

	void MoveSkyboxToListHead();

	/// render z-sorted entities
	void RenderZSortTable();

	void ClearZSortTable();

	void RenderScene( Camera& rCam );

	void RenderShadowCasters( Camera& rCam );

	void RenderShadowReceivers( Camera& rCam );

	void UpdateLightsForShadow();

	void RenderAllButEnvMapTarget( Camera& rCam, U32 target_entity_id );

	virtual void RenderSceneToCubeMap( Camera& camera );

	virtual Vector3 GetCameraPosition() { return m_CameraPose.vPosition; }

	void UpdateEnvironmentMapTargets();

	/// render scene to cube map texture(s)
	/// cube map textures are sotred and later retrieved by entity which uses
	/// envmap to render itself
	void UpdateEnvironmentMapTextures();

	void UpdateFogParams( const Camera& rCam );

	void CreateEnvMapRenderTasks();

	void CreateShadowMapRenderTasks( Camera& rCam );

	void CreateSceneRenderTask( Camera& rCam );

	static std::string ms_DefaultFallbackShaderFilename;

public:

	EntityRenderManager( EntityManager* pEntitySet );

	/// added default dtor to use the class with boost::python
	EntityRenderManager() {}

	~EntityRenderManager();

	bool LoadFallbackShader();

	const ShaderHandle& GetFallbackShader() { return m_FallbackShader; }

    void UpdateEntityTree( EntityNode* pRootNode, int num_nodes );

	void AddSweepRenderEntity( BaseEntity* pBaseEntity );

	/// put an entity that have transparent parts to z-sort list
	void SendToZSortTable(CCopyEntity* pCopyEnt);

	void Render( Camera& rCam );

	void CreateRenderTasks( bool create_scene_render_task );

	/// implementation of CScreenEffectTargetSceneRenderer::RenderSceneForScreenEffect()
	virtual void RenderSceneForScreenEffect( Camera& rCamera ) { Render( rCamera ); }

	inline void UpdateCamera( Camera* pCam );

	//
	// environemnt mapping
	//

	void EnableEnvironmentMap() { m_bEnableEnvironmentMap = true; }
	void DisableEnvironmentMap() { m_bEnableEnvironmentMap = false; }

	TextureHandle GetEnvMapTexture( U32 entity_id );

	bool AddEnvMapTarget( CCopyEntity *pEntity );

	void SaveEnvMapTextureToFile( const std::string& output_image_filename );

	bool RemoveEnvMapTarget( CCopyEntity *pEntity );


	//
	// shadow
	//

	ShadowMapManager *GetShadowManager() { return m_pShadowManager; }

	bool EnableShadowMap( int shadow_map_size = 512 );
	bool EnableSoftShadow( float softness = 1.0f, int shadowmap_size = 512 );
	void DisableShadowMap();

	/// Specify the light for which shadow is rendered
	void SetLightForShadow( const std::string& light_entity_name );

	void SetOverrideShadowMapLight( bool override ) { m_bOverrideShadowMapLight = override; } 
	void SetOverrideShadowMapLightPosition( const Vector3& pos ) { m_vOverrideShadowMapPosition = pos; }
	void SetOverrideShadowMapLightDirection( const Vector3& dir ) { m_vOverrideShadowMapDirection = dir; }

	void RenderSceneWithShadows( Camera& rCam );

	Result::Name AddPlanarReflector( EntityHandle<>& entity, const SPlane& plane = Plane(Vector3(0,0,0),0) );
	Result::Name RemovePlanarReflector( EntityHandle<>& entity, bool remove_planar_refelection_group );
	void UpdatePlanarReflectionTexture( Camera& rCam, PlanarReflectionGroup& group );
	void UpdatePlanarReflectionTextures( Camera& rCam );
	TextureHandle GetPlanarReflectionTexture( CCopyEntity& entity );
	int GetCurrentlyRenderedPlanarReflectionSceneID() const;

	void RenderPlanarReflectionSurfaces();
	void RenderMirroredScene();
	bool IsRenderingMirroredScene() const { return m_IsRenderingMirroredScene; }

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const GraphicsParameters& rParam );

	friend class CEntityEnvMapRenderTask;
	friend class CEntitySceneRenderTask;
	friend class CEntityShadowMapRenderer;
};


// ================================ inline implementations ================================ 

inline void EntityRenderManager::UpdateCamera( Camera* pCam )
{
	m_pCurrentCamera = pCam;
	pCam->GetPose( m_CameraPose );
}

} // namespace amorphous



#endif		/*  __EntityRenderManager_H__  */
