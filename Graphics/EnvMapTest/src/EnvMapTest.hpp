#ifndef  __EnvMapTest_HPP__
#define  __EnvMapTest_HPP__


#include <boost/shared_ptr.hpp>
#include "3DMath/Matrix34.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/CubeMapManager.hpp"
#include "Graphics/ShaderHandle.hpp"
#include "Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "Input.hpp"
//#include "Graphics/UnitSphere.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CEnvMapTest;


class CCubeMapTestRenderer : public CCubeMapSceneRenderer
{
	CEnvMapTest *m_pEnvMapTest;

public:

	CCubeMapTestRenderer(CEnvMapTest *pEnvMapTest) : m_pEnvMapTest(pEnvMapTest) {}

	virtual ~CCubeMapTestRenderer() {}

	virtual Vector3 GetCameraPosition() { return Vector3(0,1,0); }//g_Camera.GetPosition(); }

	/// Called by CCubeMapManager in CCubeMapManager::RenderToCubeMap()
	/// to render the scene to the cube map surface.
	/// - called 6 times in total to render the scene to all the cube map surfaces
	/// - IDirect3DDevice9::BeginScene() & EndScene() pair is not called
	///   in CCubeMapManager::RenderToCubeMap() before and after the 
	///   RenderSceneToCubeMap() calls.
	///   User must call them in this functions?
	///   - changed: BeginScene() and EndScene() are called in CCubeMapManager::RenderToCubeMap()
	virtual void RenderSceneToCubeMap( CCamera& camera );
};


class CEnvMapTest : public CGraphicsTestBase
{
	boost::shared_ptr<CCubeMapManager> m_pCubeMapManager;

	boost::shared_ptr<CCubeMapSceneRenderer> m_pCubeMapSceneRenderer;

	std::vector<CMeshObjectHandle> m_vecEnvMappedMesh;

	std::vector<CMeshObjectHandle> m_vecNonEnvMappedMesh;

	int m_CurrentEnvMapTarget;

	CShaderTechniqueHandle m_NonEnvMappedMesh;
	CShaderTechniqueHandle m_EnvMappedMesh;
//	CShaderTechniqueHandle m_SkyboxTechnique;
//	CShaderTechniqueHandle m_DefaultTechnique;

	CShaderHandle m_EnvMappedMeshShader;

	CShaderHandle m_NonEnvMappedMeshShader;

//	shared_ptr<CShaderLightManager> m_pShaderLightManager;

	bool m_UseEmbeddedShader;

	Vector3 m_vLightPosition;

	CFontSharedPtr m_pFont;

public:

	CEnvMapTest();

	~CEnvMapTest();

	int Init();

	void Release() {};

	void Update( float dt );

//	void Render();

	virtual void RenderBase();

	virtual void HandleInput( const SInputData& input );

	void RenderNonEnvMappedMeshes();

	void UpdateViewTransform( const Matrix44& matView );

	void UpdateProjectionTransform( const Matrix44& matProj );
};


#endif /* __EnvMapTest_HPP__ */
