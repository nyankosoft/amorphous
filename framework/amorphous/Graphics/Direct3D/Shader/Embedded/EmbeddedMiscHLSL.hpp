#ifndef __EmbeddedMiscHLSL_HPP__
#define __EmbeddedMiscHLSL_HPP__


#include "amorphous/base.hpp"
#include "amorphous/Graphics/Shader/MiscShaderGenerator.hpp"


namespace amorphous
{


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

	static Result::Name GetShader( MiscShader::ID shader_id, std::string& hlsl_effect );
};


} // namespace amorphous



#endif /* __EmbeddedMiscHLSL_HPP__ */
