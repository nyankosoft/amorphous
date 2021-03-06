#include "EmbeddedHLSLShader.hpp"
#include <stdio.h>


namespace amorphous
{


/**
Naming scheme

ms_{VS|PS}_{PPL|PVL}_{HSLs|...}{_QVertexBlend}{_Specular}
VS|PS: vertex shader / pixel shader
PPL|PVL: per-pixel lighting / per-vertex lighting

_QVertexBlend: appended if the shader includes calculations related to vertex blending with quaternion and float3
_Specular: appended if the shader includes calculations related to specular reflection
*/


EmbeddedHLSLShader EmbeddedHLSLShaders::ms_VS_PVL_HSLs_QVertexBlend = EmbeddedHLSLShader
(
"VS_PVL_HSLs_QVertexBlend",
"VS_PVL_HSLs_QVertexBlend()"\
"{"\
"}"\
);


EmbeddedHLSLShader EmbeddedHLSLShaders::ms_PS_PVL_HSLs_QVertexBlend = EmbeddedHLSLShader
(
"PS_PVL_HSLs_QVertexBlend",
"PS_PVL_HSLs_QVertexBlend()"\
"{"\
"}"\
);


EmbeddedHLSLShader EmbeddedHLSLShaders::ms_VS_PVL_HSLs = EmbeddedHLSLShader
(
"VS_PVL_HSLs",
"VS_PVL_HSLs()"\
"{"\
"}"\
);


EmbeddedHLSLShader EmbeddedHLSLShaders::ms_PS_PVL_HSLs = EmbeddedHLSLShader
(
"PS_PVL_HSLs",
"PS_PVL_HSLs()"\
"{"\
"}"\
);


EmbeddedHLSLShader EmbeddedHLSLShaders::ms_VS_PVL_HSLs_Specular = EmbeddedHLSLShader
(
"VS_PVL_HSLs_Specular",
"VS_PVL_HSLs_Specular()"\
"{"\
"}"\
);


EmbeddedHLSLShader EmbeddedHLSLShaders::ms_PS_PVL_HSLs_Specular = EmbeddedHLSLShader
(
"PS_PVL_HSLs_Specular",
"PS_PVL_HSLs_Specular()"\
"{"\
"}"\
);

EmbeddedHLSLShader EmbeddedHLSLShaders::ms_VS_PPL_HSLs = EmbeddedHLSLShader
(
// name
"VS_PPL_HSLs",

// content
"#include \"Matrix.fxh\""\

"void VS_PPL_HSLs("\
	"float4 vModelSpacePos     : POSITION,"\
	"float3 vModelSpaceNormal  : NORMAL,"\
	"float4 vDiffuse           : COLOR0,"\
	"float2 vTex0              : TEXCOORD0,"\
	"out float4 oPos      : POSITION,"\
	"out float4 oDiffuse  : COLOR0,"\
	"out float2 oTex0     : TEXCOORD0,"\
	"out float3 oPosWS    : TEXCOORD1,"\
	"out float3 oNormalWS : TEXCOORD2,"\
	// for sampling a mirrored scene texture to render planer reflection surface
	"out float4 oPosPS    : TEXCOORD3"\
	")"\
"{"\
	"oPos         = mul( vModelSpacePos, WorldViewProj );"\
	"float4 PosWS = mul( vModelSpacePos, World );"\
	"oPosWS       = PosWS.xyz / PosWS.w;"\
	"oNormalWS    = mul( vModelSpaceNormal, (float3x3)World );"\
	"oDiffuse     = vDiffuse;"\
	"oTex0        = vTex0;"\
	"oPosPS       = oPos;"\
"}\n"\
);


EmbeddedHLSLShader EmbeddedHLSLShaders::ms_PS_PPL_HSLs = EmbeddedHLSLShader
(
"PS_PPL_HSLs",
"#include \"LightDef.fxh\"\n"\
"#include \"TexDef.fxh\"\n"\

"#define NUM_LIGHTS	5\n"\
"CLight g_aLight[NUM_LIGHTS];"\

"float4 g_vAmbientColor = float4(0,0,0,0);"\


"[[OPTIONS_MACROS]]\n"


"float4 PS_PPL_HSLs( float4 ColorDiff : COLOR0,"\
                "float2 Tex0     : TEXCOORD0,"\
                "float3 PosWS    : TEXCOORD1,"\
				"float3 NormalWS : TEXCOORD2,"\
				// for sampling a mirrored scene texture to render planer reflection surface
				"float4 PosPS    : TEXCOORD3"\
				") : COLOR0"\
"{"\
	"\nPLANAR_REFLECTION\n"\

	"float4 surface_color = tex2D( Sampler0, Tex0 ) * ColorDiff;"\

	// normal in world space
	"const float3 Normal = normalize(NormalWS);"\

	"float4 diffuse = 0;"\
	"int i;"\
	"[unroll(1)]"\
	"for( i=iLightDirIni; i<iLightDirIni+iLightDirNum; i++ )"\
	"{"\
		"float diff_raw = dot(Normal,-g_aLight[i].vDir);"\
		"float diff_hs = ( diff_raw + 1.0 ) * 0.5;"\

		"diffuse"\
			"+= g_aLight[i].vAmbient"\
			 "+ g_aLight[i].vDiffuseUpper * diff_hs"\
			 "+ g_aLight[i].vDiffuseLower * ( 1.0 - diff_hs );"\
	"}"\

	"[unroll(2)]"\
	"for( i=iLightPointIni; i<iLightPointIni+iLightPointNum; i++ )"\
	"{"\
		"float3 vToLightWS = g_aLight[i].vPos-PosWS;"\
		"float d = length(vToLightWS);"\
		"float3 vDirToLight = vToLightWS / d;"\
		"float diff_raw = dot(Normal,vDirToLight);"\
		"float diff_hs = ( diff_raw + 1.0 ) * 0.5;"\
		"float inv_att"\
			"= g_aLight[i].vAttenuation.x"\
			"+ g_aLight[i].vAttenuation.y * d"\
			"+ g_aLight[i].vAttenuation.z * d*d;"\

		"float4 diffuse_of_this_light"\
			"= g_aLight[i].vAmbient"\
			"+ g_aLight[i].vDiffuseUpper * diff_hs"\
			"+ g_aLight[i].vDiffuseLower * ( 1.0 - diff_hs );"\

		"float att = d < g_aLight[i].fRange ? 1.0 / inv_att : 0.0;"\

//		diffuse_of_this_light = clamp( diffuse_of_this_light, 0.1, 1.0 );

		"diffuse += diffuse_of_this_light * att;"\
	"}"\

	"float4 color;"\
	"color.rgb = surface_color.rgb * diffuse.rgb + g_vAmbientColor.rgb;"\
	"color.a = surface_color.a;"\

	"\nENVMAP;\n"\

	"\nPLANAR_REFLECTION_COLOR_UPDATE\n"\

	"return color;"\
"}\n"\
);


EmbeddedHLSLShader EmbeddedHLSLShaders::ms_VS_PPL_HSLs_Specular = EmbeddedHLSLShader
(
"VS_PPL_HSLs_Specular",
"#include \"Matrix.fxh\"\n"\

//================================= Vertex Shader =================================

"#ifdef BUMPMAP\n"\
"#define BUMPMAP_VS_IN  float3 vModelSpaceTangent : TANGENT,\n"\
"#define BUMPMAP_VS     oTangentWS  = mul( vModelSpaceBinormal,(float3x3)World );\n"\
"#define BUMPMAP_VS_OUT ,out float3 oTangentWS : TEXCOORD2\n"\
"#else\n"\
"#define BUMPMAP_VS_IN\n"\
"#define BUMPMAP_VS\n"\
"#define BUMPMAP_VS_OUT\n"\
"#endif\n"\

"void VS_PPL_HSLs_Specular("\
	"float4 vModelSpacePos     : POSITION,"\
	"float3 vModelSpaceNormal  : NORMAL,"\
	"BUMPMAP_VS_IN "\
	"float4 vDiffuse           : COLOR0,"\
	"float2 vTex0              : TEXCOORD0,"\
	"out float4 oPos       : POSITION,"\
	"out float4 oDiffuse   : COLOR0,"\
	"out float2 oTex0      : TEXCOORD0,"\
	"out float3 oPosWS     : TEXCOORD1,"\
	"out float3 oNormalWS  : TEXCOORD2 "\
	"BUMPMAP_VS_OUT"\
	" )"\
"{"\
	"oPos         = mul( vModelSpacePos, WorldViewProj );"\
	"float4 PosWS = mul( vModelSpacePos, World );"\
	"oPosWS       = PosWS.xyz / PosWS.w;"\
	"oNormalWS    = mul( vModelSpaceNormal,  (float3x3)World );"\
	"BUMPMAP_VS\n"\
	"oDiffuse     = vDiffuse;"\
	"oTex0        = vTex0;"\
/*	"float3 vTangentWS = cross( oBinormalWS, oNormalWS );"\
	"float4 q = quaternion_from_axes( vTangentWS, oBinormalWS, oNormalWS );*/
	
"}\n"\
);


EmbeddedHLSLShader EmbeddedHLSLShaders::ms_PS_PPL_HSLs_Specular = EmbeddedHLSLShader
(
"PS_PPL_HSLs_Specular",
"#include \"TexDef.fxh\"\n"\
"#include \"LightDef.fxh\"\n"\
"#include \"ColorPair.fxh\"\n"\

"#define NUM_LIGHTS	5\n"\
"CLight g_aLight[NUM_LIGHTS];"\

"float g_fSpecular = 0.8f;"\
"float g_fSpecularPower = 8.0f;"\

"\n"\
"#include \"PerPixelHSLighting_Specular.fxh\"\n"\


"float4 g_vAmbientColor = float4(0,0,0,0);"\
"float3 g_vEyePos = float3(0,0,0);"\


"[[OPTIONS_MACROS]]\n"

"#ifdef BUMPMAP\n"\
	"#define BUMPMAP_PS_IN ,float3 vTangentWS : TEXCOORD3\n"\
	"#define CALCULATE_WORLD_NORMAL  "\
	"float4 normal_map = tex2D( BUMPMAP_SAMPLER, Tex0 );"\
	"float3 vNormalTS = normal_map.xyz * 2.0 - float3(1,1,1);"\
	"vNormalWS = normalize(vNormalWS);"\
	"float3 vBinormalWS = cross( vNormalWS, vTangentWS );"\
	"vBinormalWS = normalize(vBinormalWS);"\
	"vTangentWS = cross( vBinormalWS, vNormalWS );"\
	"const float3 Normal = float3( "\
	"dot( vNormalTS, vTangentWS ), "\
	"dot( vNormalTS, vBinormalWS ), "\
	"dot( vNormalTS, vNormalWS ) );\n"\
"#else\n"\
	"#define BUMPMAP_PS_IN\n"\
	"#define CALCULATE_WORLD_NORMAL  const float3 Normal = normalize(vNormalWS);\n"\
"#endif\n"\


"float4 PS_PPL_HSLs_Specular( float4 ColorDiff : COLOR0,"\
                "float2 Tex0       : TEXCOORD0,"\
                "float3 PosWS      : TEXCOORD1,"\
				"float3 vNormalWS  : TEXCOORD2"\
				"\nBUMPMAP_PS_IN"\
				") : COLOR0"
"{"\
	"\nPLANAR_REFLECTION\n"\

	"float4 surface_color = tex2D( Sampler0, Tex0 ) * ColorDiff;\n"\

	"float specular_power = GET_SPECULAR_POWER;"\

	// normal in world space stored to Normal
	"\nCALCULATE_WORLD_NORMAL\n"\

	"COLOR_PAIR Out;"\
	"Out.Color = 0;"\
	"Out.ColorSpec = 0;"\

	"float3 vDirToViewerWS = normalize(g_vEyePos - PosWS);"\

	"int i = 0;"\

	// directional lights
	"[unroll(1)]"\
	"for(i = 0; i < iLightDirNum; i++)"\
	"{"\
		"COLOR_PAIR ColOut = DoHemisphericDirLight_Specular( Normal, vDirToViewerWS, i+iLightDirIni, specular_power );"\
		"Out.Color     += ColOut.Color;"\
		"Out.ColorSpec += ColOut.ColorSpec;"\
	"}"\

	// point lights
	"[unroll(1)]"\
	"for(i = 0; i < iLightPointNum; i++)"\
	"{"\
		"COLOR_PAIR ColOut = DoHemisphericPointLight_Specular( PosWS, Normal, vDirToViewerWS, i+iLightPointIni, specular_power );"\
		"Out.Color     += ColOut.Color;"\
		"Out.ColorSpec += ColOut.ColorSpec;"\
	"}"\

	"float4 color;"\
	"color.rgb = surface_color * Out.Color + Out.ColorSpec * GET_SPECULAR + g_vAmbientColor;"\

	"\nENVMAP;\n"\

	"\nPLANAR_REFLECTION_COLOR_UPDATE\n"\

	"color.a = GET_ALPHA;"\
	"return color;"\
"}\n"
);


EmbeddedHLSLShader EmbeddedHLSLShaders::ms_VS_PPL_HSLs_QVertexBlend_Specular = EmbeddedHLSLShader
(
"VS_PPL_HSLs_QVertexBlend_Specular",
"#define NUM_MAX_BLEND_TRANSFORMS	72\n"\
"#define NUM_MAX_BLEND_TRANSFORMS_PER_VERTEX	2\n"\
"#include \"Matrix.fxh\"\n"\
"#include \"Quaternion.fxh\"\n"\
"#include \"Transform.fxh\"\n"\

"Transform g_aBlendTransform[NUM_MAX_BLEND_TRANSFORMS];\n"\
"#include \"QVertexBlendFunctions.fxh\"\n"\

//================================= Vertex Shader =================================

"void VS_PPL_HSLs_QVertexBlend_Specular("\
	"float4 vModelSpacePos     : POSITION,"\
	"float3 vModelSpaceNormal  : NORMAL,"\
	"float4 vDiffuse           : COLOR0,"\
	"float2 vTex0              : TEXCOORD0,"\
	"int4   indices            : BLENDINDICES,"\
	"float4 weights            : BLENDWEIGHT,"\
	"out float4 oPos      : POSITION,"\
	"out float4 oDiffuse  : COLOR0,"\
	"out float2 oTex0     : TEXCOORD0,"\
	"out float3 oPosWS    : TEXCOORD1,"\
	"out float3 oNormalWS : TEXCOORD2 )"\
"{"\
	// pos is transformed to model space
	"SkinPoint( vModelSpacePos,     g_aBlendTransform, indices, weights);"\

	// normal is transformed to model space
	"SkinVector( vModelSpaceNormal, g_aBlendTransform, indices, weights);"\

	"oPos         = mul( vModelSpacePos, WorldViewProj );"\
	"float4 PosWS = mul( vModelSpacePos, World );"\
	"oPosWS       = PosWS.xyz / PosWS.w;"\
	"oNormalWS    = mul( vModelSpaceNormal, (float3x3)World );"\
	"oDiffuse     = vDiffuse;"\
	"oTex0        = vTex0;"\
"}\n"\
);


/*
// Used for compile test
int main()// { return 0; }
{
	printf( "name:    %s\n", EmbeddedHLSLShaders::ms_PS_PVL_HSLs_Specular.pName );
	printf( "content: %s\n", EmbeddedHLSLShaders::ms_PS_PVL_HSLs_Specular.pContent );
}
*/


} // namespace amorphous
