#ifndef  __PerlinNoiseTextureGeneratorTest_H__
#define  __PerlinNoiseTextureGeneratorTest_H__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CPerlinNoiseTextureGeneratorTest : public CGraphicsTestBase
{
//	ShaderHandle m_Shader;

	boost::shared_ptr<Timer> m_pTimer;

	TextureHandle m_PerlinNoiseTexture;

	TextureHandle m_PerlinNoiseNormalMap;

	int m_ImageWidth;

	float m_fTextureRepeats;

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
