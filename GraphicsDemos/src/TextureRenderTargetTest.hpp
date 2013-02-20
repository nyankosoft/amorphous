#ifndef  __TextureRenderTargetTest_HPP__
#define  __TextureRenderTargetTest_HPP__


#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Graphics/Mesh/CustomMesh.hpp"
#include "gds/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CTextureRenderTargetTest : public CGraphicsTestBase
{
	MeshHandle m_Mesh;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<FontBase> m_pFont;

	std::string m_TextBuffer;

	boost::shared_ptr<TextureRenderTarget> m_pTextureRenderTarget;

	CustomMesh m_BoxMesh;

private:

	bool InitShader();

	void RenderMeshes();

	void SaveTexturesAsImageFiles();

	void RenderTexturedRect();

public:

	CTextureRenderTargetTest();

	~CTextureRenderTargetTest();

	const char *GetAppTitle() const { return "TextureRenderTargetTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};



#endif /* __TextureRenderTargetTest_HPP__ */
