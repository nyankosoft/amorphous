#ifndef __MirroredSceneTextureParam_HPP__
#define __MirroredSceneTextureParam_HPP__


#include "CopyEntity.hpp"
#include "Stage.hpp"
#include "EntityRenderManager.hpp"
#include "../Graphics/MeshContainerRenderMethod.hpp"
#include "../Graphics/Shader/ShaderParameter.hpp"
#include "../Graphics/Shader/ShaderManager.hpp"


namespace amorphous
{


class MirroredSceneTextureParam : public ShaderParamsLoader
{
public:

	EntityHandle<> m_Entity;

	float m_fReflection;

public:

	MirroredSceneTextureParam( EntityHandle<> entity ) : m_Entity(entity), m_fReflection(0) {}

	~MirroredSceneTextureParam() {}

	void UpdateShaderParams( ShaderManager& rShaderMgr )
	{
		using std::shared_ptr;

		shared_ptr<CCopyEntity> pEntity = m_Entity.Get();
		if( !pEntity )
			return;

//		std::shared_ptr<CStage> pStage;
		CStage *pStage = pEntity->GetStage();

		shared_ptr<BasicMesh> pMesh = pEntity->m_MeshHandle.GetMesh();
		if( 0 < pMesh->GetNumMaterials() )
		{
			m_fReflection = pMesh->GetMaterial(0).m_Mat.fReflection;
			rShaderMgr.SetParam( "g_fPlanarReflection", m_fReflection );
		}

		std::shared_ptr<EntityRenderManager> pEntityRenderManager
			= pStage->GetEntitySet()->GetRenderManager();

		ShaderParameter<TextureParam> tex_param;

//		const int mirrored_scene_texture_stage = 3;
//		tex_param.Parameter().m_Stage = mirrored_scene_texture_stage;

		tex_param.SetParameterName( "PlanarMirrorTexture" );

		if( pEntityRenderManager->IsRenderingMirroredScene() )
		{
			tex_param.Parameter().m_Handle = TextureHandle();
		}
		else
		{
//			TextureHandle tex = pEntityRenderManager->GetPlanarReflectionTexture( *pEntity );
			tex_param.Parameter().m_Handle = pEntityRenderManager->GetPlanarReflectionTexture( *pEntity );
		}

		rShaderMgr.SetParam( tex_param );
	}
};


} // namespace amorphous



#endif /* __MirroredSceneTextureParam_HPP__ */
