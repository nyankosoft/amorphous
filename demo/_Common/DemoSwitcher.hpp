#ifndef __DemoSwitcher_HPP__
#define __DemoSwitcher_HPP__


#include "amorphous/App/GraphicsApplicationBase.hpp"

using namespace amorphous;


class CGraphicsTestBase;
class DemoFactory;


class DemoSwitcher : public GraphicsApplicationBase
{
	std::shared_ptr<CGraphicsTestBase> m_pDemo;

	int m_DemoIndex;

	bool m_DisplayDebugInfo;

	std::shared_ptr<DemoFactory> m_pDemoFactory;

private:

	CGraphicsTestBase *CreateTestInstance( const std::string& demo_name );

	CGraphicsTestBase *CreateDemoInstance( unsigned int index );

public:
	
	DemoSwitcher();

	const std::string GetApplicationTitle() { return "demo_app"; }

	int Init();

	void Update( float dt );

	void NextDemo();

	void PrevDemo();

	bool InitDemo();

	bool InitDemo( int index );

	void SetDemoFactory( std::shared_ptr<DemoFactory> pFactory ) { m_pDemoFactory = pFactory; }

	void Render();

	void HandleInput( const InputData& input );
};


#endif /* __DemoSwitcher_HPP__ */
