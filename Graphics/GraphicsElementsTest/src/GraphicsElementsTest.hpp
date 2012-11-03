#ifndef  __GraphicsElementsTest_H__
#define  __GraphicsElementsTest_H__


#include <vector>
#include "gds/Graphics/GraphicsElementManager.hpp"
#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CGraphicsElementsTest : public CGraphicsTestBase//, public CGraphicsComponent
{
	boost::shared_ptr<CGraphicsElementManager> m_pGraphicsElementManager;

	boost::shared_ptr<CFontBase> m_pFont;

//	CGM_DialogManagerSharedPtr m_pSampleUI;

	std::vector< boost::shared_ptr<CCombinedRectElement> > m_pRects;

	std::vector< boost::shared_ptr<CCombinedRoundRectElement> > m_pRoundRects;

	boost::shared_ptr<CCombinedTriangleElement> m_apTriangle[0xFF];

	std::string m_TextBuffer;

private:

	void CreateSampleUI();

	void RenderGraphicsElements();

	void CreateGraphicsElements();

	void ReleaseGraphicsElements();

	void ReleaseAllGraphicsElements();

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
