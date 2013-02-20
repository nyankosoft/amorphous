#ifndef  __PlanarReflectionTest_HPP__
#define  __PlanarReflectionTest_HPP__


#include "gds/3DMath/Vector3.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Graphics/GraphicsDevice.hpp"
#include "gds/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CPlanarReflectionTest : public CGraphicsTestBase
{
	MeshHandle m_SkyboxMesh;

	MeshHandle m_TerrainMesh;

	std::vector<MeshHandle> m_ReflectionSourceMeshes;

	std::vector<MeshHandle> m_ReflectiveSurfaceMeshes;

	ShaderTechniqueHandle m_MeshTechnique;

//	ShaderTechniqueHandle m_SkyboxTechnique;

	ShaderTechniqueHandle m_DefaultTechnique;

	ShaderHandle m_Shader;

	ShaderHandle m_PlanarReflectionShader;

	TextureHandle m_PerturbationTexture;

	boost::shared_ptr<FontBase> m_pFont;

	boost::shared_ptr<TextureRenderTarget> m_pTextureRenderTarget;

protected:

	void RenderReflectionSourceMeshes( const Matrix34& camera_pose, CullingMode::Name culling_mode );

	void RenderReflectionSurface();

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

//	void OnKeyPressed( KeyCode::Code key_code );
};


#endif /* __PlanarReflectionTest_HPP__ */
