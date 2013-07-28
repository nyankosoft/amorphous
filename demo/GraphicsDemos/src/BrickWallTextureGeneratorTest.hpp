#ifndef  __BrickWallTextureGeneratorTest_HPP__
#define  __BrickWallTextureGeneratorTest_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CBrickWallTextureGeneratorTest : public CGraphicsTestBase
{
	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	std::string m_TextBuffer;

	MeshHandle m_BrickWallMesh;

	unsigned int m_PresetIndex;

private:

	int LoadMesh();

	bool InitShader();

	void RenderMeshes();

	void SetLights( bool use_hemespheric_light );

public:

	CBrickWallTextureGeneratorTest();

	~CBrickWallTextureGeneratorTest();

	const char *GetAppTitle() const { return "BrickWallTextureGeneratorTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __BrickWallTextureGeneratorTest_HPP__ */
