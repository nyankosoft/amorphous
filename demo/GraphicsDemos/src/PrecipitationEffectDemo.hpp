#ifndef  __PrecipitationEffectTest_H__
#define  __PrecipitationEffectTest_H__


#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CDoubleConeScrollEffect;


class PrecipitationEffectDemo : public CGraphicsTestBase
{
	boost::shared_ptr<CDoubleConeScrollEffect> m_pDoubleConeScrollEffect;

//	std::vector<MeshHandle> m_vecMesh;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<FontBase> m_pFont;

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
