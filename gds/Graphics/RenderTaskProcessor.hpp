#ifndef __RenderTaskProcessor_H__
#define __RenderTaskProcessor_H__


#include <vector>

#include "Support/Singleton.hpp"
using namespace NS_KGL;


#define RenderTaskProcessor ( (*CRenderTaskProcessor::Get()) )


class CRenderTask;

class CRenderTaskProcessor
{
	/// owned ref
	std::vector<CRenderTask *> m_vecpTask;

protected:

	static CSingleton<CRenderTaskProcessor> m_obj;

public:

	static CRenderTaskProcessor* Get() { return m_obj.get(); }

	CRenderTaskProcessor() {}

	virtual ~CRenderTaskProcessor() { Release(); }

	/// forces render task processor to release any remaining render tasks
	void Release();

	/// call Render() of each render task in the list
	/// - Starts from the first element of m_vecpTask
	/// - The last render task usually renders to backbuffer,
	///   whereas all the other render tasks that come before it render to texture render target
	/// - Release all the instances of render tasks once they are done
	void Render();

	/// \param owned reference of a render task
	inline void AddRenderTask( CRenderTask *pTask )
	{
		m_vecpTask.push_back( pTask );	
	}
};


#endif /* __RenderTaskProcessor_H__ */
