#ifndef  __GLSLTest_H__
#define  __GLSLTest_H__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"
#include "amorphous/Graphics/OpenGL/Shader/GLShader.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CGLSLTest : public CGraphicsTestBase
{
	std::vector<MeshHandle> m_Meshes;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<CGLProgram> m_pGLProgram;

private:

	bool InitShader();

	void CreateCachedResources();

	void RenderMeshes();

	void SetLights();

public:

	CGLSLTest();

	~CGLSLTest();

	const char *GetAppTitle() const { return "GLSLTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const InputData& input );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const GraphicsParameters& rParam );
};


#endif /* __GLSLTest_H__ */
