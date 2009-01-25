
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
	size_t i, num_tasks = m_vecpTask.size();
	for( i=0; i<num_tasks; i++ )
		m_vecpTask[i]->RenderBase();

	SafeDeleteVector( m_vecpTask );
}

