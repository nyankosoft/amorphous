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

	TextureHandle m_SkyTexture;

	MeshHandle m_TerrainMesh;

//	std::vector<MeshHandle> m_Meshes;

	ShaderTechniqueHandle m_MeshTechnique;

	ShaderTechniqueHandle m_DefaultTechnique;

	ShaderHandle m_Shader;

	int m_BlurWeight;

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

	void HandleInput( const InputData& input );
};


#endif /* __SimpleMotionBlurDemo_HPP__ */
