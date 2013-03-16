#ifndef  __PerlinNoiseTextureGeneratorTest_H__
#define  __PerlinNoiseTextureGeneratorTest_H__


#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CPerlinNoiseTextureGeneratorTest : public CGraphicsTestBase
{
//	ShaderHandle m_Shader;

	boost::shared_ptr<FontBase> m_pFont;

	boost::shared_ptr<Timer> m_pTimer;

	TextureHandle m_PerlinNoiseTexture;

	TextureHandle m_PerlinNoiseNormalMap;

	int m_ImageWidth;

	float m_fTextureRepeats;

	std::string m_TextBuffer;

private:

	void CreateSampleUI();

	bool InitShader();

	void CreatePerlinNoiseTexture();

	void SaveTexturesAsImageFiles();

public:

	CPerlinNoiseTextureGeneratorTest();

	~CPerlinNoiseTextureGeneratorTest();

	const char *GetAppTitle() const { return "PerlinNoiseTextureGeneratorTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const InputData& input );
};


#endif /* __PerlinNoiseTextureGeneratorTest_H__ */
