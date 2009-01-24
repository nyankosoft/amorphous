
#include "RenderTaskProcessor.h"
#include "RenderTask.h"
#include "Support/SafeDeleteVector.h"

#include "Graphics/Direct3D9.h"

using namespace std;


CSingleton<CRenderTaskProcessor> CRenderTaskProcessor::m_obj;


void CRenderTaskProcessor::Release()
{
	SafeDeleteVector( m_vecpTask );
}


void CRenderTaskProcessor::Render()
{
	// BeginScene() here if render task system is NOT used
	HRESULT hr = DIRECT3D9.GetDevice()->BeginScene();

	size_t i, num_tasks = m_vecpTask.size();
	for( i=0; i<num_tasks; i++ )
		m_vecpTask[i]->RenderBase();

	SafeDeleteVector( m_vecpTask );

	// EndScene() and Present() here if render task system is NOT used
	hr = DIRECT3D9.GetDevice()->EndScene();

	hr = DIRECT3D9.GetDevice()->Present( NULL, NULL, NULL, NULL );
}

