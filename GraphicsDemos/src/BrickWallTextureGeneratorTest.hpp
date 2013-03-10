#ifndef  __BrickWallTextureGeneratorTest_HPP__
#define  __BrickWallTextureGeneratorTest_HPP__


#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CBrickWallTextureGeneratorTest : public CGraphicsTestBase
{
	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<FontBase> m_pFont;

	std::string m_TextBuffer;

	MeshHandle m_BrickWallMesh;

private:

	bool InitShader();

	void RenderMeshes();

	void SetLights( bool use_hemespheric_light );

public:

	CBrickWallTextureGeneratorTest();

	~CBrickWallTextureGeneratorTest();

	const char *GetAppTitle() const { return "BrickWallTextureGeneratorTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __BrickWallTextureGeneratorTest_HPP__ */
