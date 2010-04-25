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


CEmbeddedHLSLShader CEmbeddedHLSLShaders::ms_PS_PPL_HSLs = CEmbeddedHLSLShader
(
"PS_PPL_HSLs",
"#include \"LightDef.fxh\"\n"\
"#include \"TexDef.fxh\"\n"\

"#define NUM_LIGHTS	5\n"\
"CLight g_aLight[NUM_LIGHTS];"\

"float4 g_vAmbientColor = float4(0,0,0,0);"\

"float4 PS_PPL_HSLs( float4 ColorDiff : COLOR0,"\
                "float2 Tex0     : TEXCOORD0,"\
                "float3 PosWS    : TEXCOORD1,"\
				"float3 NormalWS : TEXCOORD2 ) : COLOR0"\
"{"\
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
	"color.rgb = surface_color * diffuse + g_vAmbientColor;"\
	"color.a = surface_color.a;"\
	"return color;"\
"}\n"\
);


CEmbeddedHLSLShader CEmbeddedHLSLShaders::ms_VS_PPL_HSLs_Specular = CEmbeddedHLSLShader
(
"VS_PPL_HSLs_Specular",
"VS_PPL_HSLs_Specular()"\
"{"\
"}"\
);


CEmbeddedHLSLShader CEmbeddedHLSLShaders::ms_PS_PPL_HSLs_Specular = CEmbeddedHLSLShader
(
"PS_PPL_HSLs_Specular",
"PS_PPL_HSLs_Specular()"\
"{"\
"}"\
);


/*
// Used for compile test
int main()// { return 0; }
{
	printf( "name:    %s\n", CEmbeddedHLSLShaders::ms_PS_PVL_HSLs_Specular.pName );
	printf( "content: %s\n", CEmbeddedHLSLShaders::ms_PS_PVL_HSLs_Specular.pContent );
}
*/
