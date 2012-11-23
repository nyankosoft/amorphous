#ifndef  __GenericShaderTest_HPP__
#define  __GenericShaderTest_HPP__


#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Graphics/Shader/GenericShaderDesc.hpp"
#include "gds/Input/fwd.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CGenericShaderTest : public CGraphicsTestBase
{
	std::vector<CMeshObjectHandle> m_Meshes;

	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_Technique;

	CGenericShaderDesc m_GenericShaderDesc;

	CShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<CFontBase> m_pFont;

//	boost::shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

//	CGM_DialogManagerSharedPtr m_pSampleUI;

	std::string m_TextBuffer;

	bool m_DisplayDebugInfo;

private:

	void CreateSampleUI();

	bool SetShaderParams( CShaderManager& shader_mgr );

	bool InitShader();

	void RenderMeshes();

	void RenderShaderInfo();

	void RenderDebugInfo();

public:

	CGenericShaderTest();

	~CGenericShaderTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const SInputData& input );
};


#endif /* __GenericShaderTest_HPP__ */
