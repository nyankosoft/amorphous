#ifndef  __HLSLEffectTest_H__
#define  __HLSLEffectTest_H__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CHLSLEffectTest : public CGraphicsTestBase
{
	std::vector<MeshHandle> m_Meshes;

	std::vector<float> m_MeshScales;

	std::vector<ShaderHandle> m_Shaders;

	std::vector<ShaderTechniqueHandle> m_Techniques;

	int m_CurrentShaderIndex;

	ShaderTechniqueHandle m_MeshTechnique;

	bool m_EnableLight[2];

	bool m_DisplayDebugInfo;

	int m_CurrentMeshIndex;

	TextureHandle m_LookupTextureForLighting;

private:

	bool SetShader( int index );

	bool InitShaders();

	void RenderMesh();

	void RenderDebugInfo();

public:

	CHLSLEffectTest();

	~CHLSLEffectTest();

	const char *GetAppTitle() const { return "HLSLEffectTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __HLSLEffectTest_H__ */
