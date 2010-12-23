#ifndef __EmbeddedMiscHLSL_HPP__
#define __EmbeddedMiscHLSL_HPP__


#include <string>
#include "../../../../base.hpp"


class CEmbeddedMiscShader
{
public:
	enum ID
	{
		DEPTH_RENDERING_IN_VIEW_SPACE,
		DEPTH_RENDERING_IN_PROJECTION_SPACE,
		NUM_IDS
	};
};


class CEmbeddedMiscHLSL
{
public:

	CEmbeddedMiscHLSL(){}
	~CEmbeddedMiscHLSL(){}

	static const char *ms_pDepthRenderingInViewSpace;

	static const char *ms_pDepthRenderingInProjectionSpace;

	static const char *ms_pDepthRenderingPixelShaderAndTechnique;

	Result::Name GetShader( CEmbeddedMiscShader::ID shader_id, std::string& hlsl_effect );
};



#endif /* __EmbeddedMiscHLSL_HPP__ */
