#include "EmbeddedMiscHLSL.hpp"


const char *CEmbeddedMiscHLSL::ms_pDepthRenderingInViewSpace =
"float4x4 WorldViewProj : WORLDVIEWPROJ;\n"\

"void VS( float4 Pos : POSITION,"\
         "out float4 oPos : POSITION,"\
         "out float2 Depth : TEXCOORD0 )"\
"{"\
	// Compute the projected coordinates
	"oPos = mul( Pos, WorldViewProj );"\

	// Store z and w in our spare texcoord
	"Depth.xy = oPos.zw;"\
"}\n";

const char *CEmbeddedMiscHLSL::ms_pDepthRenderingInProjectionSpace =
"float4x4 WorldView : WORLDVIEW;\n"\
"float4x4 Proj : PROJ;"\
"void VS( float4 Pos : POSITION,"\
         "out float4 oPos : POSITION,"\
         "out float2 Depth : TEXCOORD0 )"\
"{"\
    // Compute the projected coordinates
    "float4 PosVS = mul( Pos, WorldView );"\
    "oPos = mul( PosVS, Proj );"\

    // Store z and w in our spare texcoord
    "Depth.xy = oPos.zw;"\
"}\n";

const char *CEmbeddedMiscHLSL::ms_pDepthRenderingPixelShaderAndTechnique =
"void PS( float2 Depth : TEXCOORD0,"\
         "out float4 Color : COLOR )"\
"{"\
	// Depth is z / w
	"Color = Depth.x / Depth.y;"\
	"Color.g = Color.b = 0;"\
"}\n"

"technique Default"\
"{"\
	"pass P0"\
	"{"\
		"VertexShader = compile vs_2_0 VS();"\
		"PixelShader  = compile vs_2_0 PS();"\

		"CullMode = Ccw;"\
		"ZEnable = True;"\
		"AlphaBlendEnable = False;"\
	"}"\
"}\n";


Result::Name CEmbeddedMiscHLSL::GetShader( CEmbeddedMiscShader::ID shader_id, std::string& hlsl_effect )
{
	switch( shader_id )
	{
	case CEmbeddedMiscShader::DEPTH_RENDERING_IN_VIEW_SPACE:
		hlsl_effect = ms_pDepthRenderingInViewSpace;
		hlsl_effect += ms_pDepthRenderingPixelShaderAndTechnique;
		return Result::SUCCESS;

	case CEmbeddedMiscShader::DEPTH_RENDERING_IN_PROJECTION_SPACE:
		hlsl_effect = ms_pDepthRenderingInProjectionSpace;
		hlsl_effect += ms_pDepthRenderingPixelShaderAndTechnique;
		return Result::SUCCESS;

//	case MiscShader::ANOTHER_MISC_SHADER:
		// Replace #include lines if necessary
//		return;

	default:
		return Result::INVALID_ARGS;
	}
}
