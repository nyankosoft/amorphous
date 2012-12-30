#include "EmbeddedMiscHLSL.hpp"


namespace amorphous
{


const char *CEmbeddedMiscHLSL::ms_pSingleDiffuseColor =
"float4 DiffuseColor = float4(1,1,1,1);"\
"float4x4 WorldViewProj : WORLDVIEWPROJ;\n"\
"void VS( float4 Pos : POSITION,"\
         "out float4 oPos : POSITION )"\
"{ oPos = mul( Pos, WorldViewProj ); }\n"\
"void PS( out float4 Color : COLOR ) { Color = DiffuseColor; }\n";


const char *CEmbeddedMiscHLSL::ms_pShadedSingleDiffuseColor =
"float4 DiffuseColor = float4(1,1,1,1);"\
"float3 LightDirection = float3(0,-1,0);"\
"float4x4 World : WORLD;\n"\
"float4x4 WorldViewProj : WORLDVIEWPROJ;\n"\
"void VS( float4 Pos : POSITION,"\
         "float3 Normal : NORMAL,"\
         "out float4 oPos : POSITION,"\
         "out float3 WorldNormal : TEXCOORD0 )"\
"{"\
	"oPos = mul( Pos, WorldViewProj );"\
	"WorldNormal = mul( Normal, (float3x3)World );"\
"}\n"\
"void PS( float3 WorldNormal : TEXCOORD0, out float4 Color : COLOR )"\
"{"\
	"WorldNormal = normalize(WorldNormal);"\
	"float d = -dot( LightDirection, WorldNormal );"\

	// clamp to [0,1]
	"d = (d + 1.0) * 0.5;"\

	// clamp to [0.3,1.0]
	"d = (1.0-0.3) * d + 0.3;"\

	"Color.rgb = DiffuseColor.rgb * d;"\
	"Color.a   = DiffuseColor.a;"\
"}\n";

const char *CEmbeddedMiscHLSL::ms_pVertexWeightMapDisplay =
"float4x4 WorldViewProj : WORLDVIEWPROJ;\n"\
"float4x4 World			: WORLD;\n"\
"#define NUM_MAX_COLORS 22\n"\
"float4 g_Colors[NUM_MAX_COLORS] ="\
"{"\
	"float4(1.00,0.25,0.25,1),"\
	"float4(0.25,1.00,0.25,1),"\
	"float4(0.25,0.25,1.00,1),"\
	"float4(1.00,0.25,1.00,1),"\
	"float4(0.25,1.00,1.00,1),"\
	"float4(1.00,0.25,1.00,1),"\
	"float4(0.25,0.25,0.25,1),"\
	"float4(1.0,0.5,0.5,1),"\
	"float4(0.5,1.0,0.5,1),"\
	"float4(0.5,0.5,1.0,1),"\
	"float4(1.0,0.5,1.0,1),"\
	"float4(0.5,1.0,1.0,1),"\
	"float4(1.0,0.5,1.0,1),"\
	"float4(0.5,0.5,0.5,1),"\
	"float4(1.00,0.75,0.75,1),"\
	"float4(0.75,1.00,0.75,1),"\
	"float4(0.75,0.75,1.00,1),"\
	"float4(1.00,0.75,1.00,1),"\
	"float4(0.75,1.00,1.00,1),"\
	"float4(1.00,0.75,1.00,1),"\
	"float4(0.75,0.75,0.75,1),"\
	"float4(1,1,1,1)"\
"};\n"\
"float3 g_LightDirection = float3(0,-1,0);"\
"float4x4 WorldView      : WORLDVIEW;\n"\
"void VS( float4 Pos : POSITION,"\
         "float4 BlendWeights : BLENDWEIGHT,"\
         "int4 BlendIndices : BLENDINDICES,"\
         "float3 Normal : NORMAL,"\
         "out float4 oPos : POSITION,"\
         "out float3 oNormal : TEXCOORD0,"\
		 "out float4 oDiffuse : COLOR0 )"\
"{"\
	"oPos = mul( Pos, WorldViewProj );"\
	"oNormal = mul( Normal, (float3x3)World );"\
	"oDiffuse = g_Colors[BlendIndices[0]%NUM_MAX_COLORS];"\
"}\n"\
"void PS( float4 Diffuse : COLOR0, float3 Normal : TEXCOORD0, out float4 Color : COLOR )"\
"{"\
	"Normal = normalize(Normal);"\
	"Color = Diffuse * (dot(Normal,-g_LightDirection) + 1.0) * 0.5;"\
"}\n";

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
"}\n";

const char *CEmbeddedMiscHLSL::ms_pTechniqueTemplate =
"technique Default"\
"{"\
	"pass P0"\
	"{"\
		"VertexShader = compile vs_2_0 VS();"\
		"PixelShader  = compile ps_2_0 PS();"\

		"CullMode = Ccw;"\
		"ZEnable = True;"\
		"AlphaBlendEnable = False;"\
	"}"\
"}\n";


Result::Name CEmbeddedMiscHLSL::GetShader( CEmbeddedMiscShader::ID shader_id, std::string& hlsl_effect )
{
	switch( shader_id )
	{
	case CEmbeddedMiscShader::SINGLE_DIFFUSE_COLOR:
		hlsl_effect = ms_pSingleDiffuseColor;
		hlsl_effect += ms_pTechniqueTemplate;
		return Result::SUCCESS;

	case CEmbeddedMiscShader::SHADED_SINGLE_DIFFUSE_COLOR:
		hlsl_effect = ms_pShadedSingleDiffuseColor;
		hlsl_effect += ms_pTechniqueTemplate;
		return Result::SUCCESS;

	case CEmbeddedMiscShader::VERTEX_WEIGHT_MAP_DISPLAY:
		hlsl_effect = ms_pVertexWeightMapDisplay;
		hlsl_effect += ms_pTechniqueTemplate;
		return Result::SUCCESS;

	case CEmbeddedMiscShader::DEPTH_RENDERING_IN_VIEW_SPACE:
		hlsl_effect = ms_pDepthRenderingInViewSpace;
		hlsl_effect += ms_pDepthRenderingPixelShaderAndTechnique;
		hlsl_effect += ms_pTechniqueTemplate;
		return Result::SUCCESS;

	case CEmbeddedMiscShader::DEPTH_RENDERING_IN_PROJECTION_SPACE:
		hlsl_effect = ms_pDepthRenderingInProjectionSpace;
		hlsl_effect += ms_pDepthRenderingPixelShaderAndTechnique;
		hlsl_effect += ms_pTechniqueTemplate;
		return Result::SUCCESS;

//	case MiscShader::ANOTHER_MISC_SHADER:
		// Replace #include lines if necessary
//		return;

	default:
		return Result::INVALID_ARGS;
	}
}


} // namespace amorphous
