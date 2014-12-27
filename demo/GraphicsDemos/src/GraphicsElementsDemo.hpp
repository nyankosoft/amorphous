#ifndef  __GraphicsElementsDemo_H__
#define  __GraphicsElementsDemo_H__


#include "amorphous/Graphics/GraphicsElementManager.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class GraphicsElementsDemo : public CGraphicsTestBase//, public GraphicsComponent
{
	boost::shared_ptr<GraphicsElementManager> m_pGraphicsElementManager;

//	CGM_DialogManagerSharedPtr m_pSampleUI;

	std::vector< boost::shared_ptr<CombinedRectElement> > m_pRects;

	std::vector< boost::shared_ptr<CombinedRoundRectElement> > m_pRoundRects;

	boost::shared_ptr<CombinedTriangleElement> m_apTriangle[0xFF];

	std::vector< boost::shared_ptr<FillTriangleElement> > m_pFillTriangles;

	boost::shared_ptr<FillTriangleElement> m_pCornerIndicators[4];

private:

	void CreateSampleUI();

	void RenderGraphicsElements();

	void CreateGraphicsElements();

	void ReleaseGraphicsElements();

	void ReleaseAllGraphicsElements();

	void TestRotations();

public:

	GraphicsElementsDemo();

	~GraphicsElementsDemo();

	const char *GetAppTitle() const { return "GraphicsElementsDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const GraphicsParameters& rParam );
};


#endif /* __GraphicsElementsDemo_H__ */
