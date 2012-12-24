#ifndef  __GLSLTest_H__
#define  __GLSLTest_H__


#include <vector>
#include "boost/shared_ptr.hpp"
#include "gds/3DMath/Matrix34.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"
#include "gds/GUI/fwd.hpp"
#include "gds/Graphics/OpenGL/Shader/GLShader.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CGLSLTest : public CGraphicsTestBase, public CGraphicsComponent
{
	std::vector<CMeshObjectHandle> m_Meshes;

	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<CFontBase> m_pFont;

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

	virtual void HandleInput( const SInputData& input );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif /* __GLSLTest_H__ */
