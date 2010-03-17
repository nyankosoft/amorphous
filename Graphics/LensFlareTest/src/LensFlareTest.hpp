#ifndef  __LensFlareTest_HPP__
#define  __LensFlareTest_HPP__


#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "3DMath/Vector3.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/ShaderHandle.hpp"
#include "Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "Input.hpp"
#include "Graphics/UnitSphere.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"

/*
class CLensFlareTest;

class CLensFlareTestInputHandler : public CInputHandler
{
	CLensFlareTest* m_pTest;

public:

	CLensFlareTestInputHandler( CLensFlareTest* pTest )
		:
	m_pTest(pTest) {}

	virtual void ProcessInput(SInputData& input);
};
*/

class CLensFlareTest : public CGraphicsTestBase
{
	boost::shared_ptr<CLensFlare> m_pLensFlare;

	CMeshObjectHandle m_SkyboxMesh;

	CMeshObjectHandle m_TerrainMesh;

//	std::vector<shared_ptr<CD3DXMeshObject>> m_vecpMeshObject;

	CShaderTechniqueHandle m_MeshTechnique;

//	CShaderTechniqueHandle m_SkyboxTechnique;

	CShaderTechniqueHandle m_DefaultTechnique;

//	shared_ptr<CShaderManager> m_pShaderManager;
	CShaderHandle m_Shader;

	CTextureHandle m_TestTexture;

	CUnitSphere m_LightPosIndicator;

//	CShaderLightManager m_ShaderLightManager;

//	LPDIRECT3DTEXTURE9 m_pDefaultTexture = NULL;

//	float m_FOV;// = D3DX_PI / 4.0f;

	Vector3 m_vLightPosition;

	CFontSharedPtr m_pFont;

	CInputHandlerSharedPtr m_pInputHandler;

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
