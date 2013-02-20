#ifndef  __PrimitiveShapeMeshesTest_HPP__
#define  __PrimitiveShapeMeshesTest_HPP__


#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CPrimitiveShapeMeshesTest : public CGraphicsTestBase
{
	std::vector<MeshHandle> m_vecMesh;
//	std::vector<CTestMeshHolder> m_vecMesh;

	TextureHandle m_ConeTexture;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<FontBase> m_pFont;

	std::string m_TextBuffer;

	int m_MeshIndex;

	int m_NumPrimitiveMeshes;

	bool m_Lighting;

private:

	int CreatePrimitiveShapeMeshes();

	bool InitShader();

	void RenderMeshes();

public:

	CPrimitiveShapeMeshesTest();

	~CPrimitiveShapeMeshesTest();

	const char *GetAppTitle() const { return "PrimitiveShapeMeshesTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void HandleInput( const InputData& input );
};


#endif /* __PrimitiveShapeMeshesTest_HPP__ */
