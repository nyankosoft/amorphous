#ifndef  __PrecipitationEffectTest_H__
#define  __PrecipitationEffectTest_H__


#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CDoubleConeScrollEffect;


class PrecipitationEffectDemo : public CGraphicsTestBase
{
	std::shared_ptr<CDoubleConeScrollEffect> m_pDoubleConeScrollEffect;

//	std::vector<MeshHandle> m_vecMesh;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	std::shared_ptr<FontBase> m_pFont;

	std::string m_TextBuffer;

	TextureHandle m_SkyTexture;
	MeshHandle m_SkyboxMesh;

private:

	void CreateSampleUI();

	bool InitShader();

	void RenderMeshes();

//	void SaveTexturesAsImageFiles();

public:

	PrecipitationEffectDemo();

	~PrecipitationEffectDemo();

	const char *GetAppTitle() const { return "PrecipitationEffectTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const InputData& input );
};


#endif /* __PrecipitationEffectTest_H__ */
