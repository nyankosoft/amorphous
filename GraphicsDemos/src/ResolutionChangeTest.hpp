#ifndef  __ResolutionChangeTest_HPP__
#define  __ResolutionChangeTest_HPP__


#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Graphics/GraphicsDevice.hpp"
#include "gds/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CResolutionChangeTest : public CGraphicsTestBase
{
	enum MeshIDs
	{
		MESH_BOX,
		MESH_SPHERE,
		MESH_TEXTURED_BOX,
		MESH_TEXTURED_SPHERE,
		NUM_MESHES
	};

	std::vector<DisplayMode>  m_AvailableDisplayModes;

//	MeshHandle m_SkyboxMesh;

//	MeshHandle m_SphereMesh;

	std::vector<MeshHandle> m_Meshes;

//	ShaderTechniqueHandle m_SkyboxTechnique;

//	ShaderTechniqueHandle m_DefaultTechnique;

	ShaderHandle m_Shader;

	std::vector<TextureHandle> m_TestTextures;

	boost::shared_ptr<FontBase> m_pFont;

	int m_CurrentlyEnteredDigits;

private:

	void SetLight();

	void Render2DRects();
	
public:

	CResolutionChangeTest();

	~CResolutionChangeTest();

	const char *GetAppTitle() const { return "ResolutionChangeTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void HandleInput( const InputData& input );
};


#endif /* __ResolutionChangeTest_HPP__ */
