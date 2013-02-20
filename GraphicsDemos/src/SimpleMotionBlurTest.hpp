#ifndef  __SimpleMotionBlurTest_HPP__
#define  __SimpleMotionBlurTest_HPP__


#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CSimpleMotionBlurTest : public CGraphicsTestBase
{
	boost::shared_ptr<SimpleMotionBlur> m_pSimpleMotionBlur;

//	MeshHandle m_SkyboxMesh;
	TextureHandle m_SkyTexture;

	MeshHandle m_TerrainMesh;

//	std::vector<MeshHandle> m_vecpMeshes;

	ShaderTechniqueHandle m_MeshTechnique;

//	ShaderTechniqueHandle m_SkyboxTechnique;

	ShaderTechniqueHandle m_DefaultTechnique;

	ShaderHandle m_Shader;

//	float m_FOV;// = PI / 4.0f;

	boost::shared_ptr<FontBase> m_pFont;

protected:

	void RenderScene();

public:

	CSimpleMotionBlurTest();

	~CSimpleMotionBlurTest();

	const char *GetAppTitle() const { return "SimpleMotionBlurTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	void OnKeyPressed( KeyCode::Code key_code );
};


#endif /* __SimpleMotionBlurTest_HPP__ */
