#ifndef  __ShadowMapTest_HPP__
#define  __ShadowMapTest_HPP__


#include "gds/3DMath/Vector3.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Graphics/ShadowMapManager.hpp"
#include "gds/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CShadowMapTestSceneRenderer;


class CShadowMapTest : public CGraphicsTestBase
{
	CMeshObjectHandle m_SkyboxMesh;

	CMeshObjectHandle m_Mesh;

	CMeshObjectHandle m_FloorMesh;

	CShaderTechniqueHandle m_MeshTechnique;

//	CShaderTechniqueHandle m_SkyboxTechnique;

	CShaderTechniqueHandle m_DefaultTechnique;

	CShaderHandle m_Shader;

	boost::shared_ptr<CFontBase> m_pFont;

	boost::shared_ptr<CShadowMapManager> m_pShadowMapManager;

	boost::shared_ptr<CLight> m_pLight;

	boost::shared_ptr<CShadowMapTestSceneRenderer> m_pShadowMapSceneRenderer;

private:

	void RenderScene( CShaderManager& shader_mgr );

public:

	CShadowMapTest();

	~CShadowMapTest();

	const char *GetAppTitle() const { return "ShadowMapTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void HandleInput( const SInputData& input );

	void RenderShadowCasters( CCamera& camera );

	void RenderShadowReceivers( CCamera& camera );
};


class CShadowMapTestSceneRenderer : public CShadowMapSceneRenderer
{
	CShadowMapTest *m_pTarget;

public:

	CShadowMapTestSceneRenderer( CShadowMapTest *pTarget ) : m_pTarget(pTarget) {}

	void RenderSceneToShadowMap( CCamera& camera ) { m_pTarget->RenderShadowCasters( camera ); }

	void RenderShadowReceivers( CCamera& camera ) { m_pTarget->RenderShadowReceivers( camera ); }
};



#endif /* __ShadowMapTest_HPP__ */
