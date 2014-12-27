#ifndef  __LightingDemo_HPP__
#define  __LightingDemo_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"
#include "amorphous/Graphics/Mesh/CustomMesh.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class LightingDemo : public CGraphicsTestBase
{
//	std::vector<MeshHandle> m_vecMesh;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	CustomMesh m_Mesh;

	MeshHandle m_RegularMesh;

	bool m_RandomLightColors;

	int m_NumLightsX;

	int m_NumLightsZ;

private:

	bool InitShader();

	void RenderMeshes();

	void SetLights( bool use_hemespheric_light );

	void SetHSDirectionalLights( ShaderLightManager& shader_light_mgr, bool random_color );

	void SetHSPointLights( ShaderLightManager& shader_light_mgr, bool random_color );

	void SetHSSpotights( ShaderLightManager& shader_light_mgr, bool random_color );

public:

	LightingDemo();

	~LightingDemo();

	const char *GetAppTitle() const { return "LightingDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __LightingDemo_HPP__ */
