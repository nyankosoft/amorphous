#include "EntityRenderManager.hpp"
#include "EntitySet.hpp"
#include "LightEntity.hpp"
#include "trace.hpp"
#include "ViewFrustumTest.hpp"

#include "Graphics/ShadowMapManager.hpp"
#include "Graphics/VarianceShadowMapManager.hpp"
#include "Graphics/CubeMapManager.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderManagerHub.hpp"
#include "Graphics/Shader/GenericShaderGenerator.hpp"

#include "Graphics/RenderTask.hpp"
#include "Graphics/RenderTaskProcessor.hpp"

#include "Support/memory_helpers.hpp"
#include "Support/Profile.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Vec3_StringAux.hpp"
#include "Support/Macro.h"
#include "Support/ParamLoader.hpp"

#include "ScreenEffectManager.hpp"


using namespace boost;


class CCubeTextureParamsLoader : public CShaderParamsLoader
{
	int m_CubeTexIndex;
//	LPDIRECT3DCUBETEXTURE9 m_pCubeTexture;

	CEntityHandle<> m_Entity; ///< envmap target

	CEntityRenderManager *m_pEntityRenderManager;

public:

	CCubeTextureParamsLoader( boost::shared_ptr<CCopyEntity> pEntity = boost::shared_ptr<CCopyEntity>(),
		                      CEntityRenderManager *pEntityRenderMgr = NULL,
							  int cube_tex_index = 0 )
		:
	m_Entity(pEntity),
	m_pEntityRenderManager(pEntityRenderMgr),
	m_CubeTexIndex(cube_tex_index)
//	m_pCubeTexture(pCubeTexture)
	{}

/*	void SetCubeTexture( int cube_tex_index, LPDIRECT3DCUBETEXTURE9 pCubeTexture )
	{
		m_CubeTexIndex = cube_tex_index;
		m_pCubeTexture = pCubeTexture;
	}*/

	void UpdateShaderParams( CShaderManager& rShaderMgr )
	{
//		rShaderMgr.SetCubeTexture( m_CubeTexIndex, m_pCubeTexture );
		boost::shared_ptr<CCopyEntity> pEntity = m_Entity.Get();
		if( pEntity && m_pEntityRenderManager )
		{
			LPDIRECT3DCUBETEXTURE9 pCubeTex
				= m_pEntityRenderManager->GetEnvMapTexture(pEntity->GetID());

			rShaderMgr.SetCubeTexture( m_CubeTexIndex, pCubeTex );
		}
	}
};


class CEntityShadowMapRenderer : public CShadowMapSceneRenderer
{
	CEntityRenderManager *m_pRenderer;

public:

	CEntityShadowMapRenderer(CEntityRenderManager *pRenderer)
		:
	m_pRenderer(pRenderer)
	{}

	void RenderSceneToShadowMap( CCamera& camera );

	void RenderShadowReceivers( CCamera& camera );
};


void CEntityShadowMapRenderer::RenderSceneToShadowMap( CCamera& camera )
{
	m_pRenderer->RenderShadowCasters( camera );
}


void CEntityShadowMapRenderer::RenderShadowReceivers( CCamera& camera )
{
	m_pRenderer->RenderShadowReceivers( camera );
}


class CEntityEnvMapRenderTask : public CRenderTask
{
	CEntityRenderManager *m_pRenderManager;

public:

	CEntityEnvMapRenderTask( CEntityRenderManager *pRenderMgr )
		:
	m_pRenderManager(pRenderMgr)
	{
		m_TypeFlags |= DO_NOT_CALL_BEGINSCENE_AND_ENDSCENE;
	}

	virtual void Render()
	{
		m_pRenderManager->UpdateEnvironmentMapTextures();
	}
};


class CEntityShadowMapRenderTask : public CRenderTask
{
	CEntityRenderManager *m_pRenderManager;

	CCamera *m_pCamera;

//	CScreenEffectManager *m_pScreenEffectManager;

public:

	CEntityShadowMapRenderTask( CEntityRenderManager *pRenderMgr,
		CCamera *pCam,
		CScreenEffectManager *pScreenEffectManager)
		:
	m_pRenderManager(pRenderMgr),
	m_pCamera(pCam)//,
//	m_pScreenEffectManager(pScreenEffectManager)
	{
		m_TypeFlags |= DO_NOT_CALL_BEGINSCENE_AND_ENDSCENE;
	}

	virtual void Render()
	{
		ShaderManagerHub.PushViewAndProjectionMatrices( *m_pCamera );

		// - creates shadow map
		// - creates scene depth map
		// - creates scene texture
		// each of the three has BeginScene() and EndScene() pair inside
		m_pRenderManager->RenderForShadowMaps( *m_pCamera/*, m_pScreenEffectManager*/ );

		ShaderManagerHub.PopViewAndProjectionMatrices();
	}
};



class CEntitySceneRenderTask : public CRenderTask
{
	CEntityRenderManager *m_pRenderManager;

	CCamera *m_pCamera;

public:

	CEntitySceneRenderTask( CEntityRenderManager *pRenderMgr, CCamera *pCam )
		:
	m_pRenderManager(pRenderMgr),
	m_pCamera(pCam)
	{
	}

	virtual ~CEntitySceneRenderTask() {}

	void Render()
	{
		m_pRenderManager->Render( *m_pCamera );
	}
};


string CEntityRenderManager::ms_DefaultFallbackShaderFilename = "Shader\\Default.fx";


CEntityRenderManager::CEntityRenderManager( CEntitySet* pEntitySet )
:
m_pEntitySet(pEntitySet),
m_pCubeMapManager(NULL),
m_pShadowManager(NULL),
m_pCurrentCamera(NULL)
{
	m_pacRendered = NULL;

	// clear z-sort table
	for(int i=0; i<SIZE_ZSORTTABLE; i++)
	{
		m_apZSortTable[i] = NULL;
	}

	// load texture for glare effect
	LoadTextures();

	m_fCameraFarClipDist = 1000.0f;


	m_NumMaxLightsForShadow = 1;

	// environment mapping - turned off by default
	m_bEnableEnvironmentMap = false;

	m_CurrentEnvMapTargetEntityID = 0;

	// shadow-related settings
	
	m_bOverrideShadowMapLight = false;

	m_vOverrideShadowMapPosition = Vector3(0,0,0);
	m_vOverrideShadowMapDirection = Vector3(0,-1,0);

	m_pShadowMapSceneRenderer = shared_ptr<CEntityShadowMapRenderer>( new CEntityShadowMapRenderer(this) );

	LoadFallbackShader();
}


CEntityRenderManager::~CEntityRenderManager()
{
//	ReleaseGraphicsResources();

	m_vecpSweepRenderBaseEntity.resize( 0 );

	SafeDeleteArray( m_pacRendered );

	SafeDelete( m_pShadowManager );

	SafeDelete( m_pCubeMapManager );
}


bool CEntityRenderManager::LoadFallbackShader()
{
//	m_FallbackShaderFilepath = ms_DefaultFallbackShaderFilename;
//	if( !m_FallbackShader.Load( m_FallbackShaderFilepath ) )
//	{
//		return false;
//	}

	CShaderResourceDesc desc;
	CGenericShaderDesc shader_desc;
	shader_desc.Specular = CSpecularSource::UNIFORM;
	desc.pShaderGenerator.reset( new CGenericShaderGenerator(shader_desc) );
	m_FallbackShader.Load( desc );

	// check if the shader file has been properly loaded
	LPD3DXEFFECT pEffect = m_FallbackShader.GetShaderManager()->GetEffect();
	if( !pEffect )
	{
		return false;
	}

	// CLightEntityManager needs global shader in its CLightEntityManager::InitShaderLightManager()
	CShader::Get()->SetShaderManager( m_FallbackShader.GetShaderManager() );

	return true;
}


void CEntityRenderManager::UpdateEntityTree( CEntityNode* pRootNode, int num_nodes )
{
	m_paEntityTree = pRootNode;
	m_NumEntityNodes = num_nodes;

	SafeDeleteArray( m_pacRendered );
	m_pacRendered = new char [ m_NumEntityNodes ];
	memset( m_pacRendered, 0, m_NumEntityNodes );
}


void CEntityRenderManager::AddSweepRenderEntity( CBaseEntity* pBaseEntity )
{
	m_vecpSweepRenderBaseEntity.push_back( pBaseEntity );
}


void CEntityRenderManager::RenderEntityNodeUp_r( short sEntNodeIndex, CCamera& rCam )
{
	CEntityNode* pFirstEntNode = m_paEntityTree;
	CEntityNode* pEntNode = pFirstEntNode + sEntNodeIndex;

	pEntNode->Render(rCam);
	m_pacRendered[sEntNodeIndex] = 1;	// mark this entity node as an already rendered one

	if( pEntNode->sParent != -1 && m_pacRendered[ pEntNode->sParent ] != 1)
		RenderEntityNodeUp_r( pEntNode->sParent, rCam);
}


void CEntityRenderManager::RenderDownward_r( short sEntNodeIndex, CCamera& rCam )
{
	CEntityNode& rThisEntNode = m_paEntityTree[sEntNodeIndex];

	if( !rCam.ViewFrustumIntersectsWith( rThisEntNode.m_AABB ) )
		return;

	rThisEntNode.Render( rCam );

	if( rThisEntNode.leaf )
		return;

	RenderDownward_r( rThisEntNode.sFrontChild, rCam );
	RenderDownward_r( rThisEntNode.sBackChild,  rCam );

}


void CEntityRenderManager::RenderShadowCastersDownward_r( short sEntNodeIndex, CCamera& rCam )
{
	CEntityNode& rThisEntNode = m_paEntityTree[sEntNodeIndex];

	// TODO: proper culing of shadow caster objects
//	if( !rCam.ViewFrustumIntersectsWith( rThisEntNode.m_AABB ) )
//		return;

	rThisEntNode.RenderShadowCasters( rCam );

	if( rThisEntNode.leaf )
		return;

	RenderShadowCastersDownward_r( rThisEntNode.sFrontChild, rCam );
	RenderShadowCastersDownward_r( rThisEntNode.sBackChild,  rCam );

}


void CEntityRenderManager::RenderShadowReceiversDownward_r( short sEntNodeIndex, CCamera& rCam )
{
	CEntityNode& rThisEntNode = m_paEntityTree[sEntNodeIndex];

	if( !rCam.ViewFrustumIntersectsWith( rThisEntNode.m_AABB ) )
		return;

	rThisEntNode.RenderShadowReceivers( rCam );

	if( rThisEntNode.leaf )
		return;

	RenderShadowReceiversDownward_r( rThisEntNode.sFrontChild, rCam );
	RenderShadowReceiversDownward_r( rThisEntNode.sBackChild,  rCam );

}


/**
 * put the skybox to the head of the root entity node
 * - assumes that there is only one skybox entity
 */
void CEntityRenderManager::MoveSkyboxToListHead()
{
	CEntityNode& rRootNode = m_paEntityTree[0];

	CCopyEntity *pEntity = NULL, *pPrevEntity;
	CCopyEntity *pSkyboxEntity = NULL;
	CLinkNode<CCopyEntity> *pLinkNode;
	for( pLinkNode = rRootNode.m_EntityLinkHead.pNext, pPrevEntity = NULL;
		 pLinkNode;
		 pLinkNode = pLinkNode->pNext, pPrevEntity = pEntity )
	{
		pEntity = pLinkNode->pOwner;

		if( pEntity->pBaseEntity->GetArchiveObjectID() == CBaseEntity::BE_SKYBOX )
		{
			if( pEntity == rRootNode.m_EntityLinkHead.pNext->pOwner )
				break;	// already placed at the head

			pSkyboxEntity = pEntity;

			pSkyboxEntity->m_EntityLink.Unlink();

			rRootNode.m_EntityLinkHead.InsertNext( &pSkyboxEntity->m_EntityLink );

			break;
		}
	}
}


void CEntityRenderManager::RenderZSortTable()
{
	int i;
	CCopyEntity* pEntity;

	// start rendering from distant entities and end with nearby entities
	for( i=SIZE_ZSORTTABLE-1; 0<=i; i-- )
	{
		if( !m_apZSortTable[i] )
			continue;
		
		pEntity = m_apZSortTable[i];

		while(pEntity)
		{
/*			if( pEntity->Lighting() )
			{
				if( pEntity->sState & CESTATE_LIGHT_INFORMATION_INVALID )
				{	// need to update light information - find lights that matter to this entity
					pEntity->ClearLightIndices();
					m_pEntitySet->UpdateLightInfo( pEntity );
					pEntity->sState &= ~CESTATE_LIGHT_INFORMATION_INVALID;
				}

				// turn on lights that reach 'pCopyEnt'
				m_pEntitySet->EnableLightForEntity();
				m_pEntitySet->SetLightsForEntity( pEntity );
			}
			else
			{	// turn off lights
				m_pEntitySet->DisableLightForEntity();
			}
*/
			// render the entity
			pEntity->Draw();

			pEntity = pEntity->m_pNextEntityInZSortTable;
		}

		// clear z-sort table
		m_apZSortTable[i] = NULL;
	}
}


void CEntityRenderManager::SendToZSortTable(CCopyEntity* pCopyEnt)
{
	float fZinCameraSpace;
	int iZSortValue;

	const float fMaxRenderDepth = m_fCameraFarClipDist;

	const Vector3 vCameraForwardDirection	= m_CameraPose.matOrient.GetColumn(2);//Vector3( m_matCamera._13, m_matCamera._23, m_matCamera._33 );
	const Vector3 vCameraToEntity = pCopyEnt->GetWorldPosition() - m_CameraPose.vPosition;

	fZinCameraSpace = Vec3Dot( vCameraToEntity, vCameraForwardDirection );

	if( fZinCameraSpace < -pCopyEnt->fRadius || fMaxRenderDepth + pCopyEnt->fRadius <= fZinCameraSpace )
		return;	//not in the camera's view frustum

	if( fZinCameraSpace < 0 )
		fZinCameraSpace = 0.0f;
	else if( fMaxRenderDepth <= fZinCameraSpace )
		fZinCameraSpace = fMaxRenderDepth - 0.1f;

	pCopyEnt->fZinCameraSpace = fZinCameraSpace;

	iZSortValue = (int)( fZinCameraSpace / fMaxRenderDepth * (float)(SIZE_ZSORTTABLE - 1) );

	// simple but inaccurate
	//pCopyEnt->m_pNextEntityInZSortTable = m_apZSortTable[iZSortValue];
	//m_apZSortTable[iZSortValue] = pCopyEnt;

	// more accurate but slower
	CCopyEntity *pZSortedEntity, *pPrevZSortedEntity;
	if( !m_apZSortTable[iZSortValue] )
	{
		// the current z-sort list is empty. (contains no copy entity)
		pCopyEnt->m_pNextEntityInZSortTable = NULL;
		m_apZSortTable[iZSortValue] = pCopyEnt;
		return;
	}
	else if( fZinCameraSpace < m_apZSortTable[iZSortValue]->fZinCameraSpace )	// compare z with the 1st copy entity in the current z-sort list
	{
		// 'pCopyEnt' has the minimum z value and should be placed at the head of the z-sort list
		pCopyEnt->m_pNextEntityInZSortTable = m_apZSortTable[iZSortValue];
		m_apZSortTable[iZSortValue] = pCopyEnt;
		return;
	}
	else
	{	// start searching from 2nd copy entity in this z-sort list
		pZSortedEntity = m_apZSortTable[iZSortValue]->m_pNextEntityInZSortTable;
		pPrevZSortedEntity = m_apZSortTable[iZSortValue];
		while(1)
		{
			if( !pZSortedEntity || fZinCameraSpace < pZSortedEntity->fZinCameraSpace )
			{
				pPrevZSortedEntity->m_pNextEntityInZSortTable = pCopyEnt;
				pCopyEnt->m_pNextEntityInZSortTable = pZSortedEntity;
				break;
			}
			pPrevZSortedEntity = pZSortedEntity;
			pZSortedEntity = pZSortedEntity->m_pNextEntityInZSortTable;
		}
		return;
	}

}


void CEntityRenderManager::RenderScene( CCamera& rCam )
{
	//==================== render the entities ====================

	CEntityNode::ms_NumRenderedEntities = 0;

	// move the skybox to the head of the list to render it first
	MoveSkyboxToListHead();

	// render the entity tree by downward traversal
	RenderDownward_r( 0, rCam );

	// render entities that have translucent polygons
	RenderZSortTable();

	// render groups of copy entities that belong to a same base entity and should be rendered in succession 
	// e.g. bullet hold decals
	size_t i, num_sweeprender_base_entities = m_vecpSweepRenderBaseEntity.size();
	for(i=0; i<num_sweeprender_base_entities; i++)
	{
		m_vecpSweepRenderBaseEntity[i]->SweepRender();
		m_vecpSweepRenderBaseEntity[i]->ClearSweepRenderTable();
	}
}


void CEntityRenderManager::RenderAllButEnvMapTargetDownward_r( short sEntNodeIndex,
															  CCamera& rCam,
															  U32 target_entity_id )
{
	CEntityNode& rThisEntNode = m_paEntityTree[sEntNodeIndex];

	if( !rCam.ViewFrustumIntersectsWith( rThisEntNode.m_AABB ) )
		return;

	rThisEntNode.RenderAllButEnvMapTraget( rCam, target_entity_id );

	if( rThisEntNode.leaf )
		return;

	RenderAllButEnvMapTargetDownward_r( rThisEntNode.sFrontChild, rCam, target_entity_id );
	RenderAllButEnvMapTargetDownward_r( rThisEntNode.sBackChild,  rCam, target_entity_id );
}


void CEntityRenderManager::RenderAllButEnvMapTarget( CCamera& rCam, U32 target_entity_id )
{
	//==================== render the entities ====================

	CEntityNode::ms_NumRenderedEntities = 0;

	// render the entity tree by downward traversal
	RenderAllButEnvMapTargetDownward_r( 0, rCam, target_entity_id );

	// render entities that have translucent polygons
	RenderZSortTable();

	// render groups of copy entities that belong to a same base entity and should be rendered in succession 
	// e.g. bullet hold decals
	size_t i, num_sweeprender_base_entities = m_vecpSweepRenderBaseEntity.size();
	for(i=0; i<num_sweeprender_base_entities; i++)
	{
		m_vecpSweepRenderBaseEntity[i]->SweepRender();
		m_vecpSweepRenderBaseEntity[i]->ClearSweepRenderTable();
	}
}


void CEntityRenderManager::RenderShadowCasters( CCamera& rCam )
{
	//==================== render the entities ====================

	// move the skybox to the head of the list to render it first
//	MoveSkyboxToListHead();

	// render the entity tree by downward traversal
	RenderShadowCastersDownward_r( 0, rCam );

	// render groups of copy entities that belong to a same base entity and should be rendered in succession 
	// e.g. bullet hold decals
/*	size_t i, num_sweeprender_base_entities = m_vecpSweepRenderBaseEntity.size();
	for(i=0; i<num_sweeprender_base_entities; i++)
	{
		m_vecpSweepRenderBaseEntity[i]->SweepRenderShadowCasters();
//		m_vecpSweepRenderBaseEntity[i]->ClearSweepRenderTable();
	}
*/
}


void CEntityRenderManager::RenderShadowReceivers( CCamera& rCam )
{
	//==================== render the entities ====================

	// render the entity tree by downward traversal
	RenderShadowReceiversDownward_r( 0, rCam );

	// render groups of copy entities that belong to a same base entity and should be rendered in succession 
	// e.g. bullet hold decals
/*	size_t i, num_sweeprender_base_entities = m_vecpSweepRenderBaseEntity.size();
	for(i=0; i<num_sweeprender_base_entities; i++)
	{
		m_vecpSweepRenderBaseEntity[i]->SweepRenderShadowReceivers();
//		m_vecpSweepRenderBaseEntity[i]->ClearSweepRenderTable();
	}
*/
}


void CEntityRenderManager::SetLightForShadow( const string& light_entity_name )
{
	CCopyEntity *pLightEntity = m_pEntitySet->GetEntityByName( light_entity_name.c_str() );
	if( pLightEntity )
	{
		m_vecLightForShadow.push_back( pLightEntity->Self() );
	}
}


void CEntityRenderManager::RenderSceneToCubeMap( CCamera& camera )
{
	RenderAllButEnvMapTarget( camera, m_CurrentEnvMapTargetEntityID );
}

/*
void CEntityRenderManager::RenderSceneWithShadowMap( CCamera& rCam, 
													 CScreenEffectManager *pScreenEffectMgr )
{
	pScreenEffectMgr->RaiseEffectFlag( ScreenEffect::ShadowMap );

	// render shadow map
	// the entities that cast shadows to other entities are rendered in this phase
	// shader manager of shadow map is set to CShader (singleton)
	m_pShadowManager->BeginSceneShadowMap();

	RenderShadowCasters( rCam );

	m_pShadowManager->EndSceneShadowMap();


	m_pShadowManager->BeginSceneDepthMap();

	m_pShadowManager->SetSceneCamera( rCam );
	RenderShadowReceivers( rCam );

	m_pShadowManager->EndSceneDepthMap();


	// render the scene with shadowmap
	// entities that receive shadows are rendered with shaders that support the shadowmap
	m_pShadowManager->BeginScene();

	pScreenEffectMgr->SetShaderManager();

	RenderScene( rCam );

	m_pShadowManager->EndScene();


	// shadow map texture and scene depth texture should be ready
	// at this point since they are supposed to be rendered as separate
	// render tasks in advance
	// - just draw the fullscreen rect to render the scene with shadow
	m_pShadowManager->RenderSceneWithShadow();
}*/


void CEntityRenderManager::UpdateEnvironmentMapTargets()
{
	if( m_vecEnvMapTarget.size() == 0 )
		return;

	if( !m_bEnableEnvironmentMap )
		return;

	// remove entities from target list if it has been already destroyed
	vector<CEnvMapTarget>::iterator itr = m_vecEnvMapTarget.begin();
	while( itr != m_vecEnvMapTarget.end() )
	{
		if( !IsValidEntity(itr->m_pEntity)
		 || itr->m_pEntity->GetID() != itr->m_EntityID )
		{
			// the entity has already been destroyed
			// - remove from the list
			itr = m_vecEnvMapTarget.erase( itr );
		}
		else
			itr++;
	}
}


void CEntityRenderManager::UpdateEnvironmentMapTextures()
{
	if( m_vecEnvMapTarget.size() == 0 )
		return;

	if( !m_bEnableEnvironmentMap )
		return;

	// envmap is enabled and there is at least one entity that needs envmap texture

	if( !m_pCubeMapManager
	 || !m_pCubeMapManager->IsReady() )
	{
		return; // envmap is not available
	}

	size_t num_envmap_targets = m_vecEnvMapTarget.size();

	if( 1 < num_envmap_targets )
		num_envmap_targets = 1;

	for( size_t i=0; i<num_envmap_targets; i++ )
	{
		// update env map texture

		m_CurrentEnvMapTargetEntityID = m_vecEnvMapTarget[i].m_EntityID;

		// CEntityRenderManager::RenderSceneToCubeMap() is called 6 times in this call
		// to render the scene to cube map texture
		m_pCubeMapManager->RenderToCubeMap();

		m_CurrentEnvMapTargetEntityID = 0;
	}
}


//CTextureHandle CEntityRenderManager::GetEnvMapTexture( U32 entity_id )
LPDIRECT3DCUBETEXTURE9 CEntityRenderManager::GetEnvMapTexture( U32 entity_id )
{
	if( m_pCubeMapManager )
		return m_pCubeMapManager->GetCubeTexture();
	else
		return NULL;

//	return CTextureHandle();
}


bool CEntityRenderManager::AddEnvMapTarget( CCopyEntity *pEntity )
{
	if( !IsValidEntity(pEntity) )
		return false;

	if( m_vecEnvMapTarget.size() == NUM_MAX_ENVMAP_TARGETS )
	{
		LOG_PRINT_ERROR( "cannot add any more env map targets" );
		return false;
	}

	m_vecEnvMapTarget.push_back( CEnvMapTarget() );
	CEnvMapTarget& target = m_vecEnvMapTarget.back();
	target.m_pEntity  = pEntity;
	target.m_EntityID = pEntity->GetID();
	int cube_tex_index = 0;
	target.m_pCubeMapTextureLoader
		= shared_ptr<CCubeTextureParamsLoader>( new CCubeTextureParamsLoader( pEntity->Self().lock(), this, cube_tex_index ) );

	if( pEntity->m_pMeshRenderMethod )
		pEntity->m_pMeshRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( target.m_pCubeMapTextureLoader );

	// init cube map manager if this is the first target
	if( !m_pCubeMapManager )
	{
		LOG_PRINT( " - creating a cube map manager" );
		m_pCubeMapManager = new CCubeMapManager();
		m_pCubeMapManager->Init();
		m_pCubeMapManager->SetCubeMapSceneRenderer( this );
	}

	return true;
}


void CEntityRenderManager::SaveEnvMapTextureToFile( const std::string& output_image_filename )
{
	if( m_pCubeMapManager )
		m_pCubeMapManager->SaveCubeTextureToFile( output_image_filename );
}


bool CEntityRenderManager::RemoveEnvMapTarget( CCopyEntity *pEntity )
{
	const size_t num_envmap_targets = m_vecEnvMapTarget.size();
	for( size_t i=0; i<num_envmap_targets; i++ )
	{
		if( m_vecEnvMapTarget[i].m_EntityID == pEntity->GetID() )
		{
			if( pEntity->m_pMeshRenderMethod )
			{
				pEntity->m_pMeshRenderMethod->RemoveShaderParamsLoaderToAllMeshRenderMethods(
					m_vecEnvMapTarget[i].m_pCubeMapTextureLoader
					);
			}

			m_vecEnvMapTarget.erase( m_vecEnvMapTarget.begin() + i );

			return true;
		}
	}

	return false;
}


bool CEntityRenderManager::EnableSoftShadow( float softness, int shadowmap_size )
{
	SafeDelete( m_pShadowManager );

	m_pShadowManager = new CVarianceShadowMapManager();

	m_pShadowManager->SetSceneRenderer( m_pShadowMapSceneRenderer );

	bool initialized = m_pShadowManager->Init();
	if( !initialized )
	{
		// shadow map is not available
		// graphics card does not support float point buffer
		// or there is not enough graphics memory, etc.
		SafeDelete( m_pShadowManager );
		return false;
	}

	return true;
}


bool CEntityRenderManager::EnableShadowMap( int shadow_map_size )
{
	SafeDelete( m_pShadowManager );

	m_pShadowManager = new CShadowMapManager();

	m_pShadowManager->SetSceneRenderer( m_pShadowMapSceneRenderer );

//	m_pShadowManager->SetShadowMapSize( shadow_map_size );

	bool shadow_map_mgr_initialized = m_pShadowManager->Init();
	if( !shadow_map_mgr_initialized )
	{
		// shadow map is not available
		// graphics card does not support float point buffer
		// or there is not enough graphics memory, etc.
		SafeDelete( m_pShadowManager );
		return false;
	}

	return true;
}


void CEntityRenderManager::DisableShadowMap()
{
	SafeDelete( m_pShadowManager );
}


/// Find lights near camera and update shadow map settings for the found lights.
void CEntityRenderManager::UpdateLightsForShadow()
{
	Vector3 vCamCenter = Vector3(0,0,0);
	if( m_pCurrentCamera )
	{
		vCamCenter = m_pCurrentCamera->GetPosition();
	}

	// clear the entity buffer
	m_vecpEntityBuffer.resize( 0 );

	float r = 200;
	AABB3 aabb;
	aabb.vMin = vCamCenter - Vector3(r,r,r);
	aabb.vMax = vCamCenter + Vector3(r,r,r);
	COverlapTestAABB aabb_test( aabb, &m_vecpEntityBuffer );

	/// collect only the light entities
	aabb_test.TargetEntityTypeID = CCopyEntityTypeID::LIGHT_ENTITY;
	aabb_test.GroupIndex = -1;

	// get light entities that are near the camera
	// and should be considered as lights for shadow
	m_pEntitySet->GetOverlappingEntities( aabb_test );

//	vector<float> vecLightScore;
//	vecLightScore.reserve

	const size_t num_entities = m_vecpEntityBuffer.size();
	for( size_t i=0; i<num_entities; i++ )
	{
		if( m_vecpEntityBuffer[i]->GetEntityTypeID() != CCopyEntityTypeID::LIGHT_ENTITY )
			continue;

		CLightEntity *pLightEntity = dynamic_cast<CLightEntity *>(m_vecpEntityBuffer[i]);
		if( !pLightEntity )
			continue;

		CLight *pLight = pLightEntity->GetLightObject();
		if( !pLight )
			continue;

		m_pShadowManager->UpdateLightForShadow( pLightEntity->GetID(), *pLight );

		// found a light entity
//		m_pShadowManager->AddShadowForLight(
	}
}


void CEntityRenderManager::ReleaseGraphicsResources()	
{
	size_t i, num_base_entities = m_pEntitySet->m_vecpBaseEntity.size();
	for(i=0; i<num_base_entities; i++)
		m_pEntitySet->m_vecpBaseEntity[i]->ReleaseGraphicsResources();

//	m_pEntitySet->m_pLightEntityManager->ReleaseGraphicsResources();

	// m_pCubeMapManager - graphics component
	// m_pShadowManager - graphics component
}


void CEntityRenderManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	size_t i, num_base_entities = m_pEntitySet->m_vecpBaseEntity.size();
	for(i=0; i<num_base_entities; i++)
		m_pEntitySet->m_vecpBaseEntity[i]->LoadGraphicsResources( rParam );

//	m_pEntitySet->m_pLightEntityManager->LoadGraphicsResources();

	// m_pCubeMapManager - graphics component
	// m_pShadowManager - graphics component
}


void CEntityRenderManager::LoadTextures()
{
	// load texture for glare effect
//	m_TransparentTexture.Load( "Texture\\TransparentTex.dds" );

//	m_TranslucentTexture.Load( "Texture\\TranslucentTex.dds" );
}





void CEntityRenderManager::RenderSceneWithShadowMap( CCamera& rCam )
{
	m_pShadowManager->RenderSceneWithShadow();
}


/**
 - Render the shadow caster entities to shadow map texture(s).
 - Render the shadow receiver entities.
 - Render the scene without shadow to render target texture.
*/
void CEntityRenderManager::RenderForShadowMaps( CCamera& rCam )//, 
//													 CScreenEffectManager *pScreenEffectMgr )
{
	if( m_bOverrideShadowMapLight )
	{
//		m_pShadowManager->SetLightPosition( m_vOverrideShadowMapPosition );
//		m_pShadowManager->SetLightDirection( m_vOverrideShadowMapDirection );
	}

	static float near_clip = 0.001f;
	static float far_clip  = 100.0f;
	UPDATE_PARAM( "debug/graphics_params.txt", "shadowmap_scene_cam_nearclip", near_clip );
	UPDATE_PARAM( "debug/graphics_params.txt", "shadowmap_scene_cam_farclip",  far_clip );
	m_pShadowManager->SetCameraPosition( rCam.GetPosition() );
	m_pShadowManager->SetCameraDirection( rCam.GetFrontDirection() );
	m_pShadowManager->SceneCamera().SetNearClip( near_clip );
	m_pShadowManager->SceneCamera().SetFarClip( far_clip );

	// render the objects that cast shadows to others

	// CEntityRenderManager::RenderShadowCasters() is called 1 or more times
	m_pShadowManager->RenderShadowCasters( rCam );
/*
	// render shadow map
	// the entities that cast shadows to other entities are rendered in this phase
	// shader manager of shadow map is set to CShader (singleton)
	m_pShadowManager->BeginSceneShadowMap();

	// render to shadow map texture
	// BeginScene() and EndScene() pair is called inside
	RenderShadowCasters( rCam );

	m_pShadowManager->EndSceneShadowMap();
*/

	m_pShadowManager->SetSceneCamera( rCam );
	m_pShadowManager->SceneCamera().SetNearClip( near_clip );
	m_pShadowManager->SceneCamera().SetFarClip( far_clip );

	// render the objects that receive shadows

	m_pShadowManager->RenderShadowReceivers( rCam );
/*
	m_pShadowManager->BeginSceneDepthMap();

	// Render to scene depth texture
	// BeginScene() and EndScene() pair is called inside

	RenderShadowReceivers( rCam );

	m_pShadowManager->EndSceneDepthMap();
*/

	// set texture render target and call IDirect3DDevice9::BeginScene();
	m_pShadowManager->BeginScene();

	CShader::Get()->SetShaderManager( m_FallbackShader.GetShaderManager() );

	RenderScene( rCam );

	// call IDirect3DDevice9::EndScene() and reset the prev render target;
	m_pShadowManager->EndScene();
}


void CEntityRenderManager::CreateEnvMapRenderTasks()
{
	RenderTaskProcessor.AddRenderTask( new CEntityEnvMapRenderTask( this ) );
}


void CEntityRenderManager::CreateShadowMapRenderTasks( CCamera& rCam )
{
	RenderTaskProcessor.AddRenderTask( new CEntityShadowMapRenderTask( this, &rCam, NULL ) );
}


void CEntityRenderManager::CreateSceneRenderTask( CCamera& rCam )
{
	RenderTaskProcessor.AddRenderTask( new CEntitySceneRenderTask( this, &rCam ) );
}


void CEntityRenderManager::CreateRenderTasks( bool create_scene_render_task )
{
	if( 0 < m_vecEnvMapTarget.size() 
	 && m_bEnableEnvironmentMap
	 && m_pCubeMapManager
	 && m_pCubeMapManager->IsReady() )
	{
		UpdateEnvironmentMapTargets();

		CreateEnvMapRenderTasks();
	}

	if( this->m_pShadowManager )
	{
		// the scene needs to be rendered to texture render target in advance
		// - it will be later drawn as fullscreen rect blended with shadow overlay texture
		CreateShadowMapRenderTasks( *m_pCurrentCamera );
	}
	else
	{
		// scene is rendered directly to the original render target
		// by Render() call
	}

	if( create_scene_render_task )
		CreateSceneRenderTask( *m_pCurrentCamera );
}


void CEntityRenderManager::Render( CCamera& rCam )
{
	PROFILE_FUNCTION();

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// update camera matrix & camera's position
	rCam.GetPose( m_CameraPose );
	m_fCameraFarClipDist = rCam.GetFarClip();

	Matrix44 matView, matProj;
	rCam.GetCameraMatrix(matView);
	rCam.GetProjectionMatrix(matProj);

	FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );
	FixedFunctionPipelineManager().SetViewTransform( matView );
	FixedFunctionPipelineManager().SetProjectionTransform( matProj );

	// clear dest buffer
    pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(64,64,64), 1.0f, 0 );

	GraphicsDevice().SetRenderState( RenderStateType::DEPTH_TEST, true );

	// set alpha test
	pd3dDev->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001);
    pd3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
    pd3dDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	// enable lights of graphics device for entities
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING, true );

	// test - add some ambient light
//    pd3dDev->SetRenderState( D3DRS_AMBIENT, 0x00202020 );

	/// set view and projection matrices of all shader managers
	ShaderManagerHub.PushViewAndProjectionMatrices( rCam );


	// create env map texture based no the current camera pose
	bool do_not_use_render_task = true;
	if( do_not_use_render_task )
	{
		UpdateEnvironmentMapTextures();
	}

	bool rendered_with_shadow = false;
	if( m_pShadowManager )
	{
		UpdateLightsForShadow();

		if( m_pShadowManager->HasShadowMap() )
		{
			RenderForShadowMaps( rCam );

			// render the scene as fullscreen rect with
			// scene texture and shadow overlay texture
			RenderSceneWithShadowMap( rCam );
			rendered_with_shadow = true;
		}
	}

	if( !rendered_with_shadow )
	{
		// directly render the scene to the currenet render target
		CShader::Get()->SetShaderManager( m_FallbackShader.GetShaderManager() );
		RenderScene( rCam );
	}

	ShaderManagerHub.PopViewAndProjectionMatrices();

	static int s_SaveSceneTextureOfShadowMapToFile = 0;
	if( s_SaveSceneTextureOfShadowMapToFile )
	{
		if( m_pShadowManager )
			m_pShadowManager->SaveSceneTextureToFile( "debug/scene_for_shadowmap.bmp" );
		s_SaveSceneTextureOfShadowMapToFile = 0;
	}
}
