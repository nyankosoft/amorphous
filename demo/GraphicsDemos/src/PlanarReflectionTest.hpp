#ifndef  __PlanarReflectionTest_HPP__
#define  __PlanarReflectionTest_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/GraphicsDevice.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CPlanarReflectionTest : public CGraphicsTestBase
{
	MeshHandle m_SkyboxMesh;

//	MeshHandle m_TerrainMesh;

	std::vector<MeshHandle> m_ReflectionSourceMeshes;

	std::vector<MeshHandle> m_ReflectiveSurfaceMeshes;

	ShaderTechniqueHandle m_MeshTechnique;

//	ShaderTechniqueHandle m_SkyboxTechnique;

	ShaderTechniqueHandle m_DefaultTechnique;

	ShaderHandle m_Shader;

	GenericShaderDesc m_PlanarReflectionShaderDesc;

	ShaderHandle m_PlanarReflectionShader;

	TextureHandle m_PerturbationTexture;

	boost::shared_ptr<TextureRenderTarget> m_pTextureRenderTarget;

	float m_fReflection;

	bool m_Perturbation;

	bool m_RenderSkybox;

	bool m_RenderMirroredScene;

protected:

	PlanarReflectionOption::Name GetReflectionType();

	void RenderReflectionSourceMeshes( const Matrix34& camera_pose, CullingMode::Name culling_mode );

	void RenderMirroredSceneToTextureRenderTarget();

	void RenderReflectionSurface();

	void UpdateLight( ShaderManager& shader_mgr );

	void SetLowLight( ShaderManager& shader_mgr );

	void UpdateReflection();

public:

	CPlanarReflectionTest();

	~CPlanarReflectionTest();

	const char *GetAppTitle() const { return "PlanarReflectionTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void UpdateViewTransform( const Matrix44& matView );

	void UpdateProjectionTransform( const Matrix44& matProj );

	void HandleInput( const InputData& input );
};


#endif /* __PlanarReflectionTest_HPP__ */
