#include "BaseEntity.hpp"
#include "BaseEntity_Draw.hpp"
#include "CopyEntity.hpp"
#include "Stage.hpp"
#include "EntityRenderManager.hpp"
#include "ScreenEffectManager.hpp"
#include "LightEntity.hpp"

#include "Graphics/Camera.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/ShaderLightManager.hpp"
#include "Graphics/MeshGenerators.hpp"
#include "Graphics/Mesh/SkeletalMesh.hpp"
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


static Vector3 s_OrigViewTrans;
static Vector3 s_OrigWorldPos;

static void SetOffsetWorldTransform( CShaderManager& rShaderManager, Vector3& vWorldCameraPos )
{
	Matrix44 matWorld, matView;
	rShaderManager.GetWorldTransform( matWorld );
	rShaderManager.GetViewTransform( matView );
	s_OrigWorldPos  = Vector3( matWorld(0,3), matWorld(1,3), matWorld(2,3) );
	s_OrigViewTrans = Vector3( matView(0,3),  matView(1,3),  matView(2,3) );

//	if( !pCamera )
//		return;

	matView(0,3) = matView(1,3) = matView(2,3) = 0;
	
	Vector3 cam_pos = vWorldCameraPos;//pCamera->GetPosition();
	matWorld(0,3) = s_OrigWorldPos.x - cam_pos.x;
	matWorld(1,3) = s_OrigWorldPos.y - cam_pos.y;
	matWorld(2,3) = s_OrigWorldPos.z - cam_pos.z;

//	MsgBoxFmt( "world pos offset: ( %f, %f, %f )", matWorld._41, matWorld._42, matWorld._43 );

	rShaderManager.SetWorldTransform( matWorld );
	rShaderManager.SetViewTransform( matView );
}


static void RestoreOffsetWorldTransform( CShaderManager& rShaderManager )
{
	Matrix44 matWorld, matView;
	rShaderManager.GetWorldTransform( matWorld );
	rShaderManager.GetViewTransform( matView );

	matWorld(0,3) = s_OrigWorldPos.x;
	matWorld(1,3) = s_OrigWorldPos.y;
	matWorld(2,3) = s_OrigWorldPos.z;

	matView(0,3) = s_OrigViewTrans.x;
	matView(1,3) = s_OrigViewTrans.y;
	matView(2,3) = s_OrigViewTrans.z;

	rShaderManager.SetWorldTransform( matWorld );
	rShaderManager.SetViewTransform( matView );
}


class COffsetWorldTransformLoader : public CShaderParamsLoader
{
	Vector3 m_vCameraPosition;

	bool m_bActive;

public:

	COffsetWorldTransformLoader()
		:
	m_bActive(true),
	m_vCameraPosition( Vector3(0,0,0) )
	{
		m_bActive = false;
	}

	void SetActive( bool active ) { m_bActive = active; }

	void SetCameraPosition( Vector3 vCameraPos ) { m_vCameraPosition = vCameraPos; }

	void UpdateShaderParams( CShaderManager& rShaderMgr );
	void ResetShaderParams( CShaderManager& rShaderMgr );
};


void COffsetWorldTransformLoader::UpdateShaderParams( CShaderManager& rShaderMgr )
{
	if( !m_bActive )
		return;

	SetOffsetWorldTransform( rShaderMgr, m_vCameraPosition );
}


void COffsetWorldTransformLoader::ResetShaderParams( CShaderManager& rShaderMgr )
{
	if( !m_bActive )
		return;

	RestoreOffsetWorldTransform( rShaderMgr );
}

static shared_ptr<COffsetWorldTransformLoader> sg_pWorldTransLoader( new COffsetWorldTransformLoader() );


void InitSkeletalMesh(  )
{
//	shared_ptr<CBlendMatricesLoader> pBlendMatricesLoader( new CBlendMatricesLoader() );
//	entity.m_pMeshRenderMethod->AddParamsLoaderToAllRenderMethods( pBlendMatricesLoader );

//	if(  )
}


/// Perhaps this should be a member of CCopyEntity, not CBaseEntity
/// - m_pStage is the only member of the base entity
/// - CCopyEnttiy has m_pStage
void CBaseEntity::SetAsEnvMapTarget( CCopyEntity& entity )
{
	if( entity.GetEntityFlags() & BETYPE_ENVMAPTARGET )
	{
//		shared_ptr<CCubeTextureParamsLoader> pCubeTexLoader( new CCubeTextureParamsLoader() );
//		pCubeTexLoader->SetCubeTexture( 0, m_pStage->GetEntitySet()->GetRenderManager()->GetEnvMapTexture(entity.GetID()) );

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


void UpdateLightInfo( CCopyEntity& entity )
{
	if( entity.Lighting() )
	{
		if( entity.sState & CESTATE_LIGHT_INFORMATION_INVALID )
		{
			// need to update light information - find lights that reaches to this entity
			entity.ClearLights();
			entity.GetStage()->GetEntitySet()->UpdateLights( &entity );
			entity.sState &= ~CESTATE_LIGHT_INFORMATION_INVALID;
		}
	}
}


void UpdateEntityForRendering( CCopyEntity& entity )
{
	CBaseEntity& base_entity = *(entity.pBaseEntity);

	// light params writer
	if( entity.GetEntityFlags() & BETYPE_LIGHTING )
	{
		UpdateLightInfo( entity );

		if( base_entity.MeshProperty().m_pShaderLightParamsLoader )
		{
			// The light params loader is shared by all the entities
			base_entity.MeshProperty().m_pShaderLightParamsLoader->SetEntity( entity.Self().lock() );
		}
	}

	const float offset_world_transform_threshold = 150000.0f;
	if( square(offset_world_transform_threshold) < Vec3LengthSq(entity.GetWorldPose().vPosition) )
	{
		CCamera* pCam = entity.GetStage()->GetCurrentCamera();
		if( pCam )
		{
			sg_pWorldTransLoader->SetActive( true );
			sg_pWorldTransLoader->SetCameraPosition( pCam->GetPosition() );
		}
		else
			sg_pWorldTransLoader->SetActive( false );

	}
	else
		sg_pWorldTransLoader->SetActive( false );
}


void CBaseEntity::RenderEntity( CCopyEntity& entity )
{
	UpdateEntityForRendering( entity );

	// default render states for fixed function pipeline

	// alpha-blending settings
	GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );
	GraphicsDevice().SetSourceBlendMode( AlphaBlend::SrcAlpha );
	GraphicsDevice().SetDestBlendMode( AlphaBlend::InvSrcAlpha );
//	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	m_SrcAlphaBlend );
//	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	m_DestAlphaBlend );

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


void CBaseEntity::SetMeshRenderMethod( CCopyEntity& entity )
{
/*	if( m_EntityFlag & SHARE_RENDER_METHODS )
	{
		entity.m_pRenderMethod
			= m_MeshProperty.m_pMeshRenderMethod;
	}*/

	shared_ptr<CBasicMesh> pMesh = entity.m_MeshHandle.GetMesh();
	if( !pMesh )
		return;

	if( pMesh->GetMeshType() == CMeshType::SKELETAL )
	{
		shared_ptr<CSkeletalMesh> pSkeletalMesh
			= boost::dynamic_pointer_cast<CSkeletalMesh,CBasicMesh>(pMesh);

//		shared_ptr<CBlendMatricesLoader> pBlendMatricesLoader( new CBlendMatricesLoader(pSkeletalMesh) );
		shared_ptr<CBlendTransformsLoader> pBlendMatricesLoader( new CBlendTransformsLoader(pSkeletalMesh) );
		entity.m_pMeshRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( pBlendMatricesLoader );
	}

//		m_pShaderLightParamsLoader->SetEntity( pCopyEnt->Self() );
}


void CBaseEntity::Init3DModel()
{
	m_MeshProperty.Release();

	m_MeshProperty.LoadMeshObject();

	CBasicMesh *pMesh = m_MeshProperty.m_MeshObjectHandle.GetMesh().get();

	// shader
	// - load shader of its own if the shader filepath has been specified.
	// - borrow the fallback shader of entity render manager if no shader
	//   filepath has been specified.

	if( 0 < m_MeshProperty.m_ShaderFilepath.length() )
		m_MeshProperty.m_ShaderHandle.Load( m_MeshProperty.m_ShaderFilepath );
	else
		m_MeshProperty.m_ShaderHandle = m_pStage->GetEntitySet()->GetRenderManager()->GetFallbackShader();

	// alpha blending

	if( pMesh && (m_EntityFlag & BETYPE_SUPPORT_TRANSPARENT_PARTS) )
	{
		const float error_for_alpha = 0.0001f;
		const int num_materials = pMesh->GetNumMaterials();
		for( int i=0; i<num_materials; i++ )
		{
			const CMeshMaterial& mat = pMesh->GetMaterial( i );

			if( 1.0f - error_for_alpha < mat.fMinVertexDiffuseAlpha )
			{
				m_MeshProperty.m_vecTargetMaterialIndex.push_back( i );
			}
		}
	}

	// create default mesh render method

	CSubsetRenderMethod render_method;
	render_method.m_Shader = m_MeshProperty.m_ShaderHandle;
	if( 0 < m_MeshProperty.m_ShaderTechnique.size_x() )
		render_method.m_Technique = m_MeshProperty.m_ShaderTechnique(0,0);

	m_MeshProperty.m_pMeshRenderMethod
		= shared_ptr<CMeshContainerRenderMethod>( new CMeshContainerRenderMethod() );

	if( m_EntityFlag & BETYPE_LIGHTING )
	{
		// parameters loader for lighting
		m_MeshProperty.m_pShaderLightParamsLoader
			= shared_ptr<CEntityShaderLightParamsLoader>( new CEntityShaderLightParamsLoader() );

		render_method.m_vecpShaderParamsLoader.push_back( m_MeshProperty.m_pShaderLightParamsLoader );

//		m_MeshProperty.m_vecpShaderParamsLoader.push_back( m_MeshProperty.m_pShaderLightParamsLoader );
	}

	// offset world transform loader - shared by all the entities
	render_method.m_vecpShaderParamsLoader.push_back( sg_pWorldTransLoader );

	// set the mesh render method for the first LOD
	m_MeshProperty.m_pMeshRenderMethod->MeshRenderMethod().push_back( render_method );

	if( pMesh && pMesh->GetMeshType() == CMeshType::SKELETAL )
	{
		// add blend matrices loader to m_MeshProperty.m_pBlendMatricesLoader
		// if the base entity uses shared render methods
		// - This is not often the case. Skeletal meshes are usually used by game items
		//   and each entity has its own separate render method.
//		m_MeshProperty.m_pBlendMatricesLoader = ;
	}


	m_MeshProperty.m_pShadowCasterRenderMethod           = shared_ptr<CMeshContainerRenderMethod>( new CMeshContainerRenderMethod() );
	m_MeshProperty.m_pShadowReceiverRenderMethod         = shared_ptr<CMeshContainerRenderMethod>( new CMeshContainerRenderMethod() );
	m_MeshProperty.m_pSkeletalShadowCasterRenderMethod   = shared_ptr<CMeshContainerRenderMethod>( new CMeshContainerRenderMethod() );
	m_MeshProperty.m_pSkeletalShadowReceiverRenderMethod = shared_ptr<CMeshContainerRenderMethod>( new CMeshContainerRenderMethod() );
/*
	// blend matrices loader for shadow maps of skeletal meshes
	m_MeshProperty.m_pBlendMatricesLoader
		= shared_ptr<CBlendMatricesLoader>( new CBlendMatricesLoader() );
	m_MeshProperty.m_pSkeletalShadowCasterRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( m_MeshProperty.m_pBlendMatricesLoader );
	m_MeshProperty.m_pSkeletalShadowReceiverRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( m_MeshProperty.m_pBlendMatricesLoader );
*/
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
								  CBasicMesh *pMeshObject,
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
            SetOffsetWorldTransform( pShaderManager, m_pStage->GetCurrentCamera()->GetPosition() );

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
								    CSkeletalMesh *pSkeletalMesh,
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

	shared_ptr<CBasicMesh> pMesh = pCopyEnt->m_MeshHandle.GetMesh();
	if( !pMesh )
		return;

	shared_ptr<CMeshContainerRenderMethod> pMeshRenderMethod;
	if( pMesh->GetMeshType() == CMeshType::SKELETAL )
		pMeshRenderMethod = this->m_MeshProperty.m_pSkeletalShadowCasterRenderMethod;
	else
		pMeshRenderMethod = this->m_MeshProperty.m_pShadowCasterRenderMethod;

	// Do these settings in advance
	// - Notify if the shadow map manager is changed.
	if( true /*render_all_subsets*/ )
	{
		pMeshRenderMethod->MeshRenderMethod().resize( 1 );
		CSubsetRenderMethod& mesh_render_method = pMeshRenderMethod->MeshRenderMethod()[0];
		mesh_render_method.m_Shader    = pShadowMgr->GetShader();
		mesh_render_method.m_Technique.SetTechniqueName( "ShadowMap" );
//		pMeshRenderMethod->MeshRenderMethod().resize( 1 );
//		pMeshRenderMethod.SetMeshRenderMethod( mesh_render_method, 0 );
		pMeshRenderMethod->RenderMesh( pCopyEnt->m_MeshHandle, pCopyEnt->GetWorldPose() );
	}
	else
	{
		// how to render the mesh if each subset is rendered by different render methods
	}
}


void CBaseEntity::RenderAsShadowReceiver(CCopyEntity* pCopyEnt)
{
	PROFILE_FUNCTION();

	// set option to disable texture settings

	CShadowMapManager *pShadowMgr = m_pStage->GetEntitySet()->GetRenderManager()->GetShadowManager();
	if( !pShadowMgr )
		return;

	shared_ptr<CBasicMesh> pMesh = pCopyEnt->m_MeshHandle.GetMesh();
	if( !pMesh )
		return;

	shared_ptr<CMeshContainerRenderMethod> pMeshRenderMethod;
	if( pMesh->GetMeshType() == CMeshType::SKELETAL )
		pMeshRenderMethod = this->m_MeshProperty.m_pSkeletalShadowReceiverRenderMethod;
	else
		pMeshRenderMethod = this->m_MeshProperty.m_pShadowReceiverRenderMethod;

	if( true /*render_all_subsets*/ )
	{
		pMeshRenderMethod->MeshRenderMethod().resize( 1 );
		CSubsetRenderMethod& mesh_render_method = pMeshRenderMethod->MeshRenderMethod()[0];
		mesh_render_method.m_Shader    = pShadowMgr->GetShader();
		mesh_render_method.m_Technique.SetTechniqueName( "SceneShadowMap" );
//		render_method.SetMeshRenderMethod( mesh_render_method, 0 );
		pMeshRenderMethod->RenderMesh( pCopyEnt->m_MeshHandle, pCopyEnt->GetWorldPose() );
	}
	else
	{
		// how to render the mesh if each subset is rendered by different render methods
	}
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


// sets the following shader params loaders to the render method of an entity
// - CEntityShaderLightParamsLoader
//   - Set if the BETYPE_LIGHTING flag is on
// - CBlendMatricesLoader
//   - Set if pEntity->m_MeshHandle is a skeletal mesh
void InitMeshRenderMethod( CCopyEntity &entity )
{
	if( !entity.m_pMeshRenderMethod )
	{
		entity.m_pMeshRenderMethod.reset( new CMeshContainerRenderMethod );
		entity.m_pMeshRenderMethod->MeshRenderMethod().resize( 1 );
	}

	if( entity.GetEntityFlags() & BETYPE_LIGHTING )
	{
		shared_ptr<CEntityShaderLightParamsLoader> pLightParamsLoader( new CEntityShaderLightParamsLoader() );
		pLightParamsLoader->SetEntity( entity.Self() );
		entity.m_pMeshRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( pLightParamsLoader );
	}

	shared_ptr<CBasicMesh> pMesh = entity.m_MeshHandle.GetMesh();
	if( pMesh && pMesh->GetMeshType() == CMeshType::SKELETAL )
	{
		shared_ptr<CSkeletalMesh> pSkeletalMesh
			= boost::dynamic_pointer_cast<CSkeletalMesh,CBasicMesh>(pMesh);

//		shared_ptr<CBlendMatricesLoader> pBlendMatricesLoader( new CBlendMatricesLoader(pSkeletalMesh) );
//		entity.m_pMeshRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( pBlendMatricesLoader );
		shared_ptr<CBlendTransformsLoader> pBlendTransformsLoader( new CBlendTransformsLoader(pSkeletalMesh) );
		entity.m_pMeshRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( pBlendTransformsLoader );
	}

	if( true /* world position of entity has large values */ )
	{
		entity.m_pMeshRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( sg_pWorldTransLoader );
	}
}


void CreateMeshRenderMethod( CEntityHandle<>& entity, 
							 CShaderHandle& shader,
							 CShaderTechniqueHandle& tech )
{
	shared_ptr<CCopyEntity> pEntity = entity.Get();

	if( !pEntity )
		return;

	// create mesh render method from shader and shader technique of the base entity
	CSubsetRenderMethod render_method;

	render_method.m_Shader = shader;
	render_method.m_Technique = tech;

	pEntity->m_pMeshRenderMethod
		= shared_ptr<CMeshContainerRenderMethod>( new CMeshContainerRenderMethod() );

	pEntity->m_pMeshRenderMethod->MeshRenderMethod().push_back( render_method );

	InitMeshRenderMethod( *pEntity );
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
