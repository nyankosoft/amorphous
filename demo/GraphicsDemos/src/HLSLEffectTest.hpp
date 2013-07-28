#ifndef  __HLSLEffectTest_H__
#define  __HLSLEffectTest_H__


#include "amorphous/Graphics/GraphicsComponentCollector.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"
#include "amorphous/GUI/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"

#include "AsyncLoadingTest.hpp"


class CHLSLEffectTest : public CGraphicsTestBase, public GraphicsComponent
{
	enum Params
	{
		TEXT_BUFFER_SIZE = 4096
	};

//	std::vector<MeshHandle> m_vecMesh;
	std::vector<CTestMeshHolder> m_vecMesh;

	std::vector<ShaderHandle> m_Shaders;

	std::vector<ShaderTechniqueHandle> m_Techniques;

	int m_CurrentShaderIndex;

	ShaderTechniqueHandle m_MeshTechnique;

	bool m_EnableLight[2];

	boost::shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

	CGM_DialogManagerSharedPtr m_pSampleUI;

	std::string m_TextBuffer;

	bool m_DisplayDebugInfo;

	int m_CurrentMeshIndex;

	TextureHandle m_LookupTextureForLighting;

private:

	void CreateSampleUI();

	bool SetShader( int index );

	bool InitShaders();

	void CreateCachedResources();

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

//	virtual void RenderBase();

	virtual void HandleInput( const InputData& input );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const GraphicsParameters& rParam );
};


#endif /* __HLSLEffectTest_H__ */
