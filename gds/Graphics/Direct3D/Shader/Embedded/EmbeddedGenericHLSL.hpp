#ifndef  __EmbeddedGenericHLSL_HPP__
#define  __EmbeddedGenericHLSL_HPP__


#include "gds/base.hpp"
#include "gds/Graphics/fwd.hpp"
#include <string>


class CEmbeddedGenericHLSL
{
	static const char *ms_pMatrix;

	static const char *ms_pColorPair;

	static const char *ms_pTexDef;

	static const char *ms_pLightDef;

	static const char *ms_pQuaternion;

	static const char *ms_pTransform;

	static const char *ms_pQVertexBlendFunctions;

//	static const char *ms_pVertexBlendFunctions;

	static const char *ms_pPerPixelHSLighting_Specular;

	static const char *ms_pTechniqueTemplate;

public:

	CEmbeddedGenericHLSL();

	virtual ~CEmbeddedGenericHLSL();

	static Result::Name GenerateShader( CGenericShaderDesc& desc, std::string& hlsl_effect );
};



#endif		/*  __EmbeddedGenericHLSL_HPP__  */
