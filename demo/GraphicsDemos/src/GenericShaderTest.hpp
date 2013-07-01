#ifndef  __GenericShaderTest_HPP__
#define  __GenericShaderTest_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Graphics/Shader/GenericShaderDesc.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CGenericShaderTest : public CGraphicsTestBase
{
	std::vector<MeshHandle> m_Meshes;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_Technique;

	GenericShaderDesc m_GenericShaderDesc;

	ShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<FontBase> m_pFont;

//	boost::shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

//	CGM_DialogManagerSharedPtr m_pSampleUI;

	std::string m_TextBuffer;

	bool m_DisplayDebugInfo;

private:

	void CreateSampleUI();

	bool SetShaderParams( ShaderManager& shader_mgr );

	bool InitShader();

	void RenderMeshes();

	void RenderShaderInfo();

	void RenderDebugInfo();

public:

	CGenericShaderTest();

	~CGenericShaderTest();

	const char *GetAppTitle() const { return "GenericShaderTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const InputData& input );
};


#endif /* __GenericShaderTest_HPP__ */
