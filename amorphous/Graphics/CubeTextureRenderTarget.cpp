#include "CubeTextureRenderTarget.hpp"


namespace amorphous
{


boost::shared_ptr<CubeTextureRenderTarget> (*CubeTextureRenderTarget::ms_pCreateCubeTextureRenderTarget)(void);


boost::shared_ptr<CubeTextureRenderTarget> CubeTextureRenderTarget::Create()
{
	if( ms_pCreateCubeTextureRenderTarget )
		return (*ms_pCreateCubeTextureRenderTarget)();
	else
		return boost::shared_ptr<CubeTextureRenderTarget>();
}


} // namespace amorphous
