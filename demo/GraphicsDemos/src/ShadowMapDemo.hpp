#ifndef  __ShadowMapDemo_HPP__
#define  __ShadowMapDemo_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Graphics/ShadowMapManager.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class ShadowMapDemoSceneRenderer;


class ShadowMapDemo : public CGraphicsTestBase
{
	bool m_RenderSceneWithShadow;

	MeshHandle m_SkyboxMesh;

	MeshHandle m_Mesh;

	MeshHandle m_FloorMesh;

	ShaderTechniqueHandle m_MeshTechnique;

	ShaderTechniqueHandle m_DefaultTechnique;

	bool m_Lighting;

	ShaderHandle m_NoLightingShader;

	ShaderHandle m_Shader;

	std::shared_ptr<ShadowMapManager> m_pShadowMapManager;

	std::vector< std::pair< int, std::shared_ptr<Light> > > m_pLights;

	std::shared_ptr<ShadowMapDemoSceneRenderer> m_pShadowMapSceneRenderer;

private:

	void RenderScene( ShaderManager& shader_mgr );

	void RenderSceneWithShadow();

public:

	ShadowMapDemo();

	~ShadowMapDemo();

	const char *GetAppTitle() const { return "ShadowMapDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void HandleInput( const InputData& input );

	void RenderShadowCasters( Camera& camera, ShaderHandle *shaders, ShaderTechniqueHandle *shader_techniques );

	void RenderShadowReceivers( Camera& camera, ShaderHandle *shaders, ShaderTechniqueHandle *shader_techniques );
};


class ShadowMapDemoSceneRenderer : public ShadowMapSceneRenderer
{
	ShadowMapDemo *m_pTarget;

public:

	ShadowMapDemoSceneRenderer( ShadowMapDemo *pTarget ) : m_pTarget(pTarget) {}

	void RenderSceneToShadowMap( Camera& camera, ShaderHandle *shaders, ShaderTechniqueHandle *shader_techniques ) { m_pTarget->RenderShadowCasters( camera, shaders, shader_techniques ); }

	void RenderShadowReceivers( Camera& camera, ShaderHandle *shaders, ShaderTechniqueHandle *shader_techniques ) { m_pTarget->RenderShadowReceivers( camera, shaders, shader_techniques ); }
};



#endif /* __ShadowMapDemo_HPP__ */
