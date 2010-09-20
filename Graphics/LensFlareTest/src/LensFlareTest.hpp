#ifndef  __LensFlareTest_HPP__
#define  __LensFlareTest_HPP__


#include "3DMath/Vector3.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/ShaderHandle.hpp"
#include "Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "Input/fwd.hpp"
#include "Graphics/UnitSphere.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


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

	CUnitSphere m_LightPosIndicator;

//	float m_FOV;// = PI / 4.0f;

	Vector3 m_vLightPosition;

	boost::shared_ptr<CFontBase> m_pFont;

protected:

	void RenderFloor();
	
	void InitLensFlare( const std::string& strPath );

public:

	CLensFlareTest();

	~CLensFlareTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void UpdateViewTransform( const Matrix44& matView );

	void UpdateProjectionTransform( const Matrix44& matProj );

//	void OnKeyPressed( KeyCode::Code key_code );
};


#endif /* __LensFlareTest_HPP__ */
