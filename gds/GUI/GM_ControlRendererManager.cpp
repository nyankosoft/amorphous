
#include "GM_ControlRenderer.hpp"
#include "GM_ControlRendererManager.hpp"
#include "GM_ControlRendererManager.hpp"
#include "Graphics/GraphicsEffectManager.hpp"
#include "Support/SafeDelete.hpp"

using namespace std;


CGM_ControlRendererManager::CGM_ControlRendererManager()
{
	m_pGraphicsEffectManager = CAnimatedGraphicsManagerSharedPtr( new CAnimatedGraphicsManager );

	m_pGraphicsElementManager = m_pGraphicsEffectManager->GetGraphicsElementManager();
}


CGM_ControlRendererManager::CGM_ControlRendererManager( CAnimatedGraphicsManagerSharedPtr pGraphicsEffectManager )
{
	m_pGraphicsEffectManager = pGraphicsEffectManager;

	m_pGraphicsElementManager = m_pGraphicsEffectManager->GetGraphicsElementManager();
}


CGM_ControlRendererManager::~CGM_ControlRendererManager()
{
	Release();
}


void CGM_ControlRendererManager::Release()
{
	// Make sure that the element manager is released before effect manager.
	// When the ctor with no arguments is used, in which case m_pGraphicsElementManager is
	// a borrowed copy of the element manager stored in m_pGraphicsEffectManager,
	// omitting this causes crash in CGraphiceEffectManagerCallback::OnDestroyed()
	// because m_pGraphicsEffectManager is released first.
	m_pGraphicsElementManager.reset();

//	size_t i, num_renderers = m_vecpControlRenderer.size();
//	for( i=0; i<num_renderers; i++ )
//		SafeDelete( m_vecpControlRenderer[i] );
}

/*
void CGM_ControlRendererManager::RegisterRenderer( CGM_ControlRenderer *pControlRenderer, CGM_ControlBase *pControl )
{
	m_vecpControlRenderer.push_back( pControlRenderer );

//	pControlRenderer->SetControlRendererManager( this );
//	pControlRenderer->SetControl( pControl );
//	pControlRenderer->Init();
}
*/

int CGM_ControlRendererManager::CalcUILayerIndex( CGM_ControlRenderer *pControlRenderer )
{
	int m_MaxLayerIndex = 80;
	int layer_index = 0;

	CGM_ControlBase *pBaseControl = pControlRenderer->GetBaseControl();
	if( pBaseControl )
	{
		layer_index = m_MaxLayerIndex - pBaseControl->GetDepth() * GetNumGraphicsLayersPerUILayer();

		// 'depth * 2' because each dialog is made up of 2 graphics layers
		// 
//		layer_index = m_MaxLayerIndex - pBaseControl->GetDepth() * 2 * GetNumGraphicsLayersPerUILayer();

		if( layer_index < 0 )
			layer_index = 0;
	}
	else
	{
		// does not owned by any control
		// - treat this as a caption renderer
		// - render this on top of all the other UI graphics elements
		return 0;
	}

	return layer_index;
}

/*
void CGM_ControlRendererManager::UpdateGraphicsLayers()
{
	size_t i, num_renderers = m_vecpControlRenderer.size();
	for( i=0; i<num_renderers; i++ )
	{
		m_vecpControlRenderer[i]->UpdateGraph();
	}
}
*/


void CGM_ControlRendererManager::Render()
{
	m_pGraphicsElementManager->Render();
}


void CGM_ControlRendererManager::Update( float dt )
{
	m_pGraphicsEffectManager->UpdateEffects( dt );
}
