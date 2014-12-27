#ifndef __GraphicsElementEffectDemo_HPP__
#define __GraphicsElementEffectDemo_HPP__


#include "amorphous/Graphics/GraphicsComponentCollector.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Graphics/GraphicsEffectManager.hpp"
#include "amorphous/Input/fwd.hpp"
#include "amorphous/Graphics/Mesh/CustomMesh.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


//		m_pGraphicsElementManager->SetScale( g_WindowSize.x / 800.0f );


class GraphicsElementEffectDemo : public CGraphicsTestBase
{
	boost::shared_ptr<GraphicsElementAnimationManager> m_pAnimGraphicsManager;

//	boost::shared_ptr<CGraphicsElementManager> m_pGraphicsElementManager;

	boost::shared_ptr<GraphicsElement> m_pElements[8];

private:

	void CreateGraphicsElements();

	void CreateGraphicsEffects();

public:

	GraphicsElementEffectDemo() {}

	int Init();

	virtual ~GraphicsElementEffectDemo() { Release(); }

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



#endif /* __GraphicsElementEffectDemo_HPP__ */
