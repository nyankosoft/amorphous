#ifndef  __PrimitiveShapeMeshesTest_HPP__
#define  __PrimitiveShapeMeshesTest_HPP__


#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"
#include "gds/GUI/fwd.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CPrimitiveShapeMeshesTest : public CGraphicsTestBase
{
	std::vector<CMeshObjectHandle> m_vecMesh;
//	std::vector<CTestMeshHolder> m_vecMesh;

	CTextureHandle m_ConeTexture;

	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<CFontBase> m_pFont;

	std::string m_TextBuffer;

	int m_MeshIndex;

	int m_NumPrimitiveMeshes;

	bool m_Lighting;

private:

	void CreateGUIControls();

	bool InitShader();

	void RenderMeshes();

public:

	CPrimitiveShapeMeshesTest();

	~CPrimitiveShapeMeshesTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void HandleInput( const SInputData& input );
};


#endif /* __PrimitiveShapeMeshesTest_HPP__ */
