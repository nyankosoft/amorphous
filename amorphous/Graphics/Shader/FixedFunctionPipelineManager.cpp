#include "FixedFunctionPipelineManager.hpp"
#include "ShaderManager.hpp"


namespace amorphous
{


singleton<CFixedFunctionPipelineManagerHolder> CFixedFunctionPipelineManagerHolder::m_obj;


//static ShaderManager *sg_pFixedFunctionPipelineManager = NULL;

/*
/// Take the pointer of a statically allocated object.
/// The system does not try to release pFFPMgr.
void InitFixedFunctionPipelineManager( ShaderManager *pFFPMgr )
{
	sg_pFixedFunctionPipelineManager = pFFPMgr;
}


ShaderManager& FixedFunctionPipelineManager()
{
	return *sg_pFixedFunctionPipelineManager;
}
*/


} // namespace amorphous
