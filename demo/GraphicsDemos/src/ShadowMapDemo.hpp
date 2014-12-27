#ifndef  __ShadowMapDemo_HPP__
#define  __ShadowMapDemo_HPP__


#include "amorphous/3DMath/Vector3.hpp"
#include "amorphous/Graphics/fwd.hpp"
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
	MeshHandle m_SkyboxMesh;

	MeshHandle m_Mesh;

	MeshHandle m_FloorMesh;

	ShaderTechniqueHandle m_MeshTechnique;

//	ShaderTechniqueHandle m_SkyboxTechnique;

	ShaderTechniqueHandle m_DefaultTechnique;

	ShaderHandle m_Shader;

	boost::shared_ptr<ShadowMapManager> m_pShadowMapManager;

	boost::shared_ptr<Light> m_pLight;

	boost::shared_ptr<ShadowMapDemoSceneRenderer> m_pShadowMapSceneRenderer;

private:

	void RenderScene( ShaderManager& shader_mgr );

public:

	ShadowMapDemo();

	~ShadowMapDemo();

	const char *GetAppTitle() const { return "ShadowMapDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void HandleInput( const InputData& input );

	void RenderShadowCasters( Camera& camera );

	void RenderShadowReceivers( Camera& camera );
};


class ShadowMapDemoSceneRenderer : public ShadowMapSceneRenderer
{
	ShadowMapDemo *m_pTarget;

public:

	ShadowMapDemoSceneRenderer( ShadowMapDemo *pTarget ) : m_pTarget(pTarget) {}

	void RenderSceneToShadowMap( Camera& camera ) { m_pTarget->RenderShadowCasters( camera ); }

	void RenderShadowReceivers( Camera& camera ) { m_pTarget->RenderShadowReceivers( camera ); }
};



#endif /* __ShadowMapDemo_HPP__ */
