#ifndef  __GLSLDemo_H__
#define  __GLSLDemo_H__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"
#include "amorphous/Graphics/OpenGL/Shader/GLShader.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class GLSLDemo : public CGraphicsTestBase
{
	std::vector<MeshHandle> m_Meshes;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<CGLProgram> m_pGLProgram;

private:

	bool InitShader();

	void RenderMeshes();

	void SetLights();

public:

	GLSLDemo();

	~GLSLDemo();

	const char *GetAppTitle() const { return "GLSLDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __GLSLDemo_H__ */
