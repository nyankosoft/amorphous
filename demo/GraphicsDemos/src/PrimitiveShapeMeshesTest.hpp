#ifndef  __PrimitiveShapeMeshesTest_HPP__
#define  __PrimitiveShapeMeshesTest_HPP__


#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CPrimitiveShapeMeshesTest : public CGraphicsTestBase
{
	std::vector<MeshHandle> m_vecMesh;
//	std::vector<CTestMeshHolder> m_vecMesh;

	TextureHandle m_ConeTexture;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

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
