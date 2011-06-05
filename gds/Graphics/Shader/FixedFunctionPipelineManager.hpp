#ifndef  __FixedFunctionPipelineManager_HPP__
#define  __FixedFunctionPipelineManager_HPP__


#include "../../Support/Singleton.hpp"
using namespace NS_KGL;


class CShaderManager;


/// Called by the system
extern void InitFixedFunctionPipelineManager( CShaderManager *pFFPMgr );
extern CShaderManager& FixedFunctionPipelineManager();


class CFixedFunctionPipelineManagerHolder
{
	CShaderManager *m_pFFPMgr;

	/// singleton
	static CSingleton<CFixedFunctionPipelineManagerHolder> m_obj;

public:

	CFixedFunctionPipelineManagerHolder()
		:
	m_pFFPMgr(NULL)
	{}

	static CFixedFunctionPipelineManagerHolder* Get() { return m_obj.get(); }

	/// Should be called only by the function/class that initializes graphics library.
	void Init( CShaderManager *pFFPMgr ) { m_pFFPMgr = pFFPMgr; }

	CShaderManager *GetFFPMgr() { return m_pFFPMgr; }

//	friend class ???;
};


inline CShaderManager& FixedFunctionPipelineManager()
{
	return *(CFixedFunctionPipelineManagerHolder::Get()->GetFFPMgr());
}



#endif		/*  __FixedFunctionPipelineManager_HPP__  */
