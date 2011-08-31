#include "EmbeddedHLSLShader.hpp"
#include <stdio.h>


/**
Naming scheme

ms_{VS|PS}_{PPL|PVL}_{HSLs|...}{_QVertexBlend}{_Specular}
VS|PS: vertex shader / pixel shader
PPL|PVL: per-pixel lighting / per-vertex lighting

_QVertexBlend: appended if the shader includes calculations related to vertex blending with quaternion and float3
_Specular: appended if the shader includes calculations related to specular reflection
*/


CEmbeddedHLSLShader CEmbeddedHLSLShaders::ms_VS_PVL_HSLs_QVertexBlend = CEmbeddedHLSLShader
(
"VS_PVL_HSLs_QVertexBlend",
"VS_PVL_HSLs_QVertexBlend()"\
"{"\
"}"\
);


CEmbeddedHLSLShader CEmbeddedHLSLShaders::ms_PS_PVL_HSLs_QVertexBlend = CEmbeddedHLSLShader
(
"PS_PVL_HSLs_QVertexBlend",
"PS_PVL_HSLs_QVertexBlend()"\
"{"\
"}"\
);


CEmbeddedHLSLShader CEmbeddedHLSLShaders::ms_VS_PVL_HSLs = CEmbeddedHLSLShader
(
"VS_PVL_HSLs",
"VS_PVL_HSLs()"\
"{"\
"}"\
);


CEmbeddedHLSLShader CEmbeddedHLSLShaders::ms_PS_PVL_HSLs = CEmbeddedHLSLShader
(
"PS_PVL_HSLs",
"PS_PVL_HSLs()"\
"{"\
"}"\
);


CEmbeddedHLSLShader CEmbeddedHLSLShaders::ms_VS_PVL_HSLs_Specular = CEmbeddedHLSLShader
(
"VS_PVL_HSLs_Specular",
"VS_PVL_HSLs_Specular()"\
"{"\
"}"\
);


CEmbeddedHLSLShader CEmbeddedHLSLShaders::ms_PS_PVL_HSLs_Specular = CEmbeddedHLSLShader
(
"PS_PVL_HSLs_Specular",
"PS_PVL_HSLs_Specular()"\
"{"\
"}"\
);

CEmbeddedHLSLShader CEmbeddedHLSLShaders::ms_VS_PPL_HSLs = CEmbeddedHLSLShader
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


CEmbeddedHLSLShader CEmbeddedHLSLShaders::ms_PS_PPL_HSLs = CEmbeddedHLSLShader
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


CEmbeddedHLSLShader CEmbeddedHLSLShaders::ms_VS_PPL_HSLs_Specular = CEmbeddedHLSLShader
(
"VS_PPL_HSLs_Specular",
"#include \"Matrix.fxh\"\n"\

//================================= Vertex Shader =================================

"void VS_PPL_HSLs_Specular("\
	"float4 vModelSpacePos     : POSITION,"\
	"float3 vModelSpaceNormal  : NORMAL,"\
	"float4 vDiffuse           : COLOR0,"\
	"float2 vTex0              : TEXCOORD0,"\
	"out float4 oPos      : POSITION,"\
	"out float4 oDiffuse  : COLOR0,"\
	"out float2 oTex0     : TEXCOORD0,"\
	"out float3 oPosWS    : TEXCOORD1,"\
	"out float3 oNormalWS : TEXCOORD2 )"\
"{"\
	"oPos         = mul( vModelSpacePos, WorldViewProj );"\
	"float4 PosWS = mul( vModelSpacePos, World );"\
	"oPosWS       = PosWS.xyz / PosWS.w;"\
	"oNormalWS    = mul( vModelSpaceNormal, (float3x3)World );"\
	"oDiffuse     = vDiffuse;"\
	"oTex0        = vTex0;"\
"}\n"\
);


CEmbeddedHLSLShader CEmbeddedHLSLShaders::ms_PS_PPL_HSLs_Specular = CEmbeddedHLSLShader
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


"float4 PS_PPL_HSLs_Specular( float4 ColorDiff : COLOR0,"\
                "float2 Tex0      : TEXCOORD0,"\
                "float3 PosWS     : TEXCOORD1,"\
				"float3 vNormalWS : TEXCOORD2 ) : COLOR0"\
"{"\
	"\nPLANAR_REFLECTION\n"\

	"float4 surface_color = tex2D( Sampler0, Tex0 ) * ColorDiff;\n"\

//	"NORMAL_MAP_TEXTURE\n"\
//	or
//	"TEXTURE_1\n"\

	"float specular_power = GET_SPECULAR_POWER;"\

	// normal in world space
	"const float3 Normal = normalize(vNormalWS);"\

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


CEmbeddedHLSLShader CEmbeddedHLSLShaders::ms_VS_PPL_HSLs_QVertexBlend_Specular = CEmbeddedHLSLShader
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
	printf( "name:    %s\n", CEmbeddedHLSLShaders::ms_PS_PVL_HSLs_Specular.pName );
	printf( "content: %s\n", CEmbeddedHLSLShaders::ms_PS_PVL_HSLs_Specular.pContent );
}
*/
