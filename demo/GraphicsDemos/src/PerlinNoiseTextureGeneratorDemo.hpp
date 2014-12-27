#ifndef  __PerlinNoiseTextureGeneratorDemo_H__
#define  __PerlinNoiseTextureGeneratorDemo_H__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class PerlinNoiseTextureGeneratorDemo : public CGraphicsTestBase
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

	PerlinNoiseTextureGeneratorDemo();

	~PerlinNoiseTextureGeneratorDemo();

	const char *GetAppTitle() const { return "PerlinNoiseTextureGeneratorDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __PerlinNoiseTextureGeneratorDemo_H__ */
