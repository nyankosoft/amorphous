#ifndef __EmbeddedPostProcessEffectGLSLShader_HPP__
#define __EmbeddedPostProcessEffectGLSLShader_HPP__


#include "../../../Shader/Embedded/EmbeddedPostProcessEffectShader.hpp"
#include "../../../Shader/ShaderGenerator.hpp"
//#include "../../../PostProcessEffectFilter.hpp"


namespace amorphous
{


class EmbeddedPostProcessEffectGLSLShader : public EmbeddedPostProcessEffectShader
{
public:

	EmbeddedPostProcessEffectGLSLShader(){}
	~EmbeddedPostProcessEffectGLSLShader(){}

	static const char *m_pVertexShader;

	static const char *m_pTextureSamplers;

	static const char *m_pSampleOffsetsAndWeights;

	/// \brief A fragment shader which blurs the source image along one axis using
	///        a gaussian distribution. Since gaussian blurs are separable, this shader
	///        is called twice; first along the horizontal axis, then along the vertical axis.
	static const char *m_pBloom;

	/// \brief A fragment shader which scales the source texture down to 1/4 scale
	static const char *m_pDownScale2x2;

	/// \brief A fragment shader which scales the source texture down to 1/16 scale
	static const char *m_pDownScale4x4;

	/// \brief A fragment shader which simulatess a 5x5 kernel gaussian blur
	///        by sampling the 12 points closest to the center point
	static const char *m_pGaussBlur5x5;

	/// \brief A fragment shader which performs post-processing effect
	///        that converts a colored image to a grayscale one.
	static const char *m_pMonochrome;

	static const char *m_pHDR;
	static const char *m_pHDR_InitialLuimnanceSampling;
	static const char *m_pHDR_IterativeLuimnanceSampling;
	static const char *m_pHDR_FinalLuimnanceSampling;
	static const char *m_pHDR_AdaptedLuminanceCalc;
	static const char *m_pHDR_BrightPass;
	static const char *m_pHDR_FinalPass;
	
	bool SupportsCombinedShader() const { return false; }

	Result::Name GenerateVertexShader( const std::string& effect_name, std::string& vertex_shader );

	Result::Name GenerateFragmentShader( const std::string& effect_name, std::string& fragment_shader );
};


} // namespace amorphous



#endif /* __EmbeddedPostProcessEffectGLSLShader_HPP__ */
