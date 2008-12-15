#include "BaseEntity.h"
#include "BaseEntity_Draw.h"
#include "CopyEntity.h"
#include "Stage.h"
#include "ScreenEffectManager.h"

#include "3DCommon/Camera.h"
#include "3DCommon/D3DXMeshObject.h"
#include "3DCommon/D3DXSMeshObject.h"
#include "3DCommon/Shader/Shader.h"
#include "3DCommon/Shader/ShaderManager.h"

#include "Support/Profile.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Macro.h"

using namespace std;
using namespace boost;


/*
int CBaseEntity::GetRenderMode()
{
	unsigned int effect_flag = m_pStage->GetScreenEffectManager()->GetEffectFlag();

	if( effect_flag & ScreenEffect::ShadowMap )
		return ERM_SMAP;	// shadowmap
	if( effect_flag & ScreenEffect::CubeEnvMap )
		return ERM_ENVMAP;	// cubic environment mapping
//	else if( effect_flag & ScreenEffect::SceneDepthMap )
//		return ERM_SDMAP;
	else
		return ERM_NORMAL;
}
*/

static D3DXVECTOR3 s_OrigViewTrans;
static D3DXVECTOR3 s_OrigWorldPos;

static void SetOffsetWorldTransform( CShaderManager *pShaderManager, CCamera *pCamera )
{
	D3DXMATRIX matWorld, matView;
	pShaderManager->GetWorldTransform( matWorld );
	pShaderManager->GetViewTransform( matView );
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

	pShaderManager->SetWorldTransform( matWorld );
	pShaderManager->SetViewTransform( matView );
}


static void RestoreOffsetWorldTransform( CShaderManager *pShaderManager )
{
	D3DXMATRIX matWorld, matView;
	pShaderManager->GetWorldTransform( matWorld );
	pShaderManager->GetViewTransform( matView );

	matWorld._41 = s_OrigWorldPos.x;
	matWorld._42 = s_OrigWorldPos.y;
	matWorld._43 = s_OrigWorldPos.z;

	matView._41 = s_OrigViewTrans.x;
	matView._42 = s_OrigViewTrans.y;
	matView._43 = s_OrigViewTrans.z;

	pShaderManager->SetWorldTransform( matWorld );
	pShaderManager->SetViewTransform( matView );
}


static void SetOffsetWorldTransform( CCamera *pCamera )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	D3DXMATRIX matWorld, matView;
	pd3dDev->GetTransform( D3DTS_WORLD, &matWorld );
	pd3dDev->GetTransform( D3DTS_VIEW, &matView );
	s_OrigWorldPos = D3DXVECTOR3(matWorld._41,matWorld._42,matWorld._43);
	s_OrigViewTrans = D3DXVECTOR3(matView._41,matView._42,matView._43);

	if( !pCamera )
		return;

    matView._41 = matView._42 = matView._43 = 0;
	
	D3DXVECTOR3 cam_pos = pCamera->GetPosition();
	matWorld._41 = s_OrigWorldPos.x - cam_pos.x;
	matWorld._42 = s_OrigWorldPos.y - cam_pos.y;
	matWorld._43 = s_OrigWorldPos.z - cam_pos.z;

	pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );
	pd3dDev->SetTransform( D3DTS_VIEW, &matView );
}


static void RestoreOffsetWorldTransform()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	D3DXMATRIX matWorld, matView;
	pd3dDev->GetTransform( D3DTS_WORLD, &matWorld );
	pd3dDev->GetTransform( D3DTS_VIEW, &matView );

	matWorld._41 = s_OrigWorldPos.x;
	matWorld._42 = s_OrigWorldPos.y;
	matWorld._43 = s_OrigWorldPos.z;

	matView._41 = s_OrigViewTrans.x;
	matView._42 = s_OrigViewTrans.y;
	matView._43 = s_OrigViewTrans.z;

	pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );
	pd3dDev->SetTransform( D3DTS_VIEW, &matView );
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

	D3DXMATRIX matWorld;
	world_pose.GetRowMajorMatrix44( (float *)&matWorld );

	if( !pMeshObject )
	{
		ONCE( LOG_PRINT_ERROR( "An invalid mesh object: base entity: " + m_strName ) );
		return;
	}

	LPD3DXBASEMESH pMesh = pMeshObject->GetBaseMesh();
	if( !pMesh )
		return;

//	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	CShaderManager *pShaderManager = NULL;

	// set the world transform
	pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );

	pd3dDev->SetVertexDeclaration( pMeshObject->GetVertexDeclaration() );

    int i, num_materials = pMeshObject->GetNumMaterials();

	bool use_offset_world_transform = true;

	bool bSingleTechnique
		= ( m_MeshProperty.m_PropertyFlags & CBE_MeshObjectProperty::PF_USE_SINGLE_TECHNIQUE_FOR_ALL_MATERIALS );

	if( pShaderManager = CShader::Get()->GetCurrentShaderManager() )
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
	}
	else
	{
//		SetOffsetWorldTransform( m_pStage->GetCurrentCamera() );

		pd3dDev->SetVertexShader( NULL );

/*		if( m_pStage->GetScreenEffectManager()->GetEffectFlag() & ScreenEffect::PSEUDO_NIGHT_VISION )
			... ;
		else
			... ;
*/

		// Meshes are divided into subsets by materials. Render each subset in a loop
		for( i=0; i<num_materials; i++ )
		{
			// Set the material and texture for this subset
	//		pd3dDev->SetMaterial( &pMeshObject->GetMaterial(i) );

			const int num_textures_per_material = pMeshObject->GetNumTextures( i );
			for( int tex=0; tex<num_textures_per_material; tex++ )
				pd3dDev->SetTexture( tex, pMeshObject->GetTexture(i,tex).GetTexture() );

			// Draw the mesh subset
			pMesh->DrawSubset( i );
		}

//		RestoreOffsetWorldTransform();

	}
}


//void CBaseEntity::Draw3DModel( CCopyEntity* pCopyEnt, int shader_tech_id )
void CBaseEntity::Draw3DModel( CCopyEntity* pCopyEnt,
							   C2DArray<CShaderTechniqueHandle>& rShaderTechHandleTable,
							   int ShaderLOD )
{
	PROFILE_FUNCTION();

	CD3DXMeshObjectBase *pMeshObject;

	// first, see if the mesh is set to the copy entity
	pMeshObject = pCopyEnt->MeshObjectHandle.GetMesh().get();

	if( !pMeshObject )
		pMeshObject = m_MeshProperty.m_MeshObjectHandle.GetMesh().get(); // try the mesh stored in base entity

	if( !pMeshObject || !(pMeshObject->GetBaseMesh()) )
	{
		return;
	}

	if( rShaderTechHandleTable.size_x() == 0 )
	{
		// no shader technique to render the model with
		return;
	}

//	set render states and the world matrix for the fixed function pipeline

//	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// alpha-blending settings
	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA );
//	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	m_SrcAlphaBlend );
//	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	m_DestAlphaBlend );

	// set world transform
	D3DXMATRIX matWorld;
	pCopyEnt->GetWorldPose().GetRowMajorMatrix44( (float *)&matWorld );
	pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );

	CShaderManager *pShaderManager;
	if( pShaderManager = CShader::Get()->GetCurrentShaderManager() )
	{
		if( m_MeshProperty.m_SpecTex.GetTexture() )
//		if( 0 < m_MeshProperty.m_vecExtraTexture.size()
//		 && m_MeshProperty.m_vecExtraTexture[0].GetTexture() )
		{
			pShaderManager->SetTexture( 2, m_MeshProperty.m_SpecTex.GetTexture() );
//			pShaderManager->SetTexture( 2, m_MeshProperty.m_vecExtraTexture[0].GetTexture() );
		}

		if( pCopyEnt->EntityFlag & BETYPE_ENVMAPTARGET )
		{
			pShaderManager->SetCubeTexture( 0, m_pStage->GetEntitySet()->GetRenderManager()->GetEnvMapTexture(pCopyEnt->GetID()) );
		}
	}

	const CMeshType::Name mesh_type = pMeshObject->GetMeshType();

	switch( mesh_type )
	{
	case CMeshType::SKELETAL:
		DrawSkeletalMesh( pCopyEnt, dynamic_cast<CD3DXSMeshObject *>(pMeshObject), rShaderTechHandleTable, ShaderLOD );
		break;

	case CMeshType::BASIC:
		DrawMeshObject( pCopyEnt->GetWorldPose(),
			           m_MeshProperty.m_MeshObjectHandle.GetMesh().get(),
			           m_MeshProperty.m_vecTargetMaterialIndex,
					   rShaderTechHandleTable,
					   ShaderLOD );
		break;

	default:
		break;
	}
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

	SetBlendMatrices( pSkeletalMesh );

	DrawMeshObject( pCopyEnt->GetWorldPose(),
		            pSkeletalMesh,
					m_MeshProperty.m_vecTargetMaterialIndex,
					rShaderTechHandleTable,
					ShaderLOD );

	pSkeletalMesh->ResetLocalTransformsCache();

	return;
}


void SetBlendMatrices( CD3DXSMeshObject *pSMeshObject )
{
//	PROFILE_FUNCTION();

	// set blend matrices to the shader
	LPD3DXEFFECT pEffect;
	CShaderManager *pShaderMgr;
	D3DXMATRIX *paBlendMatrix = pSMeshObject->GetBlendMatrices();
	if( (pShaderMgr = CShader::Get()->GetCurrentShaderManager())
	 && (pEffect = pShaderMgr->GetEffect())
	 && paBlendMatrix )
	{
		HRESULT hr;
		char acParam[32];
		int i, num_bones = pSMeshObject->GetNumBones();

		for( i=0; i<num_bones; i++ )
		{
			sprintf( acParam, "g_aBlendMatrix[%d]", i );
			hr = pEffect->SetMatrix( acParam, &paBlendMatrix[i] );

			if( FAILED(hr) ) return;
		}
	}
}


void CBaseEntity::RenderAsShaderCaster(CCopyEntity* pCopyEnt)
{
	PROFILE_FUNCTION();

	CD3DXMeshObjectBase *pMeshObject = NULL;

	// first, see if the mesh is set to the copy entity
	pMeshObject = pCopyEnt->MeshObjectHandle.GetMesh().get();

	if( !pMeshObject )
		pMeshObject = m_MeshProperty.m_MeshObjectHandle.GetMesh().get(); // try the mesh stored in base entity

	if( !pMeshObject || !(pMeshObject->GetBaseMesh()) )
	{
		// no mesh object
		return;
	}

//	set render states and the world matrix for the fixed function pipeline

//	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// alpha-blending settings
	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA );
//	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	m_SrcAlphaBlend );
//	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	m_DestAlphaBlend );

	int mesh_type = m_MeshProperty.m_MeshObjectHandle.GetMeshType();

	switch( mesh_type )
	{
//	case CMeshType::SKELETAL:
//		DrawSkeletalMesh( pCopyEnt, rShaderTechHandleTable, ShaderLOD );
		break;

	case CMeshType::BASIC:
	case CMeshType::SKELETAL:
//		DrawMeshObject( pCopyEnt->GetWorldPose(), m_MeshProperty.m_MeshObjectHandle.GetMesh(), rShaderTechHandleTable, ShaderLOD );
	{
		LPD3DXBASEMESH pMesh = pMeshObject->GetBaseMesh();
		if( !pMesh )
			return;

		// set the world transform
		D3DXMATRIX matWorld;
		pCopyEnt->GetWorldPose().GetRowMajorMatrix44( (float *)&matWorld );
		pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );

		bool use_offset_world_transform = false;

//		CShadowMapManager *pShadowMgr = m_pStage->GetEntitySet()->GetRenderManager()->GetShadowManager();
		CShaderManager *pShaderManager = NULL;
		LPD3DXEFFECT pEffect = NULL;
		if( (pShaderManager = CShader::Get()->GetCurrentShaderManager()) &&
			(pEffect = pShaderManager->GetEffect()) )
		{
			// render the mesh with an HLSL shader

			pShaderManager->SetWorldTransform( matWorld );

			if( use_offset_world_transform )
				SetOffsetWorldTransform( pShaderManager, m_pStage->GetCurrentCamera() );

			HRESULT hr;
			hr = pEffect->SetTechnique( "ShadowMap" );

			// TODO: skip texture settings
			pMeshObject->Render( *pShaderManager );

			if( use_offset_world_transform )
				RestoreOffsetWorldTransform( pShaderManager );
		}
	}
		break;

	default:
		break;
	}
}


void CBaseEntity::RenderAsShaderReceiver(CCopyEntity* pCopyEnt)
{
	PROFILE_FUNCTION();

	CD3DXMeshObjectBase *pMeshObject = NULL;

	// first, see if the mesh is set to the copy entity
	pMeshObject = pCopyEnt->MeshObjectHandle.GetMesh().get();

	if( !pMeshObject )
		pMeshObject = m_MeshProperty.m_MeshObjectHandle.GetMesh().get(); // try the mesh stored in base entity

	if( !pMeshObject || !(pMeshObject->GetBaseMesh()) )
	{
		// no mesh object
		return;
	}

//	set render states and the world matrix for the fixed function pipeline

//	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// alpha-blending settings
	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA );
//	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	m_SrcAlphaBlend );
//	pd3dDev->SetRenderState( D3DRS_SRCBLEND,	m_DestAlphaBlend );

	int mesh_type = m_MeshProperty.m_MeshObjectHandle.GetMeshType();

	switch( mesh_type )
	{
//	case CMeshType::SKELETAL:
//		DrawSkeletalMesh( pCopyEnt, rShaderTechHandleTable, ShaderLOD );
		break;

	case CMeshType::BASIC:
	case CMeshType::SKELETAL:
//		DrawMeshObject( pCopyEnt->GetWorldPose(), m_MeshProperty.m_MeshObjectHandle.GetMesh(), rShaderTechHandleTable, ShaderLOD );
	{
		LPD3DXBASEMESH pMesh = pMeshObject->GetBaseMesh();
		if( !pMesh )
			return;

		// set the world transform
		D3DXMATRIX matWorld;
		pCopyEnt->GetWorldPose().GetRowMajorMatrix44( (float *)&matWorld );
		pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );

		bool use_offset_world_transform = false;

		CShadowMapManager *pShadowMgr = m_pStage->GetEntitySet()->GetRenderManager()->GetShadowManager();
		CShaderManager *pShaderManager = NULL;
		LPD3DXEFFECT pEffect = NULL;
		if( (pShaderManager = CShader::Get()->GetCurrentShaderManager()) &&
			(pEffect = pShaderManager->GetEffect()) )
		{
			// render the mesh with an HLSL shader

			pShaderManager->SetWorldTransform( matWorld );

			if( use_offset_world_transform )
				SetOffsetWorldTransform( pShaderManager, m_pStage->GetCurrentCamera() );

			pEffect->SetTechnique( "SceneShadowMap" );
//			m_pStage->GetEntitySet()->GetRenderManager()->SetShaderTechniqueForShadowMap();

			// TODO: skip texture settings
			pMeshObject->Render( *pShaderManager );

			if( use_offset_world_transform )
				RestoreOffsetWorldTransform( pShaderManager );
		}
	}
		break;

	default:
		break;
	}
}


/// Update light-related shader variables
void CBaseEntity::SetLightsToShader( CCopyEntity& entity )
{
	CShaderManager *pShaderMgr = NULL;
	shared_ptr<CShaderLightManager> pShaderLightMgr = pShaderMgr->GetShaderLightManager();

	int i, num_current_lights = entity.GetNumLights();
	CLightEntity *pLightEntity = NULL;

	CShaderLightParamsWriter light_params_writer( pShaderLightMgr.get() );

	for( i=0; i<num_current_lights; i++ )
	{
		CEntityHandle<CLightEntity>& light_entity = entity.GetLight( i );
		CLightEntity *pLightEntity = light_entity.GetRawPtr();
		if( !pLightEntity )
			continue;

//		pLightEntity->SetLightToShader( pShaderLightMgr );

		pLightEntity->GetLightObject()->Accept( light_params_writer );
	}

	pShaderLightMgr->CommitChanges();
}


void CBaseEntity::SetLights( CCopyEntity& entity )
{
	if( entity.Lighting() )
	{
		if( entity.sState & CESTATE_LIGHT_INFORMATION_INVALID )
		{
			// need to update light information - find lights that reaches to this entity
			entity.ClearLights();
//			m_pStage->GetEntitySet()->UpdateLightInfo( entity );
			entity.sState &= ~CESTATE_LIGHT_INFORMATION_INVALID;
		}

		SetLightsToShader( entity );

		// turn on lights that reach 'pCopyEnt'
//		m_pEntitySet->EnableLightForEntity();
//		m_pEntitySet->SetLightsForEntity( pEntity );
	}
	else
	{	// turn off lights
//		m_pEntitySet->DisableLightForEntity();
	}
}
