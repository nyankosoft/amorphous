#include "EmbeddedGenericShader.hpp"


namespace amorphous
{


static boost::shared_ptr<EmbeddedGenericShader> sg_pEmbeddedGenericShader;


boost::shared_ptr<EmbeddedGenericShader> GetEmbeddedGenericShader()
{
	return sg_pEmbeddedGenericShader;
}


void SetEmbeddedGenericShader( boost::shared_ptr<EmbeddedGenericShader> ptr )
{
	sg_pEmbeddedGenericShader = ptr;
}


} // namespace amorphous
