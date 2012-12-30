#include "FixedFunctionPipelineManager.hpp"
#include "ShaderManager.hpp"


namespace amorphous
{


CSingleton<CFixedFunctionPipelineManagerHolder> CFixedFunctionPipelineManagerHolder::m_obj;


//static CShaderManager *sg_pFixedFunctionPipelineManager = NULL;

/*
/// Take the pointer of a statically allocated object.
/// The system does not try to release pFFPMgr.
void InitFixedFunctionPipelineManager( CShaderManager *pFFPMgr )
{
	sg_pFixedFunctionPipelineManager = pFFPMgr;
}


CShaderManager& FixedFunctionPipelineManager()
{
	return *sg_pFixedFunctionPipelineManager;
}
*/


} // namespace amorphous
