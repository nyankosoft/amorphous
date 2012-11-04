#ifndef  __TextureRenderTargetTest_HPP__
#define  __TextureRenderTargetTest_HPP__


#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Graphics/Mesh/CustomMesh.hpp"
#include "gds/Input/fwd.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CTextureRenderTargetTest : public CGraphicsTestBase
{
	CMeshObjectHandle m_Mesh;

	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<CFontBase> m_pFont;

	std::string m_TextBuffer;

	boost::shared_ptr<CTextureRenderTarget> m_pTextureRenderTarget;

	CCustomMesh m_BoxMesh;

private:

	bool InitShader();

	void RenderMeshes();

	void SaveTexturesAsImageFiles();

	void RenderTexturedRect();

public:

	CTextureRenderTargetTest();

	~CTextureRenderTargetTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const SInputData& input );
};



#endif /* __TextureRenderTargetTest_HPP__ */
