#ifndef  __BaseEntity_Draw_HPP__
#define  __BaseEntity_Draw_HPP__


#include "EntityHandle.hpp"
#include "Stage.hpp"
#include "EntityRenderManager.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/MeshContainerRenderMethod.hpp"
#include "Graphics/Mesh/SkeletalMesh.hpp"


extern void SetLightsToShader( CCopyEntity& entity,  CShaderManager& rShaderMgr );

extern void InitMeshRenderMethod( CCopyEntity &entity );

extern void CreateMeshRenderMethod( CEntityHandle<>& entity, 
							        CShaderHandle& shader,
							        CShaderTechniqueHandle& tech );

extern void UpdateEntityForRendering( CCopyEntity& entity );


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


class CBlendMatricesLoader : public CShaderParamsLoader
{
	boost::shared_ptr<CSkeletalMesh> m_pSkeletalMesh;

public:

	CBlendMatricesLoader( boost::shared_ptr<CSkeletalMesh> pSkeletalMesh = boost::shared_ptr<CSkeletalMesh>() )
		:
	m_pSkeletalMesh(pSkeletalMesh)
	{}

	void SetSkeletalMesh( boost::shared_ptr<CSkeletalMesh> pSkeletalMesh ) { m_pSkeletalMesh = pSkeletalMesh; }

	void UpdateShaderParams( CShaderManager& rShaderMgr )
	{
		//PROFILE_FUNCTION();

		// set identity matrix to the root bone since 
		m_pSkeletalMesh->SetLocalTransformToCache( 0, Matrix34Identity() );//	< usu.?

		m_pSkeletalMesh->SetLocalTransformsFromCache();

		// set blend matrices to the shader
		LPD3DXEFFECT pEffect = rShaderMgr.GetEffect();
		D3DXMATRIX *paBlendMatrix = m_pSkeletalMesh->GetBlendMatrices();
		if( pEffect && paBlendMatrix )
		{
			HRESULT hr;
			char acParam[32];
			int i, num_bones = m_pSkeletalMesh->GetNumBones();

			for( i=0; i<num_bones; i++ )
			{
				sprintf( acParam, "g_aBlendMatrix[%d]", i );
				hr = pEffect->SetMatrix( acParam, &paBlendMatrix[i] );

				if( FAILED(hr) ) return;
			}
		}
	}
};


#endif  /*  __BaseEntity_Draw_HPP__  */
