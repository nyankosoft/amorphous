#ifndef  __GenericShaderDemo_HPP__
#define  __GenericShaderDemo_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Graphics/Shader/GenericShaderDesc.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class GenericShaderDemo : public CGraphicsTestBase
{
	std::vector<MeshHandle> m_Meshes;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_Technique;

	GenericShaderDesc m_GenericShaderDesc;

	ShaderTechniqueHandle m_MeshTechnique;

//	boost::shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

//	CGM_DialogManagerSharedPtr m_pSampleUI;

	bool m_DisplayDebugInfo;

private:

	void CreateSampleUI();

	bool SetShaderParams( ShaderManager& shader_mgr );

	bool InitShader();

	void RenderMeshes();

	void RenderShaderInfo();

	void RenderDebugInfo();

public:

	GenericShaderDemo();

	~GenericShaderDemo();

	const char *GetAppTitle() const { return "GenericShaderDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __GenericShaderDemo_HPP__ */
