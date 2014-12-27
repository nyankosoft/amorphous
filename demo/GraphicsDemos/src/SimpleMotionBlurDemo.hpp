#ifndef  __SimpleMotionBlurDemo_HPP__
#define  __SimpleMotionBlurDemo_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class SimpleMotionBlurDemo : public CGraphicsTestBase
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

protected:

	void RenderScene();

public:

	SimpleMotionBlurDemo();

	~SimpleMotionBlurDemo();

	const char *GetAppTitle() const { return "SimpleMotionBlurDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	void OnKeyPressed( KeyCode::Code key_code );
};


#endif /* __SimpleMotionBlurDemo_HPP__ */
