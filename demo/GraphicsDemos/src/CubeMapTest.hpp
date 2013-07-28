#ifndef  __CubeMapTest_HPP__
#define  __CubeMapTest_HPP__


#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/CubeMapManager.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CCubeMapTest : public CGraphicsTestBase
{
	std::vector<MeshHandle> m_vecEnvMappedMesh;

	MeshHandle m_Mesh;

	uint m_MeshIndex;

	ShaderTechniqueHandle m_NonEnvMappedMesh;
	ShaderTechniqueHandle m_EnvMappedMesh;
//	ShaderTechniqueHandle m_SkyboxTechnique;
//	ShaderTechniqueHandle m_DefaultTechnique;

	std::vector<TextureHandle> m_CubeMapTextures;

	ShaderHandle m_EnvMappedMeshShader;

	ShaderHandle m_NonEnvMappedMeshShader;

	bool m_UseEmbeddedShader;

public:

	CCubeMapTest();

	~CCubeMapTest();

	const char *GetAppTitle() const { return "CubeMapTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void HandleInput( const InputData& input );

//	void RenderNonEnvMappedMeshes();

	void UpdateViewTransform( const Matrix44& matView );

	void UpdateProjectionTransform( const Matrix44& matProj );
};


#endif /* __CubeMapTest_HPP__ */
