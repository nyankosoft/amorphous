#ifndef __GraphicsElementEffectTest_HPP__
#define __GraphicsElementEffectTest_HPP__


#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Graphics/GraphicsEffectManager.hpp"
#include "gds/Input/fwd.hpp"
#include "gds/Graphics/Mesh/CustomMesh.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


//		m_pGraphicsElementManager->SetScale( g_WindowSize.x / 800.0f );


typedef CAnimatedGraphicsManager CGraphicsEffectManager;


class CGraphicsElementEffectTest : public CGraphicsTestBase
{
	boost::shared_ptr<CGraphicsEffectManager> m_pAnimGraphicsManager;

//	boost::shared_ptr<CGraphicsElementManager> m_pGraphicsElementManager;

	boost::shared_ptr<CGraphicsElement> m_pElements[8];

private:

	void CreateGraphicsElements();

	void CreateGraphicsEffects();

public:

	CGraphicsElementEffectTest() {}

	int Init();

	virtual ~CGraphicsElementEffectTest() { Release(); }

	void Release() { m_pAnimGraphicsManager.reset(); }
	void Render();
	void Update( float dt );
};


//class CPyScriptGraphicsTest : public CGraphicsTestBase
//{
//	CAnimatedGraphicsManager* m_pAnimGraphicsManager;
//
//	CGraphicsElementManager* m_pGraphicsElementManager;
//
//public:
//
//	CPyScriptGraphicsTest()
//	{
//		m_pAnimGraphicsManager = new CAnimatedGraphicsManager();
//
//		// retrieve and save the pointer to CGraphicsElementManager
//		// the pointer is a borrowed reference and must not be deleted
//		// it is deleted by CAnimatedGraphicsManager
//		m_pGraphicsElementManager = m_pAnimGraphicsManager->GetGraphicsElementManager();
//
//		m_pGraphicsElementManager->SetScale( g_WindowSize.x / 800.0f );
//
//		SetAnimatedGraphicsManagerForScript( m_pAnimGraphicsManager );
//	}
//
//	virtual ~CPyScriptGraphicsTest() { Release(); }
//
//	virtual void Release() { SafeDelete( m_pAnimGraphicsManager ); }
//	virtual void Render() { m_pGraphicsElementManager->Render(); }
//	virtual void Update( float dt ) { m_pAnimGraphicsManager->UpdateEffects( dt ); }
//};



#endif /* __GraphicsElementEffectTest_HPP__ */
