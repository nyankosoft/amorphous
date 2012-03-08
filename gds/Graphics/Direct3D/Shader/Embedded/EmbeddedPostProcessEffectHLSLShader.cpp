#include "EmbeddedPostProcessEffectHLSLShader.hpp"


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


//-----------------------------------------------------------------------------
// Name: BloomPS
// Type: Pixel shader                                      
// Desc: Blur the source image along one axis using a gaussian
//       distribution. Since gaussian blurs are separable, this shader is called 
//       twice; first along the horizontal axis, then along the vertical axis.
//-----------------------------------------------------------------------------
const char *CEmbeddedPostProcessEffectHLSLShader::m_pBloom =
"static const int MAX_SAMPLES = 16;\n"\
"float2 g_avSampleOffsets[MAX_SAMPLES];\n"\
"float4 g_avSampleWeights[MAX_SAMPLES];\n"\
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
"static const int MAX_SAMPLES = 4;\n"\
"float2 g_avSampleOffsets[MAX_SAMPLES];\n"\
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
"static const int MAX_SAMPLES = 16;\n"\
"float2 g_avSampleOffsets[MAX_SAMPLES];\n"\
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
"static const int MAX_SAMPLES = 16;\n"\
"float2 g_avSampleOffsets[MAX_SAMPLES];\n"\
"float4 g_avSampleWeights[MAX_SAMPLES];\n"\
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



void InitPPEffectShader()
{
	CShaderResourceDesc shader_desc;
	shader_desc.pShaderGenerator.reset( new CPostProcessEffectFilterShaderGenerator("monochrome") );
}


void CPostProcessEffectFilterShaderGenerator::GetShader( std::string& shader )
{
	shader = CEmbeddedPostProcessEffectHLSLShader::m_pTextureSamplers;

	if( m_EffectName == "monochrome" )          shader += CEmbeddedPostProcessEffectHLSLShader::m_pMonochrome;
	else if( m_EffectName == "bloom" )          shader += CEmbeddedPostProcessEffectHLSLShader::m_pBloom;
	else if( m_EffectName == "down_scale_2x2" ) shader += CEmbeddedPostProcessEffectHLSLShader::m_pDownScale2x2;
	else if( m_EffectName == "down_scale_4x4" ) shader += CEmbeddedPostProcessEffectHLSLShader::m_pDownScale4x4;
	else if( m_EffectName == "gauss_blur_5x5" ) shader += CEmbeddedPostProcessEffectHLSLShader::m_pGaussBlur5x5;
//	else if( m_EffectName == "hdr" )            shader += CEmbeddedPostProcessEffectHLSLShader::m_p???;
	else
		LOG_PRINT_WARNING( "An unsupported effect name: " + m_EffectName );
}
