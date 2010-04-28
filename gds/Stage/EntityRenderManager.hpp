#ifndef  __EntityRenderManager_H__
#define  __EntityRenderManager_H__


#include "../base.hpp"
#include "3DMath/AABB3.hpp"
#include "fwd.hpp"
#include "CopyEntity.hpp"
#include "EntityNode.hpp"
#include "ScreenEffectManager.hpp"

#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/CubeMapManager.hpp"
#include "Graphics/ShaderHandle.hpp"


class CShadowMapManager;
class CScreenEffectManager;

class CEntityEnvMapRenderTask;
class CEntitySceneRenderTask;
class CCubeTextureParamsLoader;
class CEntityShadowMapRenderer;


class CEnvMapTarget
{
public:

	CEnvMapTarget() : m_CubeMapTextureSize(128), m_EntityID(0) {}

public:

	int m_CubeMapTextureSize; ///< edge length of the cube texture

	CCopyEntity *m_pEntity;

	U32 m_EntityID; ///< target of the env map

	CTextureHandle m_Texture; ///< stores env map texture

	boost::shared_ptr<CCubeTextureParamsLoader> m_pCubeMapTextureLoader;
};


/**
 *
 *
 */
//class CEntityRenderManager : public CGraphicsComponent, public CCubeMapSceneRenderer
class CEntityRenderManager : public CScreenEffectTargetSceneRenderer, public CCubeMapSceneRenderer
{
private:

	enum params
	{
		SIZE_ZSORTTABLE	= 256,
		NUM_MAX_ENVMAP_TARGETS = 8,
	};

	CEntitySet *m_pEntitySet;

	int m_NumEntityNodes;
	CEntityNode* m_paEntityTree;	///< pointer to the first node of a binary tree that holds copy-entities


	char* m_pacRendered;

	/// entities that have transparent parts are z-sorted
	CCopyEntity* m_apZSortTable[SIZE_ZSORTTABLE];

	/// world pose of camera
	/// - updated at the start of Render()
	/// - used for z-sort
	Matrix34 m_CameraPose;

	CCamera *m_pCurrentCamera;
	
	/// used for z-sort
	float m_fCameraFarClipDist;

	std::vector<CBaseEntity *> m_vecpSweepRenderBaseEntity;

	CTextureHandle m_TransparentTexture;
	CTextureHandle m_TranslucentTexture;

	std::string m_FallbackShaderFilepath;
	CShaderHandle m_FallbackShader;

	//
	// shadow
	//

	CShadowMapManager *m_pShadowManager;

	boost::shared_ptr<CEntityShadowMapRenderer> m_pShadowMapSceneRenderer;

	bool m_bOverrideShadowMapLight;

	Vector3 m_vOverrideShadowMapPosition;
	Vector3 m_vOverrideShadowMapDirection;

	std::vector< CEntityHandle<> > m_vecLightForShadow;

	int m_NumMaxLightsForShadow;

	std::vector<CCopyEntity *> m_vecpEntityBuffer;


	//
	// environment mapping
	//

	/// When on, environment map texture is updated whenever the camera moves
	bool m_bEnableEnvironmentMap;

	CCubeMapManager *m_pCubeMapManager;

	// stores enitities which need env map texture
	std::vector<CEnvMapTarget> m_vecEnvMapTarget;

	U32 m_CurrentEnvMapTargetEntityID;

private:

	void RenderEntityNodeUp_r( short sEntNodeIndex, CCamera& rCam );

	void RenderDownward_r( short sEntNodeIndex, CCamera& rCam );

	void RenderShadowCastersDownward_r( short sEntNodeIndex, CCamera& rCam );

	void RenderShadowReceiversDownward_r( short sEntNodeIndex, CCamera& rCam );

	void RenderAllButEnvMapTargetDownward_r( short sEntNodeIndex, CCamera& rCam, U32 target_entity_id );

	void LoadTextures();

	void MoveSkyboxToListHead();

	/// render z-sorted entities
	void RenderZSortTable();

	void RenderSceneWithShadowMap( CCamera& rCam);

	void RenderScene( CCamera& rCam );

	void RenderShadowCasters( CCamera& rCam );

	void RenderShadowReceivers( CCamera& rCam );

	void UpdateLightsForShadow();

	void RenderAllButEnvMapTarget( CCamera& rCam, U32 target_entity_id );

	virtual void RenderSceneToCubeMap( CCamera& camera );

	virtual Vector3 GetCameraPosition() { return m_CameraPose.vPosition; }

	void UpdateEnvironmentMapTargets();

	/// render scene to cube map texture(s)
	/// cube map textures are sotred and later retrieved by entity which uses
	/// envmap to render itself
	void UpdateEnvironmentMapTextures();


	void CreateEnvMapRenderTasks();

	void CreateShadowMapRenderTasks( CCamera& rCam );

	void CreateSceneRenderTask( CCamera& rCam );

	static std::string ms_DefaultFallbackShaderFilename;

public:

	CEntityRenderManager( CEntitySet* pEntitySet );

	/// added default dtor to use the class with boost::python
	CEntityRenderManager() {}

	~CEntityRenderManager();

	bool LoadFallbackShader();

	const CShaderHandle& GetFallbackShader() { return m_FallbackShader; }

    void UpdateEntityTree( CEntityNode* pRootNode, int num_nodes );

	void AddSweepRenderEntity( CBaseEntity* pBaseEntity );

	/// put an entity that have transparent parts to z-sort list
	void SendToZSortTable(CCopyEntity* pCopyEnt);

	void Render( CCamera& rCam );

	void CreateRenderTasks( bool create_scene_render_task );

	/// implementation of CScreenEffectTargetSceneRenderer::RenderSceneForScreenEffect()
	virtual void RenderSceneForScreenEffect( CCamera& rCamera ) { Render( rCamera ); }

	inline void UpdateCamera( CCamera* pCam );

	//
	// environemnt mapping
	//

	void EnableEnvironmentMap() { m_bEnableEnvironmentMap = true; }
	void DisableEnvironmentMap() { m_bEnableEnvironmentMap = false; }

//	CTextureHandle GetEnvMapTexture( U32 entity_id );
	LPDIRECT3DCUBETEXTURE9 GetEnvMapTexture( U32 entity_id );

	bool AddEnvMapTarget( CCopyEntity *pEntity );

	void SaveEnvMapTextureToFile( const std::string& output_image_filename );

	bool RemoveEnvMapTarget( CCopyEntity *pEntity );


	//
	// shadow
	//

	CShadowMapManager *GetShadowManager() { return m_pShadowManager; }

	bool EnableShadowMap( int shadow_map_size = 512 );
	bool EnableSoftShadow( float softness = 1.0f, int shadowmap_size = 512 );
	void DisableShadowMap();

	/// Specify the light for which shadow is rendered
	void SetLightForShadow( const std::string& light_entity_name );

	void SetOverrideShadowMapLight( bool override ) { m_bOverrideShadowMapLight = override; } 
	void SetOverrideShadowMapLightPosition( const Vector3& pos ) { m_vOverrideShadowMapPosition = pos; }
	void SetOverrideShadowMapLightDirection( const Vector3& dir ) { m_vOverrideShadowMapDirection = dir; }

	void RenderForShadowMaps( CCamera& rCam );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	friend class CEntityEnvMapRenderTask;
	friend class CEntitySceneRenderTask;
	friend class CEntityShadowMapRenderer;
};


// ================================ inline implementations ================================ 

inline void CEntityRenderManager::UpdateCamera( CCamera* pCam )
{
	m_pCurrentCamera = pCam;
	pCam->GetPose( m_CameraPose );
}


#endif		/*  __EntityRenderManager_H__  */
