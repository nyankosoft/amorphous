#include "EmbeddedPostProcessEffectGLSLShader.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{


const char *EmbeddedPostProcessEffectGLSLShader::m_pVertexShader =
"#version 330\n"\
"layout(location = 0) in vec4 position;\n"\
"layout(location = 1) in vec2 tex0;\n"\
"out vec2 vScreenPosition;\n"\
"void main(){"\
	/*"gl_Position = position;"\*/
	"float vpw = 1280;"\
	"float vph =  720;"\
	"float x = ( position.x / vpw - 0.5) * 2.0;"\
	"float y = (-position.y / vph + 0.5) * 2.0;"\
	"gl_Position = vec4(x,y,position.z,1);"\
	"vScreenPosition = tex0;"\
"}\n";

//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
const char *EmbeddedPostProcessEffectGLSLShader::m_pTextureSamplers =
"uniform sampler2D S0;"\
"uniform sampler2D S1;"\
"uniform sampler2D S2;"\
"uniform sampler2D S3;"\
"uniform sampler2D S4;"\
"uniform sampler2D S5;"\
"uniform sampler2D S6;"\
"uniform sampler2D S7;\n";

// Contains sampling offsets used by the techniques
const char *EmbeddedPostProcessEffectGLSLShader::m_pSampleOffsetsAndWeights =
"const int MAX_SAMPLES = 16;\n"\
"uniform vec2 g_avSampleOffsets[MAX_SAMPLES];\n"\
"uniform vec4 g_avSampleWeights[MAX_SAMPLES];\n";

const char *EmbeddedPostProcessEffectGLSLShader::m_pBloom =
// Multiply each horizontal / vertical bloom process 
"uniform float  g_fBloomFactor = 1.0;\n"\
"in vec2 vScreenPosition"\
"out vec4 color;"\
"vec4 main()"\
"{"\

	"vec4 vSample = 0.0;"\
	"vec4 vColor = 0.0;"\

	"vec2 vSamplePosition;"\

	// Perform a one-directional gaussian blur
	"for(int iSample = 0; iSample < 15; iSample++)"\
	"{"\
		"vSamplePosition = vScreenPosition + g_avSampleOffsets[iSample];"\
		"vColor = texture(S0, vSamplePosition);"\
		"vSample += g_avSampleWeights[iSample]*vColor;"\
	"}"\

	"color = vec4( vSample.rgb * g_fBloomFactor, vSample.a );"\
//	color = vec4( vSample.rgb / 2.0, vSample.a );
//	color = vec4( vSample.rgb, vSample.a );
"}\n";
/*
"technique Bloom"\
"{"\
	"pass P0"\
	"{"\
		"PixelShader = compile ps_2_0 BloomPS();"\
	"}"\
"}\n";*/

const char *EmbeddedPostProcessEffectGLSLShader::m_pDownScale2x2 =
"in vec2 vScreenPosition"\
"out vec4 color;"\
"void main()"\
"{"\
	"vec4 sample = 0.0;"\

	"for( int i=0; i < 4; i++ )"\
	"{"\
		"sample += texture( S0, vScreenPosition + g_avSampleOffsets[i] );"\
	"}"\

	"color = sample / 4;"
"}\n";
/*
"technique DownScale2x2"\
"{"\
	"pass P0"\
	"{"\
		"PixelShader = compile ps_2_0 DownScale2x2PS();"\
	"}"\
"}\n";*/

const char *EmbeddedPostProcessEffectGLSLShader::m_pDownScale4x4 =
"in vec2 vScreenPosition"\
"out vec4 color;"\
"void main()"\
"{"\
	"vec4 sample = 0.0;"\

	"for( int i=0; i < 16; i++ )"\
	"{"\
		"sample += texture( S0, vScreenPosition + g_avSampleOffsets[i] );"\
	"}"\

	"color = sample / 16;"\
"}\n";
/*
"technique DownScale4x4"\
"{"\
	"pass P0"\
	"{"\
		"PixelShader = compile ps_2_0 DownScale4x4PS();"\
	"}"\
"}\n";*/

const char *EmbeddedPostProcessEffectGLSLShader::m_pGaussBlur5x5 =
"in vec2 vScreenPosition"\
"out vec4 color;"\
"void main()"\
"{"\

	"vec4 sample = 0.0;"\

	"for( int i=0; i < 12; i++ )"\
	"{"\
		"sample += g_avSampleWeights[i] * texture( S0, vScreenPosition + g_avSampleOffsets[i] );"\
	"}"\

	"color = sample;"\
"}\n";
/*
"technique GaussBlur5x5"\
"{"\
	"pass P0"\
	"{"\
		"PixelShader = compile ps_2_0 GaussBlur5x5PS();"\
	"}"\
"}\n";*/

const char *EmbeddedPostProcessEffectGLSLShader::m_pMonochrome =
"uniform vec3 g_vLuminanceConv = vec3( 0.2125, 0.7154, 0.0721 );\n"\
"uniform vec4 g_vMonochromeColorOffset = vec4( 0.0, 0.0, 0.0, 0.0 );\n"\

"in vec2 vScreenPosition;\n"\
"layout(location = 0) out vec4 fc;\n"\
"void main()"\
"{"\
	"float f = dot( texture(S0,vScreenPosition).xyz, g_vLuminanceConv );"\
	"vec4 monochrome_color = vec4(f,f,f,f);"\
	"monochrome_color.a = 1.0;"\
	"fc = monochrome_color + g_vMonochromeColorOffset;"\
"}\n";

/*"technique MonochromeColor"\
"{"\
	"pass p0"\
	"{"\
		"PixelShader = compile ps_2_0 MonochromeColorPS();"\
		"ZEnable = false;"\
	"}"\
"}\n";*/



const char *EmbeddedPostProcessEffectGLSLShader::m_pHDR =

// ================ Global constants ================

// Maximum texture grabs
//"static const int    MAX_SAMPLES            = 16;\n"\

// Scene lights 
//"static const int    NUM_LIGHTS             = 2;\n"\

// Threshold for BrightPass filter
"static const float  BRIGHT_PASS_THRESHOLD  = 5.0;\n"\

// Offset for BrightPass filter
"static const float  BRIGHT_PASS_OFFSET     = 10.0;\n"\

// The per-color weighting to be used for luminance calculations in RGB order.
"static const vec3 LUMINANCE_VECTOR  = vec3(0.2125, 0.7154, 0.0721);\n"\

// The per-color weighting to be used for blue shift under low light.
"static const vec3 BLUE_SHIFT_VECTOR = vec3(1.05, 0.97, 1.27);\n"\

// ================ Global variables ================

// Light variables
//vec4 g_avLightPositionView[NUM_LIGHTS];   // Light positions in view space
//vec4 g_avLightIntensity[NUM_LIGHTS];      // Floating point light intensities

// Exponents for the phong equation
"uniform float  g_fPhongExponent;\n"\

// Coefficient for the phong equation
"uniform float  g_fPhongCoefficient;"\

// Coefficient for diffuse equation
"uniform float  g_fDiffuseCoefficient;\n"\

// Emissive intensity of the current light
"uniform vec4 g_vEmissive;\n"\

// ---------- Tone mapping variables ----------
// The middle gray key value
"uniform float  g_fMiddleGray;\n"\

// Lowest luminance which is mapped to white
//"float  g_fWhiteCutoff;\n"\

// Time in seconds since the last calculation
"uniform float  g_fElapsedTime;\n"\

// Flag indicates if blue shift is performed
"uniform bool  g_bEnableBlueShift = false;\n"\

// Flag indicates if tone mapping is performed
"uniform bool  g_bEnableToneMap = true;\n"\

// star effect
"uniform bool  g_bRenderStar = false;\n"\

// Bloom process multiplier
"uniform float  g_fBloomScale;\n"\

// Star process multiplier
"uniform float  g_fStarScale;\n"\

// Multiply each horizontal / vertical bloom process
//"float  g_fBloomFactor = 1.0;\n"\

// how quickly the camera adjust the brightness. range: (0.0,1.0]
"uniform float  g_fAdaptationRate = 0.02;\n";

//  Sample the luminance of the source image using a kernal of sample
//  points, and return a scaled image containing the log() of averages
// examples (ignored 0.0001 in log())
// scene tex
// (e,   e,   e,   1.0) -> log(e)   -> fLogLumSum +=  1
// (1,   1,   1,   1.0) -> log(1)   -> fLogLumSum +=  0
// (1/e, 1/e, 1/e, 1.0) -> log(1/e) -> fLogLumSum += -1
//-----------------------------------------------------------------------------
const char *EmbeddedPostProcessEffectGLSLShader::m_pHDR_InitialLuimnanceSampling =
"in vec2 vScreenPosition"\
/* SampleLumInitial */
"void main()\n"\
"{"\
	"vec3 vSample = 0.0;"\
	"float  fLogLumSum = 0.0;"\

	"for(int i=0; i<9; i++)"\
	"{"\
		// Compute the sum of log(luminance) throughout the sample points
		// - Returned to (luminance) by exp() in SampleLumFinal()
		"vSample = texture(S0, vScreenPosition+g_avSampleOffsets[i]);"\
		"fLogLumSum += log(dot(vSample, LUMINANCE_VECTOR)+0.0001f);"\
	"}"\

	// Divide the sum to complete the average
	"fLogLumSum /= 9;"\

	"return vec4(fLogLumSum, fLogLumSum, fLogLumSum, 1.0);"\
"}\n";

// Scale down the luminance texture by blending sample points
const char *EmbeddedPostProcessEffectGLSLShader::m_pHDR_IterativeLuimnanceSampling =
"in vec2 vScreenPosition"\
/* SampleLumIterative */
"void main()\n"\
"{"\
	"float fResampleSum = 0.0;"\

	"for(int i=0; i<16; i++)"\
	"{"\
		// Compute the sum of luminance throughout the sample points
		"fResampleSum += texture(S0, vScreenPosition+g_avSampleOffsets[i]);"\
	"}"\

	// Divide the sum to complete the average
	"fResampleSum /= 16;"\

	"return vec4(fResampleSum, fResampleSum, fResampleSum, 1.0);"\
"}\n";

// Extract the average luminance of the image by completing the averaging
// and taking the exp() of the result
const char *EmbeddedPostProcessEffectGLSLShader::m_pHDR_FinalLuimnanceSampling =
"in vec2 vScreenPosition"\
/* SampleLumFinal */
"void main()\n"\
"{"\
	"float fResampleSum = 0.0;"\

	"for(int i=0; i<16; i++)"\
	"{"\
		// Compute the sum of luminance throughout the sample points
		"fResampleSum += texture(S0, vScreenPosition+g_avSampleOffsets[i]);"\
	"}"\

	// Divide the sum to complete the average, and perform an exp() to complete
	// the average luminance calculation
	"fResampleSum = exp(fResampleSum/16);"\

	"return vec4(fResampleSum, fResampleSum, fResampleSum, 1.0);"\
"}\n";

// Calculate the luminance that the camera is current adapted to, using
// the most recented adaptation level, the current scene luminance, and
// the time elapsed since last calculated
const char *EmbeddedPostProcessEffectGLSLShader::m_pHDR_AdaptedLuminanceCalc =
"in vec2 vScreenPosition"\
/* CalculateAdaptedLumPS */
"void main()\n"\
"{"\
	"float fAdaptedLum = texture(S0, vec2(0.5f, 0.5f));"\
	"float fCurrentLum = texture(S1, vec2(0.5f, 0.5f));"\

	// >>> comment in original D3D sample
	// The user's adapted luminance level is simulated by closing the gap between
	// adapted luminance and current luminance by (g_fAdaptationRate*100)% every frame, based on a
	// 30 fps rate. This is not an accurate model of human adaptation, which can
	// take longer than half an hour.
	// <<< comment in original D3D sample
	// In the original D3D sample g_fAdaptationRate was hard-coded to 0.02.
	"float f = ( 1 - pow( 1.0 - g_fAdaptationRate, 30 * g_fElapsedTime ) );"\
	"float fNewAdaptation = fAdaptedLum + (fCurrentLum - fAdaptedLum) * f;"\
	"return vec4(fNewAdaptation, fNewAdaptation, fNewAdaptation, 1.0);"\
"}\n";

// Perform blue shift, tone map the scene, and add post-processed light effects
const char *EmbeddedPostProcessEffectGLSLShader::m_pHDR_FinalPass =
"in vec2 vScreenPosition"\
/* FinalScenePassPS */
"void main()\n"\
"{"\
	"vec4 vSample      = texture(S0, vScreenPosition);"\
	"vec4 vBloom       = texture(S1, vScreenPosition);"\
	"vec4 vStar        = texture(S2, vScreenPosition);"\
	"float fAdaptedLum = texture(S3, vec2(0.5f, 0.5f));\n"\

	// For very low light conditions, the rods will dominate the perception
	// of light, and therefore color will be desaturated and shifted
	// towards blue.
	"if( g_bEnableBlueShift )"\
	"{"\
		// Define a linear blending from -1.5 to 2.6 (log scale) which
		// determines the lerp amount for blue shift
		"float fBlueShiftCoefficient = 1.0 - (fAdaptedLum + 1.5)/4.1;"\
		"fBlueShiftCoefficient = saturate(fBlueShiftCoefficient);"\

		// Lerp between current color and blue, desaturated copy
		"vec3 vRodColor = dot( (vec3)vSample, LUMINANCE_VECTOR ) * BLUE_SHIFT_VECTOR;"\
		"vSample.rgb = lerp( (vec3)vSample, vRodColor, fBlueShiftCoefficient );"\
	"}\n"\


	// Map the high range of color values into a range appropriate for
	// display, taking into account the user's adaptation level, and selected
	// values for for middle gray and white cutoff.
	"if( g_bEnableToneMap )"\
	"{"\
		"vSample.rgb *= g_fMiddleGray/(fAdaptedLum + 0.001f);"\
		"vSample.rgb /= (1.0+vSample);"\
	"}\n"\

	// Add the star and bloom post processing effects
	"if( g_bRenderStar )"\
		"vSample += g_fStarScale * vStar;\n"

	"vSample += g_fBloomScale * vBloom;\n"\

	"return vSample;"\
//	return vec4(fAdaptedLum,fAdaptedLum,fAdaptedLum,1.0);
"}\n";

// Perform a high-pass filter on the source texture
const char *EmbeddedPostProcessEffectGLSLShader::m_pHDR_BrightPass =
"in vec2 vScreenPosition"\
/* BrightPassFilterPS */
"vec4 main()\n"\
"{"\
	"vec4 vSample = texture( S0, vScreenPosition );"\
	"float  fAdaptedLum = texture( S1, vec2(0.5, 0.5) );"\
//	float  fAdaptedLum = 0.03;

	// Determine what the pixel's value will be after tone-mapping occurs
	"vSample.rgb *= g_fMiddleGray/(fAdaptedLum + 0.001f);"\

	// Subtract out dark pixels
	"vSample.rgb -= BRIGHT_PASS_THRESHOLD;"\

	// Clamp to 0
	"vSample = max(vSample, 0.0);"\

	// Map the resulting value into the 0 to 1 range. Higher values for
	// BRIGHT_PASS_OFFSET will isolate lights from illuminated scene 
	// objects.
	"vSample.rgb /= (BRIGHT_PASS_OFFSET+vSample);"\

	"return vSample;"\
//	return vec4(fAdaptedLum,fAdaptedLum,fAdaptedLum,1.0);
"}\n";
/*
// Takes the HDR Scene texture as input and starts the process of 
// determining the average luminance by converting to grayscale, taking
// the log(), and scaling the image to a single pixel by averaging sample 
// points.
"technique SampleAvgLum"\
"{"\
	"pass P0"\
	"{"\
		"PixelShader  = compile ps_2_0 SampleLumInitial();"\
	"}"\
"}\n"\

// Continue to scale down the luminance texture
"technique ResampleAvgLum"\
"{"\
	"pass P0"\
	"{"\
		"PixelShader  = compile ps_2_0 SampleLumIterative();"\
	"}"\
"}\n"\

// Sample the texture to a single pixel and perform an exp() to complete
// the evalutation
"technique ResampleAvgLumExp"\
"{"\
	"pass P0"\
	"{"\
		"PixelShader  = compile ps_2_0 SampleLumFinal();"\
	"}"\
"}\n"\

// Determines the level of the user's simulated light adaptation level
// using the last adapted level, the current scene luminance, and the
// time since last calculation
"technique CalculateAdaptedLum"\
"{"\
	"pass P0"\
	"{"\
		"PixelShader  = compile ps_2_0 CalculateAdaptedLumPS();"\
	"}"\
"}\n"\

// Perform a high-pass filter on the source texture
"technique BrightPassFilter"\
"{"\
	"pass P0"\
	"{"\
		"PixelShader  = compile ps_2_0 BrightPassFilterPS();"\
	"}"\
"}\n"\

// Minimally transform and texture the incoming geometry
"technique FinalScenePass"\
"{"\
	"pass P0"\
	"{"\
		"PixelShader  = compile ps_2_0 FinalScenePassPS();"\
	"}"\
"}\n";*/


Result::Name EmbeddedPostProcessEffectGLSLShader::GenerateVertexShader( const std::string& effect_name, std::string& vertex_shader )
{
	vertex_shader = m_pVertexShader;

	return Result::SUCCESS;
}


Result::Name EmbeddedPostProcessEffectGLSLShader::GenerateFragmentShader( const std::string& effect_name, std::string& fragment_shader )
{
	std::string& fs = fragment_shader;
	const std::string& effect = effect_name;

	fs = EmbeddedPostProcessEffectGLSLShader::m_pTextureSamplers;

	fs += EmbeddedPostProcessEffectGLSLShader::m_pSampleOffsetsAndWeights;

//	if( m_EffectName.length() == 0 )
//	{
//		GetCombinedShader( shader );
//		return;
//	}

	if( effect == "monochrome" )                            fs += m_pMonochrome;
	else if( effect == "bloom" )                            fs += m_pBloom;
	else if( effect == "down_scale_2x2" )                   fs += m_pDownScale2x2;
	else if( effect == "down_scale_4x4" )                   fs += m_pDownScale4x4;
	else if( effect == "gauss_blur_5x5" )                   fs += m_pGaussBlur5x5;
	else if( effect == "hdr.initial_luminance_sampling" )   fs += std::string(m_pHDR) + m_pHDR_InitialLuimnanceSampling;
	else if( effect == "hdr.iterative_luminance_sampling" ) fs += std::string(m_pHDR) + m_pHDR_IterativeLuimnanceSampling;
	else if( effect == "hdr.final_luminance_sampling" )     fs += std::string(m_pHDR) + m_pHDR_FinalLuimnanceSampling;
	else if( effect == "hdr.adapted_luminance_calc" )       fs += std::string(m_pHDR) + m_pHDR_AdaptedLuminanceCalc;
	else if( effect == "hdr.bright_pass" )                  fs += std::string(m_pHDR) + m_pHDR_BrightPass;
	else if( effect == "hdr.final_pass" )                   fs += std::string(m_pHDR) + m_pHDR_FinalPass;
	else
	{
		LOG_PRINT_WARNING( "An unsupported effect name: " + effect );
		return Result::INVALID_ARGS;
	}

	return Result::SUCCESS;
}


} // namespace amorphous
