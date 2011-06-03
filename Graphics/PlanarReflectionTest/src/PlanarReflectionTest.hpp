#ifndef  __PlanarReflectionTest_HPP__
#define  __PlanarReflectionTest_HPP__


#include "3DMath/Vector3.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/ShaderHandle.hpp"
#include "Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "Input/fwd.hpp"

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

	void RenderReflectionSourceMeshes( const Vector3& camera_pos );

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
