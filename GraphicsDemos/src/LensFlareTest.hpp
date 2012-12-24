#ifndef  __LensFlareTest_HPP__
#define  __LensFlareTest_HPP__


#include "gds/3DMath/Vector3.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CLensFlareTest : public CGraphicsTestBase
{
	boost::shared_ptr<CLensFlare> m_pLensFlare;

	CMeshObjectHandle m_SkyboxMesh;

	CMeshObjectHandle m_TerrainMesh;

//	std::vector<CMeshObjectHandle> m_vecpMeshes;

	CShaderTechniqueHandle m_MeshTechnique;

//	CShaderTechniqueHandle m_SkyboxTechnique;

	CShaderTechniqueHandle m_DefaultTechnique;

	CShaderHandle m_Shader;

	CTextureHandle m_TestTexture;

	CTextureHandle m_SkyTexture;

	CMeshObjectHandle m_LightPosIndicator;

//	float m_FOV;// = PI / 4.0f;

	Vector3 m_vLightPosition;

	boost::shared_ptr<CFontBase> m_pFont;

protected:

	void RenderFloor();

	void InitSkyTexture();
	
	void InitLensFlare( const std::string& strPath );

public:

	CLensFlareTest();

	~CLensFlareTest();

	const char *GetAppTitle() const { return "LensFlareTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void UpdateViewTransform( const Matrix44& matView );

	void UpdateProjectionTransform( const Matrix44& matProj );

//	void OnKeyPressed( KeyCode::Code key_code );
};


#endif /* __LensFlareTest_HPP__ */
