#ifndef  __GLSLTest_H__
#define  __GLSLTest_H__


#include <vector>
#include "boost/shared_ptr.hpp"
#include "amorphous/3DMath/Matrix34.hpp"
#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/GraphicsComponentCollector.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"
#include "amorphous/GUI/fwd.hpp"
#include "amorphous/Graphics/OpenGL/Shader/GLShader.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CGLSLTest : public CGraphicsTestBase, public GraphicsComponent
{
	std::vector<MeshHandle> m_Meshes;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<FontBase> m_pFont;

	boost::shared_ptr<CGLProgram> m_pGLProgram;

	std::string m_TextBuffer;

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
