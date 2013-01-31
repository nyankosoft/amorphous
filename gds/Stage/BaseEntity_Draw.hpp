#ifndef  __BaseEntity_Draw_HPP__
#define  __BaseEntity_Draw_HPP__


#include "EntityHandle.hpp"
#include "Stage.hpp"
#include "EntityRenderManager.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/MeshContainerRenderMethod.hpp"
#include "Graphics/Mesh/SkeletalMesh.hpp"


namespace amorphous
{


extern void SetLightsToShader( CCopyEntity& entity,  ShaderManager& rShaderMgr );

extern void InitMeshRenderMethod( CCopyEntity &entity,
								  boost::shared_ptr<CBlendTransformsLoader> pBlendTransformsLoader = boost::shared_ptr<CBlendTransformsLoader>() );

extern void CreateMeshRenderMethod( CEntityHandle<>& entity, 
							        ShaderHandle& shader,
							        ShaderTechniqueHandle& tech );

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

	void UpdateShaderParams( ShaderManager& rShaderMgr )
	{
		boost::shared_ptr<CCopyEntity> pEntity = m_Entity.Get();
		if( pEntity )
			SetLightsToShader( *(pEntity.get()), rShaderMgr );
	}
};


} // namespace amorphous



#endif  /*  __BaseEntity_Draw_HPP__  */
