#ifndef  __GraphicsElementsTest_H__
#define  __GraphicsElementsTest_H__


#include <vector>
#include <string>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
using namespace boost;

#include <gds/3DMath/Vector3.hpp>
#include <gds/Graphics/fwd.hpp>
#include <gds/Graphics/GraphicsElementManager.hpp>
#include <gds/Graphics/GraphicsComponentCollector.hpp>
#include <gds/Graphics/ShaderHandle.hpp>
#include <gds/Graphics/Shader/ShaderTechniqueHandle.hpp>
#include <gds/Input/fwd.hpp>
#include <gds/Input.hpp>
#include <gds/GUI/fwd.hpp>

#include "../../../_Common/GraphicsTestBase.hpp"


class CGraphicsElementsTest : public CGraphicsTestBase, public CGraphicsComponent
{
	enum UIID
	{
		UIID_DLG_ROOT = 1000,
		UIID_DLG_SLIDERS,
		UIID_DLG_RESOLUTION,
		UIID_LBX_RESOLUTION,
		UIID_DLG_LISTBOXGROUP,
		UIID_OTHER
	};

	enum Params
	{
		TEXT_BUFFER_SIZE = 4096
	};

	boost::shared_ptr<CGraphicsElementManager> m_pGraphicsElementManager;

	shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

	boost::shared_ptr<CFontBase> m_pFont;

	CInputHandlerSharedPtr m_pInputHandler;

	CGM_DialogManagerSharedPtr m_pSampleUI;

	bool m_TestAsyncLoading;

	boost::shared_ptr<CCombinedRectElement> m_apRect[0xFF];

	boost::shared_ptr<CCombinedTriangleElement> m_apTriangle[0xFF];

	char m_TextBuffer[TEXT_BUFFER_SIZE];

private:

	void CreateSampleUI();

	void RenderGraphicsElements();

	void CreateGraphicsElements();

	void TestRotations();

public:

	CGraphicsElementsTest();

	~CGraphicsElementsTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const SInputData& input );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif /* __GraphicsElementsTest_H__ */
