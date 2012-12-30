#include "BaseEntity.hpp"
#include "BaseEntity_Draw.hpp"
#include "CopyEntity.hpp"
#include "Stage.hpp"
#include "EntityRenderManager.hpp"
#include "ScreenEffectManager.hpp"
#include "LightEntity.hpp"
#include "MirroredSceneTextureParam.hpp"

#include "Graphics/Camera.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/ShaderLightManager.hpp"
#include "Graphics/Shader/BlendTransformsLoader.hpp"
#include "Graphics/Shader/GenericShaderGenerator.hpp"
#include "Graphics/MeshGenerators/MeshGenerators.hpp"
#include "Graphics/Mesh/SkeletalMesh.hpp"
#include "Graphics/ShadowMapManager.hpp"

#include "Support/stl_aux.hpp"
#include "Support/Profile.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Macro.h"


namespace amorphous
{

using std::string;
using std::vector;
using std::pair;
using boost::shared_ptr;


/*
Alpha entities and render methods
- Alpha entities created from different entnties share the same base entity
  - Each alpha entity need to have its own render method
  - They cannot share the same render method of base entity
    - This means dynamic memory allocation is done through shared_ptr<> every time an alpha entity is created in a stage.
/

Skeletal meshes and transforms
- How transforms are set to shader
  - 2 stages of updates need to be performed before rendering a skeletal mesh.
  []: object
  (): datan
  (local transforms) -> [seletal mesh] -> (world transforms) -> [shader]
  - Different types of entities have different mechanisms for updating local transforms
    - aircrafts: update based on player's input
    - human characters: updated based on the motion being played.

- Setting local transforms to a skeletal mesh.
  1. once in some update function before rendering
    - works if the skeletal mesh is used by a single entity.
	- no need to set the transforms at every render function (e.g. standard render function, chadow caster render function, etc.)
  2. every time a render function is called.
    - necessary if the skeletal mesh is shared by more than one entity.

  - examples:
    - CBE_PlayerPseudoAircraft calls CGI_Aircraft::UpdateTargetMeshTransforms() in CBE_PlayerPseudoAircraft::Draw()

  plan 1. check if the transforms need to be updated in RenderAs() function
  if( mesh.is_skeletal() )
	if( rc.first_render || entity.shared_skeletal_mesh )
	{
		// Update local transforms of skeletal mesh
	}

  plan 2. separate skeletal mesh and storage of world transforms
  - Every entity which uses skeletal mesh has std::vector<Transform> world_transforms
  - CSkeletalMesh::CalculateWorldTransformsFromLocalTransforms( local_transforms, world_transforms ); // Calculates world transforms from local transforms based on the skeleton of the mesh.

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


/// Update shader params loaders for the entity.
/// Shader params loaders are shared by entities, and need to be updated every time an entity is rendered.
void UpdateEntityForRendering( CCopyEntity& entity )
{
	CBaseEntity& base_entity = *(entity.pBaseEntity);

	// light params writer
	if( entity.GetEntityFlags() & BETYPE_LIGHTING )
	{
		UpdateLightInfo( entity );

		if( base_entity.MeshProperty().m_pShaderLightParamsLoader )
		{
			// Set the entity to the light params loader, because a single light params loader
			// is shared by all the entities of this base entity.
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

	// default alpha-blending settings (premultiplied alpha)
	GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );
	GraphicsDevice().SetSourceBlendMode( AlphaBlend::One );
	GraphicsDevice().SetDestBlendMode( AlphaBlend::InvSrcAlpha );

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
//		shared_ptr<CSkeletalMesh> pSkeletalMesh
//			= boost::dynamic_pointer_cast<CSkeletalMesh,CBasicMesh>(pMesh);

		shared_ptr<CBlendTransformsLoader> pBlendMatricesLoader( new CBlendTransformsLoader );//(pSkeletalMesh) );
		entity.m_pMeshRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( pBlendMatricesLoader );
	}

//		m_pShaderLightParamsLoader->SetEntity( pCopyEnt->Self() );
}


static void InitShadowCasterReceiverSettings( shared_ptr<CSkeletalMesh> pSkeletalMesh, CBE_MeshObjectProperty& mesh_property )
{
	ONCE( LOG_PRINT_ERROR( " Not implemented yet." ) );

/*	if( !pSkeletalMesh )
		return;

	shared_ptr<CBlendTransformsLoader> pLoader( new CBlendTransformsLoader );
	pLoader->SetSkeletalMesh( pSkeletalMesh );
	mesh_property.m_pSkeletalShadowCasterRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( pLoader );
	mesh_property.m_pSkeletalShadowReceiverRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( pLoader );

	mesh_property.m_pBlendTransformsLoader = pLoader;
*/
}


void CBaseEntity::Init3DModel()
{
	m_MeshProperty.Release();

	m_MeshProperty.LoadMeshObject();

	shared_ptr<CBasicMesh> pMesh = m_MeshProperty.m_MeshObjectHandle.GetMesh();

	// shader
	// - load shader of its own if the shader filepath has been specified.
	// - borrow the fallback shader of entity render manager if no shader
	//   filepath has been specified.

	if( 0 < m_MeshProperty.m_ShaderDesc.ResourcePath.length() )
		m_MeshProperty.m_ShaderHandle.Load( m_MeshProperty.m_ShaderDesc.ResourcePath );
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

	m_MeshProperty.m_pMeshRenderMethod.reset( new CMeshContainerRenderMethod() );

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
	m_MeshProperty.m_pMeshRenderMethod->PrimaryMeshRenderMethod() = render_method;

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
								  CBasicMesh *pMeshObject,
								  const std::vector<int>& vecTargetMaterialIndex,
							      C2DArray<CShaderTechniqueHandle>& rShaderTechHandleTable,
							      int ShaderLOD )
{
	PROFILE_FUNCTION();
}


void CBaseEntity::Draw3DModel( CCopyEntity* pCopyEnt )
{
	RenderEntity( *pCopyEnt );
}


// How to render meshes that are stored in a mesh node tree and have different world transforms?
void CBaseEntity::RenderAsShadowCaster(CCopyEntity* pCopyEnt)
{
	ONCE( LOG_PRINT_ERROR( " Not implemented yet." ) );

	PROFILE_FUNCTION();

	// set option to disable texture settings

	CShadowMapManager *pShadowMgr = m_pStage->GetEntitySet()->GetRenderManager()->GetShadowManager();
	if( !pShadowMgr )
		return;

	shared_ptr<CBasicMesh> pMesh = pCopyEnt->m_MeshHandle.GetMesh();
	if( !pMesh )
		return;

	CShaderHandle shader = pShadowMgr->GetShader();
	CShaderManager *pShaderMgr = shader.GetShaderManager();
	if( !pShaderMgr )
		return;

	pShaderMgr->SetWorldTransform( pCopyEnt->GetWorldPose() );

	pShaderMgr->SetTechnique( pShadowMgr->ShaderTechniqueForShadowCaster( CVertexBlendType::NONE ) );

	pMesh->Render( *pShaderMgr );

/*	shared_ptr<CMeshContainerRenderMethod> pMeshRenderMethod;
	const bool is_skeletal_mesh = (pMesh->GetMeshType() == CMeshType::SKELETAL);
	if( is_skeletal_mesh )
	{
		pMeshRenderMethod = this->m_MeshProperty.m_pSkeletalShadowCasterRenderMethod;

		shared_ptr<CSkeletalMesh> pSkeletalMesh
			= dynamic_pointer_cast<CSkeletalMesh,CBasicMesh>(pMesh);

//		if( !this->m_MeshProperty.m_pBlendTransformsLoader )
//			InitShadowCasterReceiverSettings( pSkeletalMesh, this->m_MeshProperty );

		this->m_MeshProperty.m_pBlendTransformsLoader->SetSkeletalMesh( pSkeletalMesh );

//		pSkeletalMesh->SetLocalTransformToCache( 0, Matrix34Identity() );
//		pSkeletalMesh->SetLocalTransformsFromCache();
	}
	else
		pMeshRenderMethod = this->m_MeshProperty.m_pShadowCasterRenderMethod;

	// Do these settings in advance
	// - Notify if the shadow map manager is changed.
	if( true )//render_all_subsets )
	{
//		pMeshRenderMethod->MeshRenderMethod().resize( 1 );
		CSubsetRenderMethod& mesh_render_method = pMeshRenderMethod->MeshRenderMethod()[0];
		mesh_render_method.m_Shader    = pShadowMgr->GetShader();
//		const char *tech = is_skeletal_mesh ? "ShadowMap_VertexBlend" : "ShadowMap";
//		mesh_render_method.m_Technique.SetTechniqueName( tech );
		CVertexBlendType::Name blend_type = is_skeletal_mesh ? CVertexBlendType::QUATERNION_AND_VECTOR3 : CVertexBlendType::NONE;
		mesh_render_method.m_Technique = pShadowMgr->ShaderTechniqueForShadowCaster( blend_type );
//		pMeshRenderMethod->MeshRenderMethod().resize( 1 );
//		pMeshRenderMethod.SetMeshRenderMethod( mesh_render_method, 0 );
		pMeshRenderMethod->RenderMesh( pCopyEnt->m_MeshHandle, pCopyEnt->GetWorldPose() );
	}
	else
	{
		// how to render the mesh if each subset is rendered by different render methods
	}*/
}


void CBaseEntity::RenderAsShadowReceiver(CCopyEntity* pCopyEnt)
{
	ONCE( LOG_PRINT_ERROR( " Not implemented yet." ) );

	PROFILE_FUNCTION();

	// set option to disable texture settings

	CShadowMapManager *pShadowMgr = m_pStage->GetEntitySet()->GetRenderManager()->GetShadowManager();
	if( !pShadowMgr )
		return;

	shared_ptr<CBasicMesh> pMesh = pCopyEnt->m_MeshHandle.GetMesh();
	if( !pMesh )
		return;

	CShaderHandle shader = pShadowMgr->GetShader();
	CShaderManager *pShaderMgr = shader.GetShaderManager();
	if( !pShaderMgr )
		return;

	pShaderMgr->SetWorldTransform( pCopyEnt->GetWorldPose() );

	CShaderTechniqueHandle tech;
	const U32 entity_flags = pCopyEnt->GetEntityFlags();
	if( entity_flags & BETYPE_SHADOW_RECEIVER )
		tech = pShadowMgr->ShaderTechniqueForShadowReceiver( CVertexBlendType::NONE );
	else // i.e. entity_flags & BETYPE_SHADOW_CASTER. See CEntityNode::RenderShadowReceivers() for details.
		tech = pShadowMgr->ShaderTechniqueForNonShadowedCasters( CVertexBlendType::NONE );

	pShaderMgr->SetTechnique( tech );

	pMesh->Render( *pShaderMgr );

/*	shared_ptr<CMeshContainerRenderMethod> pMeshRenderMethod;
	const bool is_skeletal_mesh = (pMesh->GetMeshType() == CMeshType::SKELETAL);
	if( is_skeletal_mesh )
	{
		pMeshRenderMethod = this->m_MeshProperty.m_pSkeletalShadowReceiverRenderMethod;

		shared_ptr<CSkeletalMesh> pSkeletalMesh
			= dynamic_pointer_cast<CSkeletalMesh,CBasicMesh>(pMesh);

//		if( !this->m_MeshProperty.m_pBlendTransformsLoader )
//			InitShadowCasterReceiverSettings( pSkeletalMesh, this->m_MeshProperty );

		this->m_MeshProperty.m_pBlendTransformsLoader->SetSkeletalMesh( pSkeletalMesh );

//		pSkeletalMesh->SetLocalTransformToCache( 0, Matrix34Identity() );
//		pSkeletalMesh->SetLocalTransformsFromCache();
	}
	else
		pMeshRenderMethod = this->m_MeshProperty.m_pShadowReceiverRenderMethod;

	if( true )//render_all_subsets )
	{
//		pMeshRenderMethod->MeshRenderMethod().resize( 1 );
		CSubsetRenderMethod& mesh_render_method = pMeshRenderMethod->MeshRenderMethod()[0];
		mesh_render_method.m_Shader    = pShadowMgr->GetShader();
//		const char *tech = is_skeletal_mesh ? "SceneShadowMap_VertexBlend" : "SceneShadowMap";
//		mesh_render_method.m_Technique.SetTechniqueName( tech );
		CVertexBlendType::Name blend_type = is_skeletal_mesh ? CVertexBlendType::QUATERNION_AND_VECTOR3 : CVertexBlendType::NONE;
		mesh_render_method.m_Technique = pShadowMgr->ShaderTechniqueForShadowReceiver( blend_type );
//		render_method.SetMeshRenderMethod( mesh_render_method, 0 );
		pMeshRenderMethod->RenderMesh( pCopyEnt->m_MeshHandle, pCopyEnt->GetWorldPose() );
	}
	else
	{
		// how to render the mesh if each subset is rendered by different render methods
	}
*/
}


void CBaseEntity::RenderAs( CCopyEntity& entity, CRenderContext& render_context )
{
/*	CRenderContext rc;

	rc.SetShaderTechnique( CRenderContext::ROT_3D_MODEL );

	CShaderManager& shader_mgr = rc.GetShaderManager( CRenderContext::ROT_3D_MODEL );

	shader_mgr.SetWorldTransform( entity.GetWorldPose() );

	shared_ptr<CBasicMesh> pMesh = entity.m_MeshHandle.GetMesh();
	if( pMesh )
		pMesh->Render( shader_mgr );*/
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
// - CBlendTransformsLoader
//   - Set if pEntity->m_MeshHandle is a skeletal mesh
void InitMeshRenderMethod( CCopyEntity &entity, shared_ptr<CBlendTransformsLoader> pBlendTransformsLoader )
{
	if( !entity.m_pMeshRenderMethod )
	{
		entity.m_pMeshRenderMethod.reset( new CMeshContainerRenderMethod );
//		entity.m_pMeshRenderMethod->MeshRenderMethod().resize( 1 );
	}

	if( entity.GetEntityFlags() & BETYPE_LIGHTING )
	{
		shared_ptr<CEntityShaderLightParamsLoader> pLightParamsLoader( new CEntityShaderLightParamsLoader() );
		pLightParamsLoader->SetEntity( entity.Self() );
		entity.m_pMeshRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( pLightParamsLoader );
	}

	// Not used now.
	// Item entities set this in its own member function CItemEntity::InitMesh().
	// Does any entity other than item entty need this?
	if( pBlendTransformsLoader )
		entity.m_pMeshRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( pBlendTransformsLoader );
/*
	shared_ptr<CBasicMesh> pMesh = entity.m_MeshHandle.GetMesh();
	if( pMesh && pMesh->GetMeshType() == CMeshType::SKELETAL )
	{
//		shared_ptr<CSkeletalMesh> pSkeletalMesh
//			= boost::dynamic_pointer_cast<CSkeletalMesh,CBasicMesh>(pMesh);

		if( !pBlendTransformsLoader )
			pBlendTransformsLoader.reset( new CBlendTransformsLoader() );

		entity.m_pMeshRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( pBlendTransformsLoader );
	}*/

	if( true /* world position of entity has large values */ )
	{
		entity.m_pMeshRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( sg_pWorldTransLoader );
	}
}


Result::Name RegisterAsPlanarMirror( CCopyEntity& entity, CBasicMesh& mesh, int subset_index )
{
	const AABB3& aabb = mesh.GetAABB(subset_index);

	CEntityRenderManager& entity_render_mgr
		= *(entity.GetStage()->GetEntitySet()->GetRenderManager());

	// >>> TODO: support planes that are not axis-aligned or facing along the negative half-space
	SPlane plane;
	int plane_axis = 1;
	if(      aabb.vMax.x - aabb.vMin.x < 0.001f ) plane_axis = 0;
	else if( aabb.vMax.y - aabb.vMin.y < 0.001f ) plane_axis = 1;
	else if( aabb.vMax.z - aabb.vMin.z < 0.001f ) plane_axis = 2;
	else plane_axis = 1;

	plane.normal = Vector3(0,0,0);
	plane.normal[plane_axis] = 1;
	plane.dist = aabb.vMax[plane_axis];

	Result::Name res = entity_render_mgr.AddPlanarReflector( CEntityHandle<>( entity.Self() ), plane );

	if( res != Result::SUCCESS )
		return Result::UNKNOWN_ERROR;

	entity.RaiseEntityFlags( BETYPE_PLANAR_REFLECTOR );

	// Create shader variable loader for mirror 

	if( !entity.m_pMeshRenderMethod )
	{
		if( entity.pBaseEntity->MeshProperty().m_pMeshRenderMethod )
		{
			entity.m_pMeshRenderMethod
				= entity.pBaseEntity->MeshProperty().m_pMeshRenderMethod->CreateCopy();

			if( !entity.m_pMeshRenderMethod )
				return Result::UNKNOWN_ERROR;
		}
		else
			return Result::UNKNOWN_ERROR;
	}

	// create a planar reflection entity
//	shared_ptr<CMeshContainerRenderMethod> pMeshRenderMethodCopy
//		= entity.m_pMeshRenderMethod->CreateCopy();

	shared_ptr<CMirroredSceneTextureParam> pTexParam;
	pTexParam.reset( new CMirroredSceneTextureParam( CEntityHandle<>( entity.Self() ) ) );
	pTexParam->m_fReflection = mesh.GetMaterial(subset_index).m_Mat.fReflection;
//	pMeshRenderMethodCopy->SetShaderParamsLoaderToAllMeshRenderMethods( pTexParam );

	// test - we assume that the entity's mesh is composed of polygons that belong to a single plane.
	entity.m_pMeshRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( pTexParam );

	// Move the indices of the planar reflection subset(s)
	// to the render method of the planar reflection entity

	return Result::SUCCESS;
}


bool RegisterAsMirrorIfReflective( CCopyEntity& entity, CBasicMesh& mesh, int subset_index, CGenericShaderDesc& shader_desc )
{
	if( mesh.GetMaterial(subset_index).m_Mat.fReflection < 0.001f )
		return false;

	const CMeshMaterial& mat = mesh.GetMaterial(subset_index);
	const AABB3& aabb = mesh.GetAABB(subset_index);

	bool subset_triangles_on_plane = false;
	if( aabb.vMax.x - aabb.vMin.x < 0.001f
	 || aabb.vMax.y - aabb.vMin.y < 0.001f
	 || aabb.vMax.z - aabb.vMin.z < 0.001f )
	{
		// The triangles of the i-th subset is on an axis-aligned plane.
		subset_triangles_on_plane = true;
	}
//	else if( IsSubsetTrianglesOnSinglePlane(pMesh,i) )
//	{
//		subset_triangles_on_plane = true;
//	}

	if( subset_triangles_on_plane )
	{
		RegisterAsPlanarMirror( entity, mesh, subset_index );
		shader_desc.PlanarReflection = CPlanarReflectionOption::FLAT;
		return true;
	}
	else
	{
		// TODO: Register as an envmap target?
//		entity.RaiseEntityFlags( BETYPE_ENVMAPTARGET );
//		shader_desc.EnvMap = CEnvMapOption::ENABLED;
		return false;
	}

	return false;
}


// Creates a default render method for the entity.
// - If the lighting is enabled for the entity, a light parameter loader is set
// - If the mesh of the entity is skeletal, a blend transforms loader is set.
void CreateMeshRenderMethod( CEntityHandle<>& entity, 
							 CShaderHandle& shader,
							 CShaderTechniqueHandle& tech )
{
	shared_ptr<CCopyEntity> pEntity = entity.Get();

	if( !pEntity )
		return;

	// create mesh render method from shader and shader technique of the base entity

	pEntity->m_pMeshRenderMethod.reset( new CMeshContainerRenderMethod() );

	CSubsetRenderMethod& render_method = pEntity->m_pMeshRenderMethod->PrimaryMeshRenderMethod();
	render_method.m_Shader    = shader;
	render_method.m_Technique = tech;

	InitMeshRenderMethod( *pEntity );
}


// Initializes CCopyEntity::m_pMeshRenderMethod
// - Initialize the mesh render method
// - Initialize shader parameter loaders
// - Create alpha entities
// - Creates a shader
void CBaseEntity::InitEntityGraphics( CCopyEntity &entity,
                                      CShaderHandle& shader,
                                      CShaderTechniqueHandle& tech )
{
	if( shader.IsLoaded()
	 && tech.GetTechniqueName()
	 && 0 < strlen(tech.GetTechniqueName()) )
	{
		CreateMeshRenderMethod( CEntityHandle<>( entity.Self() ), shader, tech );
	}
	else
	{
		InitMeshRenderMethod( entity );
	}

	// create transparent parts of the model as separate entities
	if( m_EntityFlag & BETYPE_SUPPORT_TRANSPARENT_PARTS )
	{
		// Remove any previous alpha entities
		int next_child_index = 0;
		while( next_child_index < entity.GetNumChildren() )
//		for( int i=0; i<entity.GetNumChildren(); i++ )
		{
			shared_ptr<CCopyEntity> pChild = entity.m_aChild[next_child_index].Get();
			if( IsValidEntity(pChild.get())
			 && pChild->GetEntityTypeID() == CCopyEntityTypeID::ALPHA_ENTITY )
			{
				CCopyEntity *pChildRawPtr = pChild.get();
				m_pStage->TerminateEntity( pChildRawPtr );
			}
			else
				next_child_index += 1;
		}

		CreateAlphaEntities( &entity );
	}

	shared_ptr<CBasicMesh> pMesh = entity.m_MeshHandle.GetMesh();
	if( !pMesh )
		return;

	CBasicMesh& mesh = *pMesh;
	const int num_mesh_materials = mesh.GetNumMaterials();

	std::vector<CGenericShaderDesc> shader_descs;
	shader_descs.resize( num_mesh_materials );

	std::vector<int> mirror_subsets_indices;
//	std::vector<int> non_mirror_subsets_indices;

	if( m_MeshProperty.m_MeshDesc.IsValid() )
//	if( true )
	{
		// The mesh is specified in the base entity
	}
	else
	{
		// base entity has no mesh
		// - entity's mesh is individual
		shader_descs.resize( num_mesh_materials );
		for( int i=0; i<num_mesh_materials; i++ )
		{
			// Fill out the shader desc based on the parameter values of the material

			// reflection
			bool registered_as_mirror = RegisterAsMirrorIfReflective( entity, mesh, i, shader_descs[i] );
			if( registered_as_mirror )
				mirror_subsets_indices.push_back( i );

			// specularity
			if( 0.001f < mesh.GetMaterial(i).m_Mat.fSpecularity )
				shader_descs[i].Specular = CSpecularSource::UNIFORM;
			else
				shader_descs[i].Specular = CSpecularSource::NONE;
		}

		vector< pair< CGenericShaderDesc, vector<unsigned int> > > grouped_descs;
		group_elements( shader_descs, grouped_descs );

		// Do a NULL check just in case
		// The mesh render method is initialized by InitMeshRenderMethod() above.
		if( !entity.m_pMeshRenderMethod )
			entity.m_pMeshRenderMethod.reset( new CMeshContainerRenderMethod );

		bool shader_loaded = false;
		if( grouped_descs.size() == 1 )
		{
			CSubsetRenderMethod& render_method = entity.m_pMeshRenderMethod->PrimaryMeshRenderMethod();

			render_method.m_Technique.SetTechniqueName( "Default" );
			render_method.m_ShaderDesc.pShaderGenerator.reset( new CGenericShaderGenerator( grouped_descs[0].first ) );

//			shader_loaded = render_method.Load();
			shader_loaded = render_method.m_Shader.Load( render_method.m_ShaderDesc );
		}
		else
		{
			LOG_PRINT_WARNING( "Mesh materials need different shaders. This situation is not supported yet (the total number of materials: " + to_string(num_mesh_materials) + ")." );
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


} // namespace amorphous
