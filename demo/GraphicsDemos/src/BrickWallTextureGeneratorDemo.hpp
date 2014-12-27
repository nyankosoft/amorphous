#ifndef  __BrickWallTextureGeneratorDemo_HPP__
#define  __BrickWallTextureGeneratorDemo_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class BrickWallTextureGeneratorDemo : public CGraphicsTestBase
{
	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	MeshHandle m_BrickWallMesh;

	unsigned int m_PresetIndex;

private:

	int LoadMesh();

	bool InitShader();

	void RenderMeshes();

	void SetLights( bool use_hemespheric_light );

public:

	BrickWallTextureGeneratorDemo();

	~BrickWallTextureGeneratorDemo();

	const char *GetAppTitle() const { return "BrickWallTextureGeneratorDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __BrickWallTextureGeneratorDemo_HPP__ */
