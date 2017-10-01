#ifndef  __ParticleTextureGeneratorDemo_H__
#define  __ParticleTextureGeneratorDemo_H__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class ParticleTextureGeneratorDemo : public CGraphicsTestBase
{
	//	ShaderHandle m_Shader;

//	std::shared_ptr<Timer> m_pTimer;

	std::vector<TextureHandle> m_ParticleTextures;

	std::vector<TextureHandle> m_SmokeParticleTextures;

	TextureHandle m_BackgroundTexture;

//	TextureHandle m_PerlinNoiseNormalMap;

	int m_ImageWidth;

	float m_fTextureRepeats;

	int m_NumColorVariations;

private:

//	void CreateSampleUI();

	bool InitShader();

	void CreateParticleTextures();

	void CreateSmokeParticleTextures();

	void SaveTexturesAsImageFiles();

public:

	ParticleTextureGeneratorDemo();

	~ParticleTextureGeneratorDemo();

	const char *GetAppTitle() const { return "ParticleTextureGeneratorDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __ParticleTextureGeneratorDemo_H__ */
