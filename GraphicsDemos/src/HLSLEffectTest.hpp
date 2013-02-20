#ifndef  __HLSLEffectTest_H__
#define  __HLSLEffectTest_H__


#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"
#include "gds/GUI/fwd.hpp"

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

	boost::shared_ptr<FontBase> m_pFont;

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
