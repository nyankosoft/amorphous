#ifndef  __EmbeddedGenericHLSL_HPP__
#define  __EmbeddedGenericHLSL_HPP__


#include "amorphous/base.hpp"
#include "amorphous/Graphics/fwd.hpp"
#include <string>


namespace amorphous
{


class EmbeddedGenericHLSL
{
	static const char *ms_pMatrix;

	static const char *ms_pColorPair;

	static const char *ms_pTexDef;

	static const char *ms_pLightDef;

	static const char *ms_pQuaternion;

	static const char *ms_pTransform;

	static const char *ms_pQVertexBlendFunctions;

//	static const char *ms_pVertexBlendFunctions;

	static const char *ms_pEnvMapSamplerInclude;

	static const char *ms_pOptionsMacros;

	static const char *ms_pPerPixelHSLighting_Specular;

	static const char *ms_pTechniqueTemplate;

public:

	EmbeddedGenericHLSL();

	virtual ~EmbeddedGenericHLSL();

	static Result::Name GenerateShader( const GenericShaderDesc& desc, std::string& hlsl_effect );
};


} // namespace amorphous



#endif		/*  __EmbeddedGenericHLSL_HPP__  */
