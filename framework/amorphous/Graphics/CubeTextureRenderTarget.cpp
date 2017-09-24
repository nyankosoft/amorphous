#include "CubeTextureRenderTarget.hpp"


namespace amorphous
{


std::shared_ptr<CubeTextureRenderTarget> (*CubeTextureRenderTarget::ms_pCreateCubeTextureRenderTarget)(void);


std::shared_ptr<CubeTextureRenderTarget> CubeTextureRenderTarget::Create()
{
	if( ms_pCreateCubeTextureRenderTarget )
		return (*ms_pCreateCubeTextureRenderTarget)();
	else
		return std::shared_ptr<CubeTextureRenderTarget>();
}


} // namespace amorphous
