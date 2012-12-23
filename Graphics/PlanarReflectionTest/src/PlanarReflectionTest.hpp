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

#include "../../../_Common/GraphicsTestBase.hpp"


class CPlanarReflectionTest : public CGraphicsTestBase
{
	CMeshObjectHandle m_SkyboxMesh;

	CMeshObjectHandle m_TerrainMesh;

	std::vector<CMeshObjectHandle> m_ReflectionSourceMeshes;

	std::vector<CMeshObjectHandle> m_ReflectiveSurfaceMeshes;

	CShaderTechniqueHandle m_MeshTechnique;

//	CShaderTechniqueHandle m_SkyboxTechnique;

	CShaderTechniqueHandle m_DefaultTechnique;

	CShaderHandle m_Shader;

	CShaderHandle m_PlanarReflectionShader;

	CTextureHandle m_PerturbationTexture;

	boost::shared_ptr<CFontBase> m_pFont;

	boost::shared_ptr<CTextureRenderTarget> m_pTextureRenderTarget;

protected:

	void RenderReflectionSourceMeshes( const Matrix34& camera_pose, CullingMode::Name culling_mode );

	void RenderReflectionSurface();

public:

	CPlanarReflectionTest();

	~CPlanarReflectionTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void UpdateViewTransform( const Matrix44& matView );

	void UpdateProjectionTransform( const Matrix44& matProj );

//	void OnKeyPressed( KeyCode::Code key_code );
};


#endif /* __PlanarReflectionTest_HPP__ */
