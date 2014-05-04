#ifndef  __TextureRenderTargetTest_HPP__
#define  __TextureRenderTargetTest_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Graphics/Mesh/CustomMesh.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CTextureRenderTargetTest : public CGraphicsTestBase
{
	MeshHandle m_Mesh;

	ShaderHandle m_Shader;

	ShaderHandle m_NoLightingShader;

	ShaderTechniqueHandle m_MeshTechnique;

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
