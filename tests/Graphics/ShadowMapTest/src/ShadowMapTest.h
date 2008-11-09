#ifndef  __ShadowMapTest_H__
#define  __ShadowMapTest_H__


#include <vector>
#include <string>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
using namespace boost;

#include "3DMath/Vector3.h"
#include "3DCommon/fwd.h"
#include "3DCommon/GraphicsComponentCollector.h"
#include "3DCommon/MeshObjectHandle.h"
#include "3DCommon/ShaderHandle.h"
#include "3DCommon/Shader/ShaderTechniqueHandle.h"
#include "GameInput/fwd.h"
#include "GameInput.h"
#include "UI/fwd.h"

#include "../../../_Common/GraphicsTestBase.h"


class CShadowMapTest : public CGraphicsTestBase, public CGraphicsComponent
{
	std::vector<CMeshObjectHandle> m_vecMesh;

	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_MeshTechnique;

	shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

	boost::shared_ptr<CFontBase> m_pFont;

	CInputHandlerSharedPtr m_pInputHandler;

	boost::shared_ptr<CShadowMapManager> m_pShadowMapManager; 

private:

	void CreateLights();

	bool InitShader();

public:

	CShadowMapTest();

	~CShadowMapTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	void RenderMeshes( CShaderHandle& shader_handle );

	void RenderShadowCasters();

	void RenderShadowReceivers();

	virtual void HandleInput( const SInputData& input );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif /* __ShadowMapTest_H__ */
