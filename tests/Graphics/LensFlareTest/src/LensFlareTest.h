#ifndef  __LensFlareTest_H__
#define  __LensFlareTest_H__


#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
using namespace boost;

#include "3DMath/Vector3.h"
#include "3DCommon/fwd.h"
#include "3DCommon/MeshObjectHandle.h"
#include "3DCommon/TextureHandle.h"
#include "3DCommon/Shader/ShaderTechniqueHandle.h"
#include "GameInput/fwd.h"
#include "GameInput.h"
#include "3DCommon/UnitSphere.h"

#include "../../../_Common/GraphicsTestBase.h"

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
	shared_ptr<CLensFlare> m_pLensFlare;

	CMeshObjectHandle m_TerrainMesh;

//	std::vector<shared_ptr<CD3DXMeshObject>> m_vecpMeshObject;

	CShaderTechniqueHandle m_MeshTechnique;

	CShaderTechniqueHandle m_SkyboxTechnique;

	CShaderTechniqueHandle m_DefaultTechnique;

	shared_ptr<CShaderManager> m_pShaderManager;

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

	void UpdateViewTransform( const D3DXMATRIX& matView );

	void UpdateProjectionTransform( const D3DXMATRIX& matProj );

//	void OnKeyPressed( KeyCode::Code key_code );
};


#endif /* __LensFlareTest_H__ */
