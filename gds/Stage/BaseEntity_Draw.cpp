#include "BaseEntity.hpp"
#include "BaseEntity_Draw.hpp"
#include "CopyEntity.hpp"
#include "Stage.hpp"
#include "EntityRenderManager.hpp"
#include "ScreenEffectManager.hpp"

#include "Graphics/Camera.hpp"
#include "Graphics/D3DXMeshObject.hpp"
#include "Graphics/D3DXSMeshObject.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/ShaderLightManager.hpp"
#include "Graphics/MeshGenerators.hpp"
#include "Graphics/ShadowMapManager.hpp"

#include "Support/Profile.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Macro.h"

using namespace std;
using namespace boost;


/*
Alpha entities and render methods
- Alpha entities created from different entnties share the same base entity
  - Each alpha entity need to have its own render method
  - They cannot share the same render method of base entity
    - This means dynamic memory allocation is done through shared_ptr<> every time an alpha entity is created in a stage.
/


*/


class CCubeTextureParamsLoader : public CShaderParamsLoader
{
	int m_CubeTexIndex;
	LPDIRECT3DCUBETEXTURE9 m_pCubeTexture;

public:

//	CCubeTextureParamsLoader( int stage, CTextureHandle& cube_texture );
	CCubeTextureParamsLoader( int cube_tex_index = 0, LPDIRECT3DCUBETEXTURE9 pCubeTexture = NULL )
		:
	m_CubeTexIndex(cube_tex_index),
	m_pCubeTexture(pCubeTexture)
	{}

	void SetCubeTexture( int cube_tex_index, LPDIRECT3DCUBETEXTURE9 pCubeTexture )
	{
		m_CubeTexIndex = cube_tex_index;
		m_pCubeTexture = pCubeTexture;
	}

	void UpdateShaderParams( CShaderManager& rShaderMgr )
	{
		rShaderMgr.SetCubeTexture( m_CubeTexIndex, m_pCubeTexture );
	}
};


void SetLightsToShader( CCopyEntity& entity,  CShaderManager& rShaderMgr );


class CEntityShaderLightParamsLoader : public CShaderParamsLoader
{
//	boost::shared_ptr<CCopyEntity> m_pEntity;
	CEntityHandle<> m_Entity;

public:

	CEntityShaderLightParamsLoader( boost::shared_ptr<CCopyEntity> pEntity = boost::shared_ptr<CCopyEntity>() )
		:
	m_Entity(pEntity)
	{}

//	void SetEntity( boost::shared_ptr<CCopyEntity> pEntity ) { m_Entity = CEntityHandle<>( pEntity ); }
	void SetEntity( boost::weak_ptr<CCopyEntity> pEntity ) { m_Entity = CEntityHandle<>( pEntity ); }

	void UpdateShaderParams( CShaderManager& rShaderMgr )
	{
		boost::shared_ptr<CCopyEntity> pEntity = m_Entity.Get();
		if( pEntity )
			SetLightsToShader( *(pEntity.get()), rShaderMgr );
	}
};


class COffsetWorldTransformLoader : public CShaderParamsLoader
{
public:
};


class CBlendMatricesLoader : public CShaderParamsLoader
{
	boost::shared_ptr<CD3DXSMeshObject> m_pSMeshObject;

public:

	void UpdateShaderParams( CShaderManager& rShaderMgr )
	{
		//PROFILE_FUNCTION();

		// set blend matrices to the shader
		LPD3DXEFFECT pEffect = rShaderMgr.GetEffect();
		D3DXMATRIX *paBlendMatrix = m_pSMeshObject->GetBlendMatrices();
		if( pEffect && paBlendMatrix )
		{
			HRESULT hr;
			char acParam[32];
			int i, num_bones = m_pSMeshObject->GetNumBones();

			for( i=0; i<num_bones; i++ )
			{
				sprintf( acParam, "g_aBlendMatrix[%d]", i );
				hr = pEffect->SetMatrix( acParam, &paBlendMatrix[i] );

				if( FAILED(hr) ) return;
			}
		}
	}
};


void InitSkeletalMesh(  )
{
//	shared_ptr<CBlendMatricesLoader> pBlendMatricesLoader( new CBlendMatricesLoader() );
//	entity.m_pMeshRenderMethod->AddParamsLoaderToAllRenderMethods( pBlendMatricesLoader );

//	if(  )
}


//void SetAsEnvMapTarget( CCopyEntity& entity, const std::string& target_mesh_name, const std::string& target_subset_name )

/// Perhaps this should be a member of CCopyEntity, not CBaseEntity
/// - m_pStage is the only member of the base entity
/// - CCopyEnttiy has m_pStage
void CBaseEntity::SetAsEnvMapTarget( CCopyEntity& entity )
{
	if( entity.GetEntityFlags() & BETYPE_ENVMAPTARGET )
	{
		shared_ptr<CCubeTextureParamsLoader> pCubeTexLoader( new CCubeTextureParamsLoader() );
		pCubeTexLoader->SetCubeTexture( 0, m_pStage->GetEntitySet()->GetRenderManager()->GetEnvMapTexture(entity.GetID()) );

//		entity.m_pMeshRenderMethod->AddShaderParamsLoaderToAllRenderMethods( pCubeTexLoader );

/*
		if( 0 < entity.m_pMeshRenderMethod->MeshRenderMethod().size() )
		{
			// shader LOD: fixed to 0
			entity.m_pMeshRenderMethod->MeshRenderMethod(0).m_vecpShaderParamsLoader.push_back( pCubeTexLoader );
		}
		else
		{
			// alpha entity?
		}*/
	}

}



void CBaseEntity::RenderEntity( CCopyEntity& entity )
{
	// default render states for fixed function pipeline
//	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// alpha-blending settings
	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA );
//	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	m_SrcAlphaBlend );
//	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	m_DestAlphaBlend );

	// light params writer
	if( m_EntityFlag & BETYPE_LIGHTING
	 && m_MeshProperty.m_pShaderLightParamsLoader )
	{
		UpdateLightInfo( entity );

		// always shared by all the entities
		m_MeshProperty.m_pShaderLightParamsLoader->SetEntity( entity.Self().lock() );
	}

	if( entity.m_pMeshRenderMethod )
	{
		entity.m_pMeshRenderMethod->RenderMesh( entity.m_MeshHandle, entity.GetWorldPose() );
	}
	else if( m_MeshProperty.m_pMeshRenderMethod )
	{
		m_MeshProperty.m_pMeshRenderMethod->RenderMesh( entity.m_MeshHandle, entity.GetWorldPose() );
	}

//	entity.m_pMeshRenderMethod->RenderMeshContainerNode( *(entity.m_pMeshNode.get()), m_vecpShaderParamsWriterBuffer );
}


static D3DXVECTOR3 s_OrigViewTrans;
static D3DXVECTOR3 s_OrigWorldPos;

static void SetOffsetWorldTransform( CShaderManager& rShaderManager, CCamera *pCamera )
{
	D3DXMATRIX matWorld, matView;
	rShaderManager.GetWorldTransform( matWorld );
	rShaderManager.GetViewTransform( matView );
	s_OrigWorldPos = D3DXVECTOR3(matWorld._41,matWorld._42,matWorld._43);
	s_OrigViewTrans = D3DXVECTOR3(matView._41,matView._42,matView._43);

	if( !pCamera )
		return;

    matView._41 = matView._42 = matView._43 = 0;
	
	D3DXVECTOR3 cam_pos = pCamera->GetPosition();
	matWorld._41 = s_OrigWorldPos.x - cam_pos.x;
	matWorld._42 = s_OrigWorldPos.y - cam_pos.y;
	matWorld._43 = s_OrigWorldPos.z - cam_pos.z;

//	MsgBoxFmt( "world pos offset: ( %f, %f, %f )", matWorld._41, matWorld._42, matWorld._43 );

	rShaderManager.SetWorldTransform( matWorld );
	rShaderManager.SetViewTransform( matView );
}


static void RestoreOffsetWorldTransform( CShaderManager& rShaderManager )
{
	D3DXMATRIX matWorld, matView;
	rShaderManager.GetWorldTransform( matWorld );
	rShaderManager.GetViewTransform( matView );

	matWorld._41 = s_OrigWorldPos.x;
	matWorld._42 = s_OrigWorldPos.y;
	matWorld._43 = s_OrigWorldPos.z;

	matView._41 = s_OrigViewTrans.x;
	matView._42 = s_OrigViewTrans.y;
	matView._43 = s_OrigViewTrans.z;

	rShaderManager.SetWorldTransform( matWorld );
	rShaderManager.SetViewTransform( matView );
}


void CBaseEntity::SetMeshRenderMethod( CCopyEntity& entity )
{
/*	if( m_EntityFlag & SHARE_RENDER_METHODS )
	{
		entity.m_pRenderMethod
			= m_MeshProperty.m_pMeshRenderMethod;
	}*/

	shared_ptr<CD3DXMeshObjectBase> pMesh = entity.m_MeshHandle.GetMesh();
	if( !pMesh )
		return;

	if( pMesh->GetMeshType() == CMeshType::SKELETAL )
	{
		shared_ptr<CBlendMatricesLoader> pBlendMatricesLoader( new CBlendMatricesLoader() );
//		entity.m_pRenderMethod->SetShaderParamsLoaderToAllRenderMethods( pBlendMatricesLoader );
	}

//		m_pShaderLightParamsLoader->SetEntity( pCopyEnt->Self() );
}


void CBaseEntity::Init3DModel()
{
	m_MeshProperty.Release();

	m_MeshProperty.LoadMeshObject();

	CD3DXMeshObjectBase *pMesh = m_MeshProperty.m_MeshObjectHandle.GetMesh().get();

	// shader
	// - load shader of its own if the shader filepath has been specified.
	// - borrow the fallback shader of entity render manager if no shader
	//   filepath has been specified.

	if( 0 < m_MeshProperty.m_ShaderFilepath.length() )
		m_MeshProperty.m_ShaderHandle.Load( m_MeshProperty.m_ShaderFilepath );
	else
		m_MeshProperty.m_ShaderHandle = m_pStage->GetEntitySet()->GetRenderManager()->GetFallbackShader();

	CSubsetRenderMethod render_method;
	render_method.m_Shader = m_MeshProperty.m_ShaderHandle;
	m_MeshProperty.m_pMeshRenderMethod
		= shared_ptr<CMeshContainerRenderMethod>( new CMeshContainerRenderMethod() );

	// alpha blending

	if( pMesh && (m_EntityFlag & BETYPE_SUPPORT_TRANSPARENT_PARTS) )
	{
		const float error_for_alpha = 0.0001f;
		const int num_materials = pMesh->GetNumMaterials();
		for( int i=0; i<num_materials; i++ )
		{
			const CD3DXMeshObjectBase::CMeshMaterial& mat = pMesh->GetMaterial( i );

			if( 1.0f - error_for_alpha < mat.fMinVertexDiffuseAlpha )
			{
				m_MeshProperty.m_vecTargetMaterialIndex.push_back( i );
			}
		}
	}

	if( m_EntityFlag & BETYPE_LIGHTING )
	{
		// parameters loader for lighting
		m_MeshProperty.m_pShaderLightParamsLoader
			= shared_ptr<CEntityShaderLightParamsLoader>( new CEntityShaderLightParamsLoader() );

		m_MeshProperty.m_vecpShaderParamsLoader.push_back( m_MeshProperty.m_pShaderLightParamsLoader );
	}

	if( pMesh && pMesh->GetMeshType() == CMeshType::SKELETAL )
	{
		// add blend matrices loader to m_MeshProperty.m_pBlendMatricesLoader
		// if the base entity uses shared render methods
		// - This is not often the case. Skeletal meshes are usually used by game items
		//   and each entity has its own separate render method.
//		m_MeshProperty.m_pBlendMatricesLoader = ;
	}
}


void CBaseEntity::DrawMeshMaterial( const Matrix34& world_pose, int material_index, int ShaderLOD )
{
	vector<int> single_index;
	single_index.push_back( material_index );

	DrawMeshObject( world_pose,
		            m_MeshProperty.m_MeshObjectHandle.GetMesh().get(),
					single_index,
					m_MeshProperty.m_ShaderTechnique,
					ShaderLOD );
}


void CBaseEntity::DrawMeshMaterial( const Matrix34& world_pose, int material_index, CShaderTechniqueHandle& shader_tech )
{
	vector<int> single_index;
	single_index.push_back( material_index );

	C2DArray<CShaderTechniqueHandle> shader_tech_table;
	shader_tech_table.resize( material_index + 1, 1 );
	shader_tech_table( material_index, 0 ) = shader_tech;

	DrawMeshObject( world_pose,
		            m_MeshProperty.m_MeshObjectHandle.GetMesh().get(),
					single_index,
					shader_tech_table,
					0 );
}


void CBaseEntity::DrawMeshObject( const Matrix34& world_pose,
								  CD3DXMeshObjectBase *pMeshObject,
								  const std::vector<int>& vecTargetMaterialIndex,
							      C2DArray<CShaderTechniqueHandle>& rShaderTechHandleTable,
							      int ShaderLOD )
{
	PROFILE_FUNCTION();
/*
	bool use_offset_world_transform = true;

	bool bSingleTechnique
		= ( m_MeshProperty.m_PropertyFlags & CBE_MeshObjectProperty::PF_USE_SINGLE_TECHNIQUE_FOR_ALL_MATERIALS );

	if( pShaderManager )
	{
		// render the mesh with an HLSL shader

		pShaderManager->SetWorldTransform( matWorld );

		if( use_offset_world_transform )
            SetOffsetWorldTransform( pShaderManager, m_pStage->GetCurrentCamera() );

		if( bSingleTechnique )
		{
			// render the meshes of all the materials with the same shader technique
			// - no need to change techniques for every material
			pShaderManager->SetTechnique( rShaderTechHandleTable( 0, ShaderLOD ) );

			// pMeshObject->Render( *pShaderManager );

			if( vecTargetMaterialIndex.size() == 0 )
			{
				// render all the materials (subsets) with the current shader technique
				pMeshObject->Render( *pShaderManager );
			}
			else
			{
				// render target materials with the current shader technique
				// - Models that does not include transparency
				//   -> Render all the materials of the mesh
				// - Models that includes material(s) with transparency
				//   -> Render the non-transparent materials
				pMeshObject->RenderSubsets( *pShaderManager,
					                        vecTargetMaterialIndex );
			}
		}
		else
		{
			// shader technique needs to be set for each material
			// - set up the array of shader techniques
			m_vecShaderTechniqueHolder.resize( 0 );
			for( int i=0; i<rShaderTechHandleTable.size_x(); i++ )
				m_vecShaderTechniqueHolder.push_back( rShaderTechHandleTable( i, ShaderLOD ) );

			if( m_MeshProperty.m_vecTargetMaterialIndex.size() == 0 )
			{
				// render all the materials (subsets)
				pMeshObject->Render( *pShaderManager, m_vecShaderTechniqueHolder );
			}
			else
			{
				// render target materials
				pMeshObject->RenderSubsets( *pShaderManager,
					                        vecTargetMaterialIndex,
											m_vecShaderTechniqueHolder
											);
			}
		}

		if( use_offset_world_transform )
			RestoreOffsetWorldTransform( pShaderManager );
	}*/
}


void CBaseEntity::Draw3DModel( CCopyEntity* pCopyEnt )
{
	RenderEntity( *pCopyEnt );
//	Draw3DModel( pCopyEnt, m_MeshProperty.m_ShaderTechnique );
}


/**
 * draw skeletal mesh: set local transforms to skeletal mesh object before
 * calling this function
 */
void CBaseEntity::DrawSkeletalMesh( CCopyEntity* pCopyEnt,
								    CD3DXSMeshObject *pSkeletalMesh,
								    C2DArray<CShaderTechniqueHandle>& rShaderTechHandleTable,
									int ShaderLOD )
{
	PROFILE_FUNCTION();

	// World & View matrices are recalculated to avoid occilation in large coord
	// Thus, world & blend matrices need to be set in a special way

//	pSMesh->SetLocalTransformToCache( 0, pCopyEnt->GetWorldPose() );

	// set identity matrix to the root bone since 
	pSkeletalMesh->SetLocalTransformToCache( 0, Matrix34Identity() );//	< usu.?

	pSkeletalMesh->SetLocalTransformsFromCache();

//	SetBlendMatrices( pSkeletalMesh );

	DrawMeshObject( pCopyEnt->GetWorldPose(),
		            pSkeletalMesh,
					m_MeshProperty.m_vecTargetMaterialIndex,
					rShaderTechHandleTable,
					ShaderLOD );

	pSkeletalMesh->ResetLocalTransformsCache();

	return;
}



void CBaseEntity::RenderAsShadowCaster(CCopyEntity* pCopyEnt)
{
	PROFILE_FUNCTION();

	// set option to disable texture settings

	CShadowMapManager *pShadowMgr = m_pStage->GetEntitySet()->GetRenderManager()->GetShadowManager();
	if( !pShadowMgr )
		return;

	shared_ptr<CD3DXMeshObjectBase> pMesh = pCopyEnt->m_MeshHandle.GetMesh();
	if( pMesh )
		return;

	shared_ptr<CMeshContainerRenderMethod> pMeshRenderMethod;
	if( pMesh->GetMeshType() == CMeshType::SKELETAL )
		pMeshRenderMethod = this->m_MeshProperty.m_pSkeletalShadowCasterRenderMethod;
	else
		pMeshRenderMethod = this->m_MeshProperty.m_pShadowCasterRenderMethod;

	pMeshRenderMethod->MeshRenderMethod().resize( 1 );
	CSubsetRenderMethod mesh_render_method = pMeshRenderMethod->MeshRenderMethod()[0];
	mesh_render_method.m_Shader    = pShadowMgr->GetShader();
	mesh_render_method.m_Technique.SetTechniqueName( "ShadowMap" );
//	render_method.SetMeshRenderMethod( mesh_render_method, 0 );

	pMeshRenderMethod->RenderMesh( pCopyEnt->m_MeshHandle, pCopyEnt->GetWorldPose() );
}


void CBaseEntity::RenderAsShadowReceiver(CCopyEntity* pCopyEnt)
{
	PROFILE_FUNCTION();

	// set option to disable texture settings

	CShadowMapManager *pShadowMgr = m_pStage->GetEntitySet()->GetRenderManager()->GetShadowManager();
	if( !pShadowMgr )
		return;

	shared_ptr<CD3DXMeshObjectBase> pMesh = pCopyEnt->m_MeshHandle.GetMesh();
	if( pMesh )
		return;

	shared_ptr<CMeshContainerRenderMethod> pMeshRenderMethod;
	if( pMesh->GetMeshType() == CMeshType::SKELETAL )
		pMeshRenderMethod = this->m_MeshProperty.m_pSkeletalShadowReceiverRenderMethod;
	else
		pMeshRenderMethod = this->m_MeshProperty.m_pShadowReceiverRenderMethod;

	pMeshRenderMethod->MeshRenderMethod().resize( 1 );
	CSubsetRenderMethod mesh_render_method = pMeshRenderMethod->MeshRenderMethod()[0];
	mesh_render_method.m_Shader    = pShadowMgr->GetShader();
	mesh_render_method.m_Technique.SetTechniqueName( "SceneShadowMap" );
//	render_method.SetMeshRenderMethod( mesh_render_method, 0 );

	pMeshRenderMethod->RenderMesh( pCopyEnt->m_MeshHandle, pCopyEnt->GetWorldPose() );
}


/// Update light-related shader variables
void SetLightsToShader( CCopyEntity& entity, CShaderManager& rShaderMgr )
{
	shared_ptr<CShaderLightManager> pShaderLightMgr = rShaderMgr.GetShaderLightManager();

	int i, num_current_lights = entity.GetNumLights();
	CLightEntity *pLightEntity = NULL;

	// clear any lights currenly stored in the shader light manager
	pShaderLightMgr->ClearLights();

	CShaderLightParamsWriter light_params_writer( pShaderLightMgr.get() );

	for( i=0; i<num_current_lights; i++ )
	{
		CEntityHandle<CLightEntity>& light_entity = entity.GetLight( i );
		CLightEntity *pLightEntity = light_entity.GetRawPtr();
		if( !pLightEntity )
			continue;

//		pLightEntity->SetLightToShader( pShaderLightMgr );

		// copy light properties to the shader registers
		pLightEntity->GetLightObject()->Accept( light_params_writer );
	}

	pShaderLightMgr->CommitChanges();
}


void CBaseEntity::UpdateLightInfo( CCopyEntity& entity )
{
	if( entity.Lighting() )
	{
		if( entity.sState & CESTATE_LIGHT_INFORMATION_INVALID )
		{
			// need to update light information - find lights that reaches to this entity
			entity.ClearLights();
			m_pStage->GetEntitySet()->UpdateLights( &entity );
			entity.sState &= ~CESTATE_LIGHT_INFORMATION_INVALID;
		}
	}
}


void CBaseEntity::CreateMeshGenerator( CTextFileScanner& scanner )
{
//	if( scanner.GetTagString != "GENERATED_3DMODEL" )
//		return;

	string tag;
	string type;
	scanner.ScanLine( tag, type );
	if( type == "Box" )
	{
		float x=1,y=1,z=1;
		scanner.ScanLine( tag, type, x, y, z );
		shared_ptr<CBoxMeshGenerator> pBoxGenerator( new CBoxMeshGenerator() );
		pBoxGenerator->SetDiffuseColor( SFloatRGBAColor(0.1f,0.1f,0.1f,1.0f) );
		pBoxGenerator->SetEdgeLengths( Vector3(x,y,z) );

		m_MeshProperty.m_MeshDesc.pMeshGenerator = pBoxGenerator;
	}
}
