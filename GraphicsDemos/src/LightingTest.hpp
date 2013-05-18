#ifndef  __LightingTest_HPP__
#define  __LightingTest_HPP__


#include "amorphous/Graphics/GraphicsComponentCollector.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"
#include "amorphous/Graphics/Mesh/CustomMesh.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CLightingTest : public CGraphicsTestBase
{
//	std::vector<MeshHandle> m_vecMesh;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<FontBase> m_pFont;

	std::string m_TextBuffer;

	CustomMesh m_Mesh;

	MeshHandle m_RegularMesh;

private:

	bool InitShader();

	void RenderMeshes();

	void SetLights( bool use_hemespheric_light );

	void SetHSDirectionalLights( ShaderLightManager& shader_light_mgr );

	void SetHSPointLights( ShaderLightManager& shader_light_mgr );

	void SetHSSpotights( ShaderLightManager& shader_light_mgr );

public:

	CLightingTest();

	~CLightingTest();

	const char *GetAppTitle() const { return "LightingTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __LightingTest_HPP__ */
