#ifndef  __FixedFunctionPipelineManager_HPP__
#define  __FixedFunctionPipelineManager_HPP__


#include "../../Support/singleton.hpp"


namespace amorphous
{


class ShaderManager;


/// Called by the system
extern void InitFixedFunctionPipelineManager( ShaderManager *pFFPMgr );
extern ShaderManager& FixedFunctionPipelineManager();


class CFixedFunctionPipelineManagerHolder
{
	ShaderManager *m_pFFPMgr;

	/// singleton
	static singleton<CFixedFunctionPipelineManagerHolder> m_obj;

public:

	CFixedFunctionPipelineManagerHolder()
		:
	m_pFFPMgr(NULL)
	{}

	static CFixedFunctionPipelineManagerHolder* Get() { return m_obj.get(); }

	/// Should be called only by the function/class that initializes graphics library.
	void Init( ShaderManager *pFFPMgr ) { m_pFFPMgr = pFFPMgr; }

	ShaderManager *GetFFPMgr() { return m_pFFPMgr; }

//	friend class ???;
};


inline ShaderManager& FixedFunctionPipelineManager()
{
	return *(CFixedFunctionPipelineManagerHolder::Get()->GetFFPMgr());
}


} // namespace amorphous



#endif		/*  __FixedFunctionPipelineManager_HPP__  */
