#ifndef __EmbeddedMiscHLSL_HPP__
#define __EmbeddedMiscHLSL_HPP__


#include <string>
#include "../../../../base.hpp"


namespace amorphous
{


class EmbeddedMiscShader
{
public:
	enum ID
	{
		SINGLE_DIFFUSE_COLOR,
		SHADED_SINGLE_DIFFUSE_COLOR,
		VERTEX_WEIGHT_MAP_DISPLAY,
		DEPTH_RENDERING_IN_VIEW_SPACE,
		DEPTH_RENDERING_IN_PROJECTION_SPACE,
		NUM_IDS
	};
};


class EmbeddedMiscHLSL
{
public:

	EmbeddedMiscHLSL(){}
	~EmbeddedMiscHLSL(){}

	static const char *ms_pSingleDiffuseColor;

	static const char *ms_pShadedSingleDiffuseColor;

	static const char *ms_pVertexWeightMapDisplay;

	static const char *ms_pDepthRenderingInViewSpace;

	static const char *ms_pDepthRenderingInProjectionSpace;

	static const char *ms_pDepthRenderingPixelShaderAndTechnique;

	static const char *ms_pTechniqueTemplate;

	static Result::Name GetShader( EmbeddedMiscShader::ID shader_id, std::string& hlsl_effect );
};


} // namespace amorphous



#endif /* __EmbeddedMiscHLSL_HPP__ */
