#ifndef  __BaseEntity_Draw_HPP__
#define  __BaseEntity_Draw_HPP__


#include "EntityHandle.hpp"
#include "Stage.hpp"
#include "EntityRenderManager.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/MeshContainerRenderMethod.hpp"
#include "amorphous/Graphics/Mesh/SkeletalMesh.hpp"


namespace amorphous
{


extern void SetLightsToShader( CCopyEntity& entity,  ShaderManager& rShaderMgr );

extern void InitMeshRenderMethod( CCopyEntity &entity,
								  std::shared_ptr<BlendTransformsLoader> pBlendTransformsLoader = std::shared_ptr<BlendTransformsLoader>() );

extern void CreateMeshRenderMethod( EntityHandle<>& entity, 
							        ShaderHandle& shader,
							        ShaderTechniqueHandle& tech );

extern void UpdateEntityForRendering( CCopyEntity& entity );


class CEntityShaderLightParamsLoader : public ShaderParamsLoader
{
//	std::shared_ptr<CCopyEntity> m_pEntity;
	EntityHandle<> m_Entity;

public:

	CEntityShaderLightParamsLoader( std::shared_ptr<CCopyEntity> pEntity = std::shared_ptr<CCopyEntity>() )
		:
	m_Entity(pEntity)
	{}

//	void SetEntity( std::shared_ptr<CCopyEntity> pEntity ) { m_Entity = EntityHandle<>( pEntity ); }
	void SetEntity( std::weak_ptr<CCopyEntity> pEntity ) { m_Entity = EntityHandle<>( pEntity ); }

	void UpdateShaderParams( ShaderManager& rShaderMgr )
	{
		std::shared_ptr<CCopyEntity> pEntity = m_Entity.Get();
		if( pEntity )
			SetLightsToShader( *(pEntity.get()), rShaderMgr );
	}
};


} // namespace amorphous



#endif  /*  __BaseEntity_Draw_HPP__  */
