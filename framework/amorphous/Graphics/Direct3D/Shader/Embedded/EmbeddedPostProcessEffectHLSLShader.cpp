#include "EmbeddedPostProcessEffectHLSLShader.hpp"


namespace amorphous
{


//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
const char *CEmbeddedPostProcessEffectHLSLShader::m_pTextureSamplers =
"sampler s0 : register(s0);"\
"sampler s1 : register(s1);"\
"sampler s2 : register(s2);"\
"sampler s3 : register(s3);"\
"sampler s4 : register(s4);"\
"sampler s5 : register(s5);"\
"sampler s6 : register(s6);"\
"sampler s7 : register(s7);\n";

// Contains sampling offsets used by the techniques
const char *CEmbeddedPostProcessEffectHLSLShader::m_pSampleOffsetsAndWeights =
"static const int MAX_SAMPLES = 16;\n"\
"float2 g_avSampleOffsets[MAX_SAMPLES];\n"\
"float4 g_avSampleWeights[MAX_SAMPLES];\n";

//-----------------------------------------------------------------------------
// Name: BloomPS
// Type: Pixel shader                                      
// Desc: Blur the source image along one axis using a gaussian
//       distribution. Since gaussian blurs are separable, this shader is called 
//       twice; first along the horizontal axis, then along the vertical axis.
//-----------------------------------------------------------------------------
const char *CEmbeddedPostProcessEffectHLSLShader::m_pBloom =
// Multiply each horizontal / vertical bloom process 
"float  g_fBloomFactor = 1.0f;\n"\
"float4 BloomPS( float2 vScreenPosition : TEXCOORD0 ) : COLOR0"\
"{"\

	"float4 vSample = 0.0f;"\
	"float4 vColor = 0.0f;"\

	"float2 vSamplePosition;"\

	// Perform a one-directional gaussian blur
	"for(int iSample = 0; iSample < 15; iSample++)"\
	"{"\
		"vSamplePosition = vScreenPosition + g_avSampleOffsets[iSample];"\
		"vColor = tex2D(s0, vSamplePosition);"\
		"vSample += g_avSampleWeights[iSample]*vColor;"\
	"}"\

	"return float4( vSample.rgb * g_fBloomFactor, vSample.a );"\
//	return float4( vSample.rgb / 2.0f, vSample.a );
//	return float4( vSample.rgb, vSample.a );
"}\n"\

"technique Bloom"\
"{"\
	"pass P0"\
	"{"\
		"PixelShader = compile ps_2_0 BloomPS();"\
	"}"\
"}\n";

//-----------------------------------------------------------------------------
// Name: DownScale2x2PS
// Type: Pixel shader                                      
// Desc: Scale the source texture down to 1/4 scale
//-----------------------------------------------------------------------------
const char *CEmbeddedPostProcessEffectHLSLShader::m_pDownScale2x2 =
"float4 DownScale2x2PS( float2 vScreenPosition : TEXCOORD0 ) : COLOR0"\
"{"\
	"float4 sample = 0.0;"\

	"for( int i=0; i < 4; i++ )"\
	"{"\
		"sample += tex2D( s0, vScreenPosition + g_avSampleOffsets[i] );"\
	"}"\

	"return sample / 4;"
"}\n"\

"technique DownScale2x2"\
"{"\
	"pass P0"\
	"{"\
		"PixelShader = compile ps_2_0 DownScale2x2PS();"\
	"}"\
"}\n";

//-----------------------------------------------------------------------------
// Name: DownScale4x4PS
// Type: Pixel shader                                      
// Desc: Scale the source texture down to 1/16 scale
//-----------------------------------------------------------------------------
const char *CEmbeddedPostProcessEffectHLSLShader::m_pDownScale4x4 =
"float4 DownScale4x4PS( float2 vScreenPosition : TEXCOORD0 ) : COLOR0"\
"{"\
	"float4 sample = 0.0;"\

	"for( int i=0; i < 16; i++ )"\
	"{"\
		"sample += tex2D( s0, vScreenPosition + g_avSampleOffsets[i] );"\
	"}"\

	"return sample / 16;"\
"}\n"\

"technique DownScale4x4"\
"{"\
	"pass P0"\
	"{"\
		"PixelShader = compile ps_2_0 DownScale4x4PS();"\
	"}"\
"}\n";

//-----------------------------------------------------------------------------
// Name: GaussBlur5x5PS
// Type: Pixel shader                                      
// Desc: Simulate a 5x5 kernel gaussian blur by sampling the 12 points closest
//       to the center point.
//-----------------------------------------------------------------------------
const char *CEmbeddedPostProcessEffectHLSLShader::m_pGaussBlur5x5 =
"float4 GaussBlur5x5PS( float2 vScreenPosition : TEXCOORD0 ) : COLOR0"\
"{"\

	"float4 sample = 0.0f;"\

	"for( int i=0; i < 12; i++ )"\
	"{"\
		"sample += g_avSampleWeights[i] * tex2D( s0, vScreenPosition + g_avSampleOffsets[i] );"\
	"}"\

	"return sample;"\
"}\n"\

"technique GaussBlur5x5"\
"{"\
	"pass P0"\
	"{"\
		"PixelShader = compile ps_2_0 GaussBlur5x5PS();"\
	"}"\
"}\n";

//-----------------------------------------------------------------------------
// Pixel Shader: PostProcessPS
// Desc: Performs post-processing effect that converts a colored image to
//       black and white.
//-----------------------------------------------------------------------------
const char *CEmbeddedPostProcessEffectHLSLShader::m_pMonochrome =
"float3 g_vLuminanceConv = { 0.2125f, 0.7154f, 0.0721f };\n"\
"float4 g_vMonochromeColorOffset= { 0.0f, 0.0f, 0.0f, 0.0f };\n"\

"float4 MonochromeColorPS( float2 Tex : TEXCOORD0 ) : COLOR0"\
"{"\
	"float4 monochrome_color = dot( (float3)tex2D( s0, Tex ), g_vLuminanceConv );"\
	"monochrome_color.a = 1.0;"\
	"return monochrome_color + g_vMonochromeColorOffset;"\
"}\n"\

"technique MonochromeColor"\
"{"\
	"pass p0"\
	"{"\
		"PixelShader = compile ps_2_0 MonochromeColorPS();"\
		"ZEnable = false;"\
	"}"\
"}\n";




const char *CEmbeddedPostProcessEffectHLSLShader::m_pHDR =

// ================ Global constants ================

// Maximum texture grabs
//"static const int    MAX_SAMPLES            = 16;\n"\

// Scene lights 
//"static const int    NUM_LIGHTS             = 2;\n"\

// Threshold for BrightPass filter
"static const float  BRIGHT_PASS_THRESHOLD  = 5.0f;\n"\

// Offset for BrightPass filter
"static const float  BRIGHT_PASS_OFFSET     = 10.0f;\n"\

// The per-color weighting to be used for luminance calculations in RGB order.
"static const float3 LUMINANCE_VECTOR  = float3(0.2125f, 0.7154f, 0.0721f);\n"\

// The per-color weighting to be used for blue shift under low light.
"static const float3 BLUE_SHIFT_VECTOR = float3(1.05f, 0.97f, 1.27f);\n"\

// ================ Global variables ================

// Light variables
//float4 g_avLightPositionView[NUM_LIGHTS];   // Light positions in view space
//float4 g_avLightIntensity[NUM_LIGHTS];      // Floating point light intensities

// Exponents for the phong equation
"float  g_fPhongExponent;\n"\

// Coefficient for the phong equation
"float  g_fPhongCoefficient;"\

// Coefficient for diffuse equation
"float  g_fDiffuseCoefficient;\n"\

// Emissive intensity of the current light
"float4 g_vEmissive;\n"\

// ---------- Tone mapping variables ----------
// The middle gray key value
"float  g_fMiddleGray;\n"\

// Lowest luminance which is mapped to white
//"float  g_fWhiteCutoff;\n"\

// Time in seconds since the last calculation
"float  g_fElapsedTime;\n"\

// Flag indicates if blue shift is performed
"bool  g_bEnableBlueShift = false;\n"\

// Flag indicates if tone mapping is performed
"bool  g_bEnableToneMap = true;\n"\

// star effect
"bool  g_bRenderStar = false;\n"\

// Bloom process multiplier
"float  g_fBloomScale;\n"\

// Star process multiplier
"float  g_fStarScale;\n"\

// Multiply each horizontal / vertical bloom process
//"float  g_fBloomFactor = 1.0f;\n"\

// how quickly the camera adjust the brightness. range: (0.0,1.0]
"float  g_fAdaptationRate = 0.02f;\n"\

//  Sample the luminance of the source image using a kernal of sample
//  points, and return a scaled image containing the log() of averages
// examples (ignored 0.0001 in log())
// scene tex
// (e,   e,   e,   1.0) -> log(e)   -> fLogLumSum +=  1
// (1,   1,   1,   1.0) -> log(1)   -> fLogLumSum +=  0
// (1/e, 1/e, 1/e, 1.0) -> log(1/e) -> fLogLumSum += -1
//-----------------------------------------------------------------------------
"float4 SampleLumInitial( in float2 vScreenPosition : TEXCOORD0 ) : COLOR\n"\
"{"\
	"float3 vSample = 0.0f;"\
	"float  fLogLumSum = 0.0f;"\

	"for(int i=0; i<9; i++)"\
	"{"\
		// Compute the sum of log(luminance) throughout the sample points
		// - Returned to (luminance) by exp() in SampleLumFinal()
		"vSample = tex2D(s0, vScreenPosition+g_avSampleOffsets[i]);"\
		"fLogLumSum += log(dot(vSample, LUMINANCE_VECTOR)+0.0001f);"\
	"}"\

	// Divide the sum to complete the average
	"fLogLumSum /= 9;"\

	"return float4(fLogLumSum, fLogLumSum, fLogLumSum, 1.0f);"\
"}\n"\

// Scale down the luminance texture by blending sample points
"float4 SampleLumIterative( in float2 vScreenPosition : TEXCOORD0 ) : COLOR\n"\
"{"\
	"float fResampleSum = 0.0f;"\

	"for(int i=0; i<16; i++)"\
	"{"\
		// Compute the sum of luminance throughout the sample points
		"fResampleSum += tex2D(s0, vScreenPosition+g_avSampleOffsets[i]);"\
	"}"\

	// Divide the sum to complete the average
	"fResampleSum /= 16;"\

	"return float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);"\
"}\n"\

// Extract the average luminance of the image by completing the averaging
// and taking the exp() of the result
"float4 SampleLumFinal( in float2 vScreenPosition : TEXCOORD0 ) : COLOR\n"\
"{"\
	"float fResampleSum = 0.0f;"\

	"for(int i=0; i<16; i++)"\
	"{"\
		// Compute the sum of luminance throughout the sample points
		"fResampleSum += tex2D(s0, vScreenPosition+g_avSampleOffsets[i]);"\
	"}"\

	// Divide the sum to complete the average, and perform an exp() to complete
	// the average luminance calculation
	"fResampleSum = exp(fResampleSum/16);"\

	"return float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);"\
"}\n"\

// Calculate the luminance that the camera is current adapted to, using
// the most recented adaptation level, the current scene luminance, and
// the time elapsed since last calculated
"float4 CalculateAdaptedLumPS( in float2 vScreenPosition : TEXCOORD0 ) : COLOR\n"
"{"
	"float fAdaptedLum = tex2D(s0, float2(0.5f, 0.5f));"\
	"float fCurrentLum = tex2D(s1, float2(0.5f, 0.5f));"\

	// >>> comment in original D3D sample
	// The user's adapted luminance level is simulated by closing the gap between
	// adapted luminance and current luminance by (g_fAdaptationRate*100)% every frame, based on a
	// 30 fps rate. This is not an accurate model of human adaptation, which can
	// take longer than half an hour.
	// <<< comment in original D3D sample
	// In the original D3D sample g_fAdaptationRate was hard-coded to 0.02.
	"float f = ( 1 - pow( 1.0f - g_fAdaptationRate, 30 * g_fElapsedTime ) );"\
	"float fNewAdaptation = fAdaptedLum + (fCurrentLum - fAdaptedLum) * f;"\
	"return float4(fNewAdaptation, fNewAdaptation, fNewAdaptation, 1.0f);"\
"}\n"\

// Perform blue shift, tone map the scene, and add post-processed light effects
"float4 FinalScenePassPS( in float2 vScreenPosition : TEXCOORD0 ) : COLOR\n"\
"{"\
	"float4 vSample    = tex2D(s0, vScreenPosition);"\
	"float4 vBloom     = tex2D(s1, vScreenPosition);"\
	"float4 vStar      = tex2D(s2, vScreenPosition);"\
	"float fAdaptedLum = tex2D(s3, float2(0.5f, 0.5f));\n"\

	// For very low light conditions, the rods will dominate the perception
	// of light, and therefore color will be desaturated and shifted
	// towards blue.
	"if( g_bEnableBlueShift )"\
	"{"\
		// Define a linear blending from -1.5 to 2.6 (log scale) which
		// determines the lerp amount for blue shift
		"float fBlueShiftCoefficient = 1.0f - (fAdaptedLum + 1.5)/4.1;"\
		"fBlueShiftCoefficient = saturate(fBlueShiftCoefficient);"\

		// Lerp between current color and blue, desaturated copy
		"float3 vRodColor = dot( (float3)vSample, LUMINANCE_VECTOR ) * BLUE_SHIFT_VECTOR;"\
		"vSample.rgb = lerp( (float3)vSample, vRodColor, fBlueShiftCoefficient );"\
	"}\n"\


	// Map the high range of color values into a range appropriate for
	// display, taking into account the user's adaptation level, and selected
	// values for for middle gray and white cutoff.
	"if( g_bEnableToneMap )"\
	"{"\
		"vSample.rgb *= g_fMiddleGray/(fAdaptedLum + 0.001f);"\
		"vSample.rgb /= (1.0f+vSample);"\
	"}\n"\

	// Add the star and bloom post processing effects
	"if( g_bRenderStar )"\
		"vSample += g_fStarScale * vStar;\n"

	"vSample += g_fBloomScale * vBloom;\n"\

	"return vSample;"\
//	return float4(fAdaptedLum,fAdaptedLum,fAdaptedLum,1.0f);
"}\n"\

// Perform a high-pass filter on the source texture
"float4 BrightPassFilterPS( in float2 vScreenPosition : TEXCOORD0 ) : COLOR\n"\
"{"\
	"float4 vSample = tex2D( s0, vScreenPosition );"\
	"float  fAdaptedLum = tex2D( s1, float2(0.5f, 0.5f) );"\
//	float  fAdaptedLum = 0.03f;

	// Determine what the pixel's value will be after tone-mapping occurs
	"vSample.rgb *= g_fMiddleGray/(fAdaptedLum + 0.001f);"\

	// Subtract out dark pixels
	"vSample.rgb -= BRIGHT_PASS_THRESHOLD;"\

	// Clamp to 0
	"vSample = max(vSample, 0.0f);"\

	// Map the resulting value into the 0 to 1 range. Higher values for
	// BRIGHT_PASS_OFFSET will isolate lights from illuminated scene 
	// objects.
	"vSample.rgb /= (BRIGHT_PASS_OFFSET+vSample);"\

	"return vSample;"\
//	return float4(fAdaptedLum,fAdaptedLum,fAdaptedLum,1.0f);
"}\n"\

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
"}\n";


void CEmbeddedPostProcessEffectHLSLShader::GetCombinedShader( std::string& shader )
{
	shader += m_pMonochrome;   // "monochrome"
	shader += m_pBloom;        // "bloom"
	shader += m_pDownScale2x2; // "down_scale_2x2"
	shader += m_pDownScale4x4; // "down_scale_4x4"
	shader += m_pGaussBlur5x5; // "gauss_blur_5x5"
	shader += m_pHDR;          // "hdr"
}


Result::Name CEmbeddedPostProcessEffectHLSLShader::GenerateShader( const std::string& effect_name, std::string& shader )
{
	shader = CEmbeddedPostProcessEffectHLSLShader::m_pTextureSamplers;

	shader += CEmbeddedPostProcessEffectHLSLShader::m_pSampleOffsetsAndWeights;

	if( effect_name.length() == 0 )
	{
		GetCombinedShader( shader );
		return Result::SUCCESS;
	}

	if( effect_name == "monochrome" )          shader += m_pMonochrome;
	else if( effect_name == "bloom" )          shader += m_pBloom;
	else if( effect_name == "down_scale_2x2" ) shader += m_pDownScale2x2;
	else if( effect_name == "down_scale_4x4" ) shader += m_pDownScale4x4;
	else if( effect_name == "gauss_blur_5x5" ) shader += m_pGaussBlur5x5;
	else if( effect_name == "hdr" )            shader += m_pHDR;
	else
	{
		LOG_PRINT_WARNING( "An unsupported effect name: " + effect_name );
		return Result::INVALID_ARGS;
	}

	return Result::SUCCESS;
}


void CPostProcessEffectFilterShaderGenerator::GetShader( std::string& shader )
{
	GetEmbeddedPostProcessEffectShader()->GenerateShader( m_EffectName.c_str(), shader );
}


void CPostProcessEffectFilterShaderGenerator::GetVertexShader( std::string& shader )
{
	GetEmbeddedPostProcessEffectShader()->GenerateVertexShader( m_EffectName.c_str(), shader );
}


void CPostProcessEffectFilterShaderGenerator::GetPixelShader( std::string& shader )
{
	GetEmbeddedPostProcessEffectShader()->GenerateFragmentShader( m_EffectName.c_str(), shader );
}


} // namespace amorphous
