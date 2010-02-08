#ifndef  __PrimitiveShapeMeshesTest_HPP__
#define  __PrimitiveShapeMeshesTest_HPP__


#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
using namespace boost;

#include "3DMath/Matrix34.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/ShaderHandle.hpp"
#include "Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "Input/fwd.hpp"
#include "Input.hpp"
#include "GUI/fwd.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CPrimitiveShapeMeshesTest : public CGraphicsTestBase, public CGraphicsComponent
{
	enum Params
	{
		TEXT_BUFFER_SIZE = 4096
	};

	std::vector<CMeshObjectHandle> m_vecMesh;
//	std::vector<CTestMeshHolder> m_vecMesh;

	CTextureHandle m_ConeTexture;

	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_MeshTechnique;

	shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

	boost::shared_ptr<CFontBase> m_pFont;

	CInputHandlerSharedPtr m_pInputHandler;

	CGM_DialogManagerSharedPtr m_pSampleUI;

	bool m_TestAsyncLoading;

	char m_TextBuffer[TEXT_BUFFER_SIZE];

	int m_MeshIndex;

	int m_NumPrimitiveMeshes;

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

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif /* __PrimitiveShapeMeshesTest_HPP__ */
