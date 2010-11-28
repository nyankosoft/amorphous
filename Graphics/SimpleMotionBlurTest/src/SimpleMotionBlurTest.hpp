#ifndef  __SimpleMotionBlurTest_HPP__
#define  __SimpleMotionBlurTest_HPP__


#include "3DMath/Vector3.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/ShaderHandle.hpp"
#include "Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "Input/fwd.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CSimpleMotionBlurTest : public CGraphicsTestBase
{
	boost::shared_ptr<CSimpleMotionBlur> m_pSimpleMotionBlur;

	CMeshObjectHandle m_SkyboxMesh;

	CMeshObjectHandle m_TerrainMesh;

//	std::vector<CMeshObjectHandle> m_vecpMeshes;

	CShaderTechniqueHandle m_MeshTechnique;

//	CShaderTechniqueHandle m_SkyboxTechnique;

	CShaderTechniqueHandle m_DefaultTechnique;

	CShaderHandle m_Shader;

//	float m_FOV;// = PI / 4.0f;

	boost::shared_ptr<CFontBase> m_pFont;

protected:

	void RenderScene();

public:

	CSimpleMotionBlurTest();

	~CSimpleMotionBlurTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	void OnKeyPressed( KeyCode::Code key_code );
};


#endif /* __SimpleMotionBlurTest_HPP__ */
