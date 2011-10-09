#include "EmbeddedGenericHLSL.hpp"
#include "EmbeddedHLSLShader.hpp"
#include "Graphics/Shader/GenericShaderDesc.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"
#include "Support/Log/DefaultLog.hpp"
#include <boost/algorithm/string/replace.hpp>

using namespace std;
using namespace boost;


class CEmbeddedHLSLEffectDesc
{
public:
	CEmbeddedHLSLShader vs;
	CEmbeddedHLSLShader ps;
//	const char *pTechniqueName;

//	const char *pVSName;
//	const char *pPSName;
//	const char *pVS;
//	const char *pPS;

public:

	CEmbeddedHLSLEffectDesc()
//		:
//	pTechniqueName("")
/*	pVSName(""),
	pPSName(""),
	pVS(""),
	pPS("")*/
	{}
/*
	bool IsValid() const
	{
		if( pVSName && 0 < strlen(pVSName)
		 && pPSName && 0 < strlen(pPSName)
		 && pVS && 0 < strlen(pVS)
		 && pPS && 0 < strlen(pPS) )
		{
			return true;
		}
		else
			return false;
	}*/

	bool IsValid() const
	{
		if( vs.IsValid()
		 && ps.IsValid() )
//		 && pTechniqueName && 0 < strlen(pTechniqueName) )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};


const char *CEmbeddedGenericHLSL::ms_pMatrix =
"#ifndef __MATRIX_FXH__\n"\
"#define __MATRIX_FXH__\n"\

// transformation matrices
"float4x4 WorldViewProj : WORLDVIEWPROJ;"\
"float4x4 World : WORLD;"\
"float4x4 View : VIEW;"\
"float4x4 Proj : PROJ;"\

"float4x4 WorldView : WORLDVIEW;\n"\

"#endif\n"; /* __MATRIX_FXH__ */


const char *CEmbeddedGenericHLSL::ms_pColorPair =
"#ifndef __COLORPAIR_H__\n"\
"#define __COLORPAIR_H__\n"\

//-----------------------------------------------------------------------------
// color pair (diffuse & specular)
//-----------------------------------------------------------------------------
"struct COLOR_PAIR"\
"{"\
	"float4 Color     : COLOR0;"\
	"float4 ColorSpec : COLOR1;"\
"};\n"\

"#endif\n"; /* __COLORPAIR_H__ */


const char *CEmbeddedGenericHLSL::ms_pTexDef =
"#ifndef __TEXDEF_FXH__\n"\
"#define __TEXDEF_FXH__\n"\

//--------------------------------------------------------------------------------
// textures
//--------------------------------------------------------------------------------

"texture Texture0;"\
"sampler Sampler0 = sampler_state"\
"{"\
	"Texture   = <Texture0>;"\
	"MipFilter = LINEAR;"\
	"MinFilter = LINEAR;"\
	"MagFilter = LINEAR;"\
"};"\

"texture Texture1;"\
"sampler Sampler1 = sampler_state"\
"{"\
	"Texture   = <Texture1>;"\
	"MipFilter = LINEAR;"\
	"MinFilter = LINEAR;"\
	"MagFilter = LINEAR;"\
"};"\

"texture Texture2;"\
"sampler Sampler2 = sampler_state"\
"{"\
	"Texture   = <Texture2>;"\
	"MipFilter = LINEAR;"\
	"MinFilter = LINEAR;"\
	"MagFilter = LINEAR;"\
"};"\

"texture Texture3;"\
"sampler Sampler3 = sampler_state"\
"{"\
	"Texture   = <Texture3>;"\
	"MipFilter = LINEAR;"\
	"MinFilter = LINEAR;"\
	"MagFilter = LINEAR;"\
"};"\

"texture Texture4;"\
"sampler Sampler4 = sampler_state"\
"{"\
	"Texture   = <Texture4>;"\
	"MipFilter = LINEAR;"\
	"MinFilter = LINEAR;"\
	"MagFilter = LINEAR;"\
"};"\

"texture Texture5;"\
"sampler Sampler5 = sampler_state"\
"{"\
	"Texture   = <Texture5>;"\
	"MipFilter = LINEAR;"\
	"MinFilter = LINEAR;"\
	"MagFilter = LINEAR;"\
"};\n"\
"#endif\n"; /* __TEXDEF_FXH__ */



const char *CEmbeddedGenericHLSL::ms_pLightDef =
"#ifndef __LIGHTDEF_FXH__\n"\
"#define __LIGHTDEF_FXH__\n"\


//-----------------------------------------------------------------------------
// light types
//-----------------------------------------------------------------------------

"#define LIGHT_TYPE_NONE        0\n"\
"#define LIGHT_TYPE_POINT       1\n"\
"#define LIGHT_TYPE_SPOT        2\n"\
"#define LIGHT_TYPE_DIRECTIONAL 3\n"\
"#define LIGHT_NUM_TYPES        4\n"\


//-----------------------------------------------------------------------------
// variables
//-----------------------------------------------------------------------------

//initial and range of directional, point and spot lights within the light array
"int iLightDirIni   = 0;"\
"int iLightDirNum   = 0;"\
"int iLightPointIni = 0;"\
"int iLightPointNum = 0;"\
"int iSpotlightIni  = 0;"\
"int iSpotlightNum  = 0;"\


//-----------------------------------------------------------------------------
// light structure
//-----------------------------------------------------------------------------

"struct CLight"\
"{"\
	"int iType;"\

	// position in world space
	"float3 vPos;"\

	// position in view space
	"float3 vPosVS;"\

	"float3 vDir;"\
	"float4 vAmbient;"\
	"float4 vDiffuseUpper;"\
	"float4 vDiffuseLower;"\
	"float4 vSpecular;"\
	"float  fRange;"\

	//1, D, D^2;
	"float3 vAttenuation;"\

	//cos(theta/2), cos(phi/2), falloff
	"float3 vSpot;"\
"};\n"\

"#endif\n"; /* __LIGHTDEF_FXH__ */



const char *CEmbeddedGenericHLSL::ms_pQuaternion =
"#ifndef __Quaternion_FXH__\n"\
"#define __Quaternion_FXH__\n"\

// Use float4 as quaternion

"float length_sq( float4 src )"\
"{"\
	"return src.x*src.x + src.y*src.y + src.z*src.z + src.w*src.w;"\
"}"\


"float4 quat_inv( float4 src )"\
"{"\
//	Scalar fNorm = (Scalar)0.0;

	"float fNormSq = length_sq(src);"\

	"float4 inv;"\
//	if ( fNormSq > (Scalar)0.0 )
//	{
		"float fInvNormSq = 1.0 / fNormSq;"\
		"inv.x = -src.x*fInvNormSq;"\
		"inv.y = -src.y*fInvNormSq;"\
		"inv.z = -src.z*fInvNormSq;"\
		"inv.w =  src.w*fInvNormSq;"\
/*	}
	else
	{
		// return an invalid result to flag the error
		inv.x = (Scalar)0.0;
		inv.y = (Scalar)0.0;
		inv.z = (Scalar)0.0;
		inv.w = (Scalar)0.0;
	}*/

	// Replace to this after cofirming the above code works
//	return float4( -src.x, -src.y, -src.z, src.w ) / fNormSq;

	"return inv;"\
"}"\


"float4 quat_mul( float4 lhs, float4 rhs )"\
"{"\
    "float4 res;"\

    "res.w ="\
        "lhs.w * rhs.w -"\
        "lhs.x * rhs.x -"\
        "lhs.y * rhs.y -"\
        "lhs.z * rhs.z;"\

    "res.x ="\
        "lhs.w * rhs.x +"\
        "lhs.x * rhs.w +"\
        "lhs.y * rhs.z -"\
        "lhs.z * rhs.y;"\

    "res.y ="\
        "lhs.w * rhs.y +"\
        "lhs.y * rhs.w +"\
        "lhs.z * rhs.x -"\
        "lhs.x * rhs.z;"\

    "res.z ="\
        "lhs.w * rhs.z +"\
        "lhs.z * rhs.w +"\
        "lhs.x * rhs.y -"\
        "lhs.y * rhs.x;"\

    "return res;"\
"}\n"\

"#endif\n"; /* __Quaternion_FXH__ */


const char *CEmbeddedGenericHLSL::ms_pTransform =
"#ifndef __Transform_FXH__\n"\
"#define __Transform_FXH__\n"\

//#include "Quaternion.fxh"

"struct Transform"\
"{"\
	"float4 translation;"\

	/// quaternion
	"float4 rotation;"\
"};"\


"float3 mul( Transform transform, float3 rhs )"\
"{"\
	"float4 res = quat_mul( quat_mul( transform.rotation, float4( rhs.x, rhs.y, rhs.z, 0 ) ), quat_inv( transform.rotation ) );"\
	"return res.xyz + transform.translation.xyz / transform.translation.w;"\
"}"\


"float4 mul( Transform transform, float4 rhs )"\
"{"\
	"return float4( mul( transform, rhs.xyz / rhs.w ), 1 );"\
"}\n"\


"#endif\n"; /* __Transform_FXH__ */


const char *CEmbeddedGenericHLSL::ms_pQVertexBlendFunctions =
"#ifndef __QVertexBlendFunctions_FXH__\n"\
"#define __QVertexBlendFunctions_FXH__\n"\

//===========================================================================================
// SkinPoint
// Applies 4 matrix skinning to a single point.  The point passed in is changed as well as returned.
//
"float4 SkinPoint( inout float4 io_value, Transform blends[NUM_MAX_BLEND_TRANSFORMS],  int4 indices, float4 weights)"\
"{"\
	"int i;"\
	"float4 incoming_io_value = io_value;"\

	"if(weights[0] != -1 )"\
	"{"\
		"io_value = 0;"\
		// skin
//		for(i=0; i <4 ; i++)
		"for(i=0; i <NUM_MAX_BLEND_TRANSFORMS_PER_VERTEX ; i++)"\
		"{"\
			"io_value  += mul( blends[indices[3-i]], incoming_io_value ) * weights[i];"\
//			io_value  += mul( blends[indices[i]], incoming_io_value ) * weights[i];
		"}"\
	"}"\
	"return io_value;"\
"}\n"\

//===========================================================================================
// SkinVector
// Applies 4 matrix skinning to a vector.  The vector passed in is changed as well as returned.
//
"float3 SkinVector( inout float3 io_value, Transform blends[NUM_MAX_BLEND_TRANSFORMS], int4 indices, float4 weights)"\
"{"\
	"int i;"\
	"float3 incoming_io_value = io_value;"\

	"if(weights[0] != -1 )"\
	"{"\
		"io_value = 0;"\
    
		// skin
//		for(i=0; i <4 ; i++)
		"for(i=0; i <NUM_MAX_BLEND_TRANSFORMS_PER_VERTEX ; i++)"\
		"{"\
			"io_value  += mul( blends[indices[3-i]], incoming_io_value ) * weights[i];"\
//			io_value  += mul( blends[indices[i]], incoming_io_value ) * weights[i];
		"}"\

		"io_value = normalize(io_value);"\
	"}"\

	"return io_value;"\
"}\n"\

"#endif\n";  /* __QVertexBlendFunctions_FXH__ */


// Vertex blending with 4x4 matrices (still under development)
// pros
// - Can support scaling
// cons
// - Increase in code size
// - Does not support vertex blending of meshes that have many bones
/*
const char *CEmbeddedGenericHLSL::ms_pVertexBlendFunctions =
"#ifndef __VertexBlendFunctions_FXH__\n"\
"#define __VertexBlendFunctions_FXH__\n"\

//===========================================================================================
// SkinPoint
// Applies 4 matrix skinning to a single point.  The point passed in is changed as well as returned.
//
"float4 SkinPoint( inout float4 io_value, float4x4 blendMats[NUM_MAX_BLEND_MATRICES],  int4 indices, float4 weights)"\
"{"\
	"int i;"\
	"float4 incoming_io_value = io_value;"\

	"if(weights[0] != -1 )"\
	"{"\
		"io_value = 0;"\
		// skin
//		for(i=0; i <4 ; i++)
		"for(i=0; i <NUM_MAX_BLEND_MATRICES_PER_VERTEX ; i++)"\
		"{"\
			"io_value  += mul( incoming_io_value, blendMats[indices[3-i]]) * weights[i];"\
//			io_value  += mul( incoming_io_value, blendMats[indices[i]]) * weights[i];
		"}"\
	"}"\

	"return io_value;"\
"}\n"\

//===========================================================================================
// SkinVector
// Applies 4 matrix skinning to a vector.  The vector passed in is changed as well as returned.
//
"float3 SkinVector( inout float3 io_value, float4x4 blendMats[NUM_MAX_BLEND_MATRICES], int4 indices, float4 weights)"\
"{"\
	"int i;"\
	"float3 incoming_io_value = io_value;"\

	"if(weights[0] != -1 )"\
	"{"\
		"io_value = 0;"\
    
		// skin
//		for(i=0; i <4 ; i++)
		"for(i=0; i <NUM_MAX_BLEND_MATRICES_PER_VERTEX ; i++)"\
		"{"\
			"io_value  += mul( incoming_io_value, blendMats[indices[3-i]]) * weights[i];"\
//			io_value  += mul( incoming_io_value, blendMats[indices[i]]) * weights[i];
		"}"\

		"io_value = normalize(io_value);"\
	"}"\

	"return io_value;"\
"}\n"\

"#endif\n";  // __VertexBlendFunctions_FXH__
*/


const char *CEmbeddedGenericHLSL::ms_pEnvMapSamplerInclude =
"texture  CubeMapTexture;"\
"samplerCUBE CubeMapSampler = "\
"sampler_state"\
"{"\
    "Texture = <CubeMapTexture>;"\
    "MinFilter = LINEAR;"\
    "MagFilter = LINEAR;"\
    "MipFilter = LINEAR;"\
"};\n";


const char *CEmbeddedGenericHLSL::ms_pOptionsMacros =
//"#define SPECTYPE__UNIFORM\n"\

// specular types
"\n"\
"#ifdef SPECTYPE__NONE\n"\
"#define GET_SPECULAR 0.0\n"\
"#endif\n"\

"#ifdef SPECTYPE__UNIFORM\n"\
"#define GET_SPECULAR g_fSpecular\n"\
"#endif\n"\

"#ifdef SPECTYPE__DECAL_TEX_ALPHA\n"\
"#define GET_SPECULAR surface_color.a\n"\
"#endif\n"\

"#ifdef SPECTYPE__NORMAL_MAP_ALPHA\n"\
"#define GET_SPECULAR normal_map.a\n"\
"#endif\n"\

"#ifdef SPECTYPE__TEX1_RED\n"\
"#define GET_SPECULAR normal_map.r\n"\
"#endif\n"\

"#ifdef SPECTYPE__TEX1_GREEN\n"\
"#define GET_SPECULAR normal_map.g\n"\
"#endif\n"\

"#ifdef SPECTYPE__TEX1_BLUE\n"\
"#define GET_SPECULAR normal_map.b\n"\
"#endif\n"\


// glossiness types
"\n"\
"#ifdef SPECPOWTYPE__NONE\n"\
"#define GET_SPECULAR_POWER 8.0\n"\
"#endif\n"\

"#ifdef SPECPOWTYPE__UNIFORM\n"\
"#define GET_SPECULAR_POWER g_fSpecularPower\n"\
"#endif\n"\

"#ifdef SPECPOWTYPE__DECAL_TEX_ALPHA\n"\
"#define GET_SPECULAR_POWER surface_color.a\n"\
"#endif\n"\

"#ifdef SPECPOWTYPE__NORMAL_MAP_ALPHA\n"\
"#define GET_SPECULAR_POWER normal_map.a\n"\
"#endif\n"\

"#ifdef SPECPOWTYPE__TEX1_RED\n"\
"#define GET_SPECULAR_POWER normal_map.r\n"\
"#endif\n"\

"#ifdef SPECPOWTYPE__TEX1_GREEN\n"\
"#define GET_SPECULAR_POWER normal_map.g\n"\
"#endif\n"\

"#ifdef SPECPOWTYPE__TEX1_BLUE\n"\
"#define GET_SPECULAR_POWER normal_map.b\n"\
"#endif\n"\

"#ifdef GET_SPECULAR_POWER\n"\
// one of the macros above is used
"#else\n"\
"#define GET_SPECULAR_POWER 8.0\n"\
"#endif\n"\


// alpha blend types
"\n"\
"#ifdef ALPHABLEND__NONE\n"\
"#define GET_ALPHA 1.0\n"\
"#endif\n"\

//"#ifdef ALPHABLEND__UNIFORM\n"\
//"#define GET_ALPHA g_fAlpha\n"\
//"#endif\n"\

"#ifdef ALPHABLEND__DIFFUSE_ALPHA\n"\
"#define GET_ALPHA ColorDiff.a\n"\
"#endif\n"\

"#ifdef ALPHABLEND__DECAL_TEX_ALPHA\n"\
"#define GET_ALPHA surface_color.a\n"\
"#endif\n"\

"#ifdef ALPHABLEND__MOD_DIFFUSE_ALPHA_AND_DECAL_TEX_ALPHA\n"\
"#define GET_ALPHA ColorDiff.a * surface_color.a\n"\
"#endif\n"\

"#ifdef ALPHABLEND__MOD_DIFFUSE_ALPHA_AND_DECAL_TEX_ALPHA\n"\
"#define GET_ALPHA ColorDiff.a * surface_color.a\n"\
"#endif\n"\


// environmental mapping options
"#ifdef ENVMAP__NONE\n"\
"#define ENVMAP\n"\
"#endif\n"\

"#ifdef ENVMAP__ENABLED\n"\
//"#define ENVMAP color.rgb = texCUBE( CubeMapSampler, mul( Normal, (float3x3)View ) ).rgb * 0.5 + color.rgb * 0.5\n"
"#define ENVMAP color.rgb = texCUBE( CubeMapSampler, mul( Normal, (float3x3)View ) ).rgb\n"\
"#endif\n"\

"#ifdef ENVMAP\n"\
"#else\n"\
"#define ENVMAP\n"\
"#endif\n"\


// planer reflection options
// Replace PlanarMirrorTextureSampler with Sampler0 and set g_fPlanarReflection to 1
// in order to see if the scene is rendered to the mirror texture is created or not.
"#ifdef PLANAR_REFLECTION__NONE\n"\
"#define PLANAR_REFLECTION\n"\
"#define PLANAR_REFLECTION_COLOR_UPDATE\n"\
"#endif\n"\

"#ifdef PLANAR_REFLECTION__FLAT\n"\
"texture PlanarMirrorTexture;"\
"sampler PlanarMirrorTextureSampler = sampler_state"\
"{"\
	"Texture   = <PlanarMirrorTexture>;"\
	"MipFilter = LINEAR;"\
	"MinFilter = LINEAR;"\
	"MagFilter = LINEAR;"\
"};"\

"float g_fPlanarReflection = 1.0f;\n"
"#define PLANAR_REFLECTION float2 pr_tex = float2( PosPS.x / PosPS.w, -PosPS.y / PosPS.w ) * 0.5 + 0.5;"\
"float4 pr_tex_color = float4( tex2D(PlanarMirrorTextureSampler, pr_tex).xyz, 1 );\n"\
"#define PLANAR_REFLECTION_COLOR_UPDATE color = color * (1-g_fPlanarReflection) + pr_tex_color * g_fPlanarReflection;\n"\

//"#define PLANAR_REFLECTION\n"\
//"#define PLANAR_REFLECTION_COLOR_UPDATE\n"\

"#endif\n"\

"#ifdef PLANAR_REFLECTION__PERTURBED\n"\
"float g_fPlanarReflection = 0.2f;\n"
"float2 g_vPerturbationTextureUVShift = float2(0,0);\n"
"#define PLANAR_REFLECTION float2 pr_tex = float2( PosPS.x, -PosPS.y ) * 0.5 + 0.5;"\
"float2 perturbation = ( tex2D(Sampler2, Tex0 + g_vPerturbationTextureUVShift).xy - float2(0.5,0.5) ) * 0.1;"\
"pr_tex += perturbation;"\
"Tex0 += perturbation;"\
"float4 pr_tex_color = float4( tex2D(Sampler1, pr_tex).xyz, 1 );\n"\
"#define PLANAR_REFLECTION_COLOR_UPDATE color = color * (1-g_fPlanarReflection) + pr_tex_color * g_fPlanarReflection;\n"\
"#endif\n"\

"#ifdef PLANAR_REFLECTION\n"\
"#else\n"\
"#define PLANAR_REFLECTION\n"\
"#endif\n"\
"\n";


const char *CEmbeddedGenericHLSL::ms_pPerPixelHSLighting_Specular =
"#ifndef __PerPixelHSLighting_Specular_FXH__\n"\
"#define __PerPixelHSLighting_Specular_FXH__\n"\

/// param vLightDirWS [in] direction of the light ray. i.e. direciton from the light to the vertex
"float CalculateSpecularComponent( float3 vLightDirWS,"\
								  "float3 vNormalWS,"\
								  "float3 vToViewerWS,"\
								  "float specular_power )"\
"{"\
	"float3 vReflectionWS = vLightDirWS - 2.0 * dot( vNormalWS, vLightDirWS ) * vNormalWS;"\
//	vReflectionWS = normalize(vReflectionWS);
	"float alignment = dot(vReflectionWS,vToViewerWS);"\
	"if( 0 < alignment )"\
		"return pow( alignment, specular_power );"\
	"else\n"\
		"return 0.0f;"\
"}\n"\


//-----------------------------------------------------------------------------
// Name: DoHemisphericDirLight()
// Desc: hemispheric directional light computation
//-----------------------------------------------------------------------------
"COLOR_PAIR DoHemisphericDirLight_Specular(float3 vNormalWS, float3 vToViewerWS, int i, float specular_power )"\
"{"\
	"COLOR_PAIR Out;"\
	"float NdotL = dot( vNormalWS, -g_aLight[i].vDir );"\
	"float fUpper = ( NdotL + 1.0 ) / 2.0;"\
	"Out.Color = g_aLight[i].vAmbient;"\
	"Out.ColorSpec = 0;"\

	// compute diffuse color
	"Out.Color += fUpper * g_aLight[i].vDiffuseUpper;"\
	"Out.Color += (1 - fUpper) * g_aLight[i].vDiffuseLower;"\

	// add specular component
	"if( 0.0 <= NdotL )"\
	"{"\
		"Out.ColorSpec = CalculateSpecularComponent( g_aLight[i].vDir, vNormalWS, vToViewerWS, specular_power );// * g_aLight[i].vSpecular\n"\
	"}"\

	"return Out;"\
"}\n"\


//-----------------------------------------------------------------------------
// Name: DoHemisphericPointLight()
// Desc: hemispheric point light computation
//-----------------------------------------------------------------------------
"COLOR_PAIR DoHemisphericPointLight_Specular(float3 vWorldPos, float3 vNormalWS, float3 vToViewerWS, int i, float specular_power )"\
"{"\
	"float3 vVertToLightWS = g_aLight[i].vPos - vWorldPos;"\
	"float LD = length( vVertToLightWS );"\
	"float3 vDirToLightWS = vVertToLightWS / LD;"\
	"COLOR_PAIR Out;"\
	"float NdotL = dot(vNormalWS, vDirToLightWS);"\
	"float fUpper = ( NdotL + 1.0 ) / 2.0;"\
	"Out.Color = g_aLight[i].vAmbient;"\
	"Out.ColorSpec = 0;"\
	"float fAtten = 1.f;"\

	// compute diffuse color
	"Out.Color += fUpper * g_aLight[i].vDiffuseUpper;"\
	"Out.Color += (1 - fUpper) * g_aLight[i].vDiffuseLower;"\

	// add specular component
	"if( 0.0 <= NdotL )"\
	"{"\
		"Out.ColorSpec = CalculateSpecularComponent( -vDirToLightWS, vNormalWS, vToViewerWS, specular_power );// * g_aLight[i].vSpecular\n"\
	"}"\

	"if(LD > g_aLight[i].fRange)"\
	"{"\
		"fAtten = 0.0;"\
	"}"\
	"else"\
	"{"\
		"fAtten *= 1.0 / (g_aLight[i].vAttenuation.x + g_aLight[i].vAttenuation.y*LD + g_aLight[i].vAttenuation.z*LD*LD);"\
	"}"\

	"Out.Color *= fAtten;"\
	"Out.ColorSpec *= fAtten;"\

	"return Out;"\
"}\n"\

"#endif\n";  /* __PerPixelHSLighting_Specular_FXH__ */


const char *CEmbeddedGenericHLSL::ms_pTechniqueTemplate =
"technique $TECH"\
"{"\
	"pass P0"\
	"{"\
		"VertexShader = compile vs_$VS_VER $(VS)();"\
		"PixelShader  = compile ps_$PS_VER $(PS)();"\

/*		"CullMode = None;"\*/
		"ZEnable = True;"\
		"AlphaBlendEnable = True;"\
		"SrcBlend = SrcAlpha;"\
		"DestBlend = InvSrcAlpha;"\
	"}"\
"}";



// ??? VS_HSPerVertexLights_QVertexBlend


static const char *GetAlphaBlendTypeMacro( CAlphaBlendType::Name alpha_blend )
{
	switch( alpha_blend )
	{
	case CAlphaBlendType::NONE:                                  return "#define ALPHABLEND__NONE\n";
//	case CAlphaBlendType::UNIFORM:                               return "#define ALPHABLEND__UNIFORM\n";
	case CAlphaBlendType::DIFFUSE_ALPHA:                         return "#define ALPHABLEND__DIFFUSE_ALPHA\n";
	case CAlphaBlendType::DECAL_TEX_ALPHA:                       return "#define ALPHABLEND__DECAL_TEX_ALPHA\n";
//	case CAlphaBlendType::NORMAL_MAP_ALPHA:                      return "#define ALPHABLEND__NORMAL_MAP_ALPHA\n";
	case CAlphaBlendType::MOD_DIFFUSE_ALPHA_AND_DECAL_TEX_ALPHA: return "#define ALPHABLEND__MOD_DIFFUSE_ALPHA_AND_DECAL_TEX_ALPHA\n";
	default: return "#define ALPHABLEND__NONE\n";
	}
}


void LoadShader_HSPerVeretxLighting( CGenericShaderDesc& desc, CEmbeddedHLSLEffectDesc& dest )
{
	// per-vetex without specular reflection
	// Just select the shader for multi lights
//	dest.pVS     = "";//ms_pVS_PerVertexHSLights;
//	dest.pPS     = "";//ms_pPS_PerVertexHSLights;
//	dest.pVSName = "";
//	dest.pPSName = "";
}


void LoadShader_HSPerVeretxLighting_Specular( CGenericShaderDesc& desc, CEmbeddedHLSLEffectDesc& dest )
{
/*	switch( desc.num_directional_lights )
	{
	case -1:
		switch( desc.num_point_lights )
		{
			dest.vs = CEmbeddedHLSLShaders::ms_VS_PVL_HSLs_Spacular;
			dest.ps = CEmbeddedHLSLShaders::ms_PS_PVL_HSLs_Spacular;
			dest.pTechniqueName = "PVL_HSLs_Spacular";
//			CreateShader( vs, ps,  );

		case -1:

			break;
		}
		// select a technique that supports multiple number of lights
		break;
	case 1:
		break;
	default:
		break;
	}*/
}


void LoadShader_HSPerVeretxLighting_QVertexBlend( CGenericShaderDesc& desc,
												  CEmbeddedHLSLEffectDesc& dest )
{
	dest.vs = CEmbeddedHLSLShaders::ms_VS_PVL_HSLs_QVertexBlend;
	dest.ps = CEmbeddedHLSLShaders::ms_PS_PVL_HSLs_QVertexBlend;
//	dest.pTechniqueName = "";

	switch( desc.NumDirectionalLights )
	{
	case -1:
	default:
		break;
	}
}


void LoadShader_HSPerVeretxLighting_QVertexBlend_Specular( CGenericShaderDesc& desc, CEmbeddedHLSLEffectDesc& dest )
{
}

static const char *GetSpecularTypeMacro( CSpecularSource::Name spec_src )
{
	switch( spec_src )
	{
	case CSpecularSource::NONE:             return "#define SPECTYPE__NONE\n";
	case CSpecularSource::UNIFORM:          return "#define SPECTYPE__UNIFORM\n";
	case CSpecularSource::DECAL_TEX_ALPHA:  return "#define SPECTYPE__DECAL_TEX_ALPHA\n";
	case CSpecularSource::NORMAL_MAP_ALPHA: return "#define SPECTYPE__NORMAL_MAP_ALPHA\n";
	default: return "#define SPECTYPE__NONE\n";
	}
}

static const char *GetBumpMapOptionMacro( int index )
{
	if( index < 0 )
		return "";
	else
		return "#define BUMPMAP\n";
}

static const char *GetBumpMapSamplerMacro( int index )
{
	switch( index )
	{
	case 0: return "#define BUMPMAP_SAMPLER Sampler0\n";
	case 1: return "#define BUMPMAP_SAMPLER Sampler1\n";
	case 2: return "#define BUMPMAP_SAMPLER Sampler2\n";
	case 3: return "#define BUMPMAP_SAMPLER Sampler3\n";
	default:
		return "";
	}

}

static const char *GetEnvMapOptionMacro( CEnvMapOption::Name envmap_option )
{
	switch( envmap_option )
	{
	case CEnvMapOption::NONE:             return "#define ENVMAP__NONE\n";
	case CEnvMapOption::ENABLED:          return "#define ENVMAP__ENABLED\n";
	default: return "#define ENVMAP__NONE\n";
	}
}

static const char *GetPlanarReflectionOptionMacro( CPlanarReflectionOption::Name pr_option )
{
	switch( pr_option )
	{
	case CPlanarReflectionOption::NONE:         return "#define PLANAR_REFLECTION__NONE\n";
	case CPlanarReflectionOption::FLAT:         return "#define PLANAR_REFLECTION__FLAT\n";
	case CPlanarReflectionOption::PERTURBED:    return "#define PLANAR_REFLECTION__PERTURBED\n";
	default: return "#define PLANAR_REFLECTION__NONE\n";
	}
}

void LoadShader_HSPerPixelLighting_Specular( CGenericShaderDesc& desc, CEmbeddedHLSLEffectDesc& dest )
{
	dest.vs = CEmbeddedHLSLShaders::ms_VS_PPL_HSLs_Specular;
	dest.ps = CEmbeddedHLSLShaders::ms_PS_PPL_HSLs_Specular;

	dest.ps.pDependencies.push_back( GetAlphaBlendTypeMacro(desc.AlphaBlend) );
	dest.ps.pDependencies.push_back( GetSpecularTypeMacro(desc.Specular) );
	dest.ps.pDependencies.push_back( GetBumpMapOptionMacro(desc.NormalMapTextureIndex) );
	dest.ps.pDependencies.push_back( GetBumpMapSamplerMacro(desc.NormalMapTextureIndex) );
	dest.ps.pDependencies.push_back( GetEnvMapOptionMacro(desc.EnvMap) );
	dest.ps.pDependencies.push_back( GetPlanarReflectionOptionMacro(desc.PlanarReflection) );

//	dest.pTechniqueName = "PPL_HSLs_Specular";
}


void LoadShader_HSPerPixelLighting( CGenericShaderDesc& desc, CEmbeddedHLSLEffectDesc& dest )
{
	dest.vs = CEmbeddedHLSLShaders::ms_VS_PPL_HSLs;
	dest.ps = CEmbeddedHLSLShaders::ms_PS_PPL_HSLs;
//	dest.pTechniqueName = "PPL_HSLs";

	dest.ps.pDependencies.push_back( GetBumpMapOptionMacro(desc.NormalMapTextureIndex) );
	dest.ps.pDependencies.push_back( GetBumpMapSamplerMacro(desc.NormalMapTextureIndex) );
	dest.ps.pDependencies.push_back( GetEnvMapOptionMacro(desc.EnvMap) );
	dest.ps.pDependencies.push_back( GetPlanarReflectionOptionMacro(desc.PlanarReflection) );
}


void LoadShader_HSPerPixelLighting_QVertexBlend( CGenericShaderDesc& desc, CEmbeddedHLSLEffectDesc& dest )
{
	LOG_PRINT_ERROR( " Not implemented." );
}


void LoadShader_HSPerPixelLighting_QVertexBlend_Specular( CGenericShaderDesc& desc, CEmbeddedHLSLEffectDesc& dest )
{
	dest.vs = CEmbeddedHLSLShaders::ms_VS_PPL_HSLs_QVertexBlend_Specular;
	dest.ps = CEmbeddedHLSLShaders::ms_PS_PPL_HSLs_Specular;

	dest.ps.pDependencies.push_back( GetAlphaBlendTypeMacro(desc.AlphaBlend) );
	dest.ps.pDependencies.push_back( GetSpecularTypeMacro(desc.Specular) );
	dest.ps.pDependencies.push_back( GetEnvMapOptionMacro(desc.EnvMap) );
	dest.ps.pDependencies.push_back( GetPlanarReflectionOptionMacro(desc.PlanarReflection) );
}

/*
void LoadPerVeretxLightingShader( CGenericShaderDesc& desc, CEmbeddedHLSLEffectDesc& dest )
{
	switch( desc. )
	{
	default:
		break;
	}
}
*/

void LoadHSLightingShader( CGenericShaderDesc& desc, CEmbeddedHLSLEffectDesc& dest )
{
	switch( desc.ShaderLightingType )
	{
	case CShaderLightingType::PER_VERTEX:
		if( desc.VertexBlendType == CVertexBlendType::NONE )
		{
			if( desc.Specular == CSpecularSource::NONE )
				LoadShader_HSPerVeretxLighting(desc,dest);
			else
				LoadShader_HSPerVeretxLighting_Specular(desc,dest);
		}
		else if( desc.VertexBlendType == CVertexBlendType::QUATERNION_AND_VECTOR3
		      || desc.VertexBlendType == CVertexBlendType::MATRIX )
		{
			if( desc.Specular == CSpecularSource::NONE )
				LoadShader_HSPerVeretxLighting_QVertexBlend(desc,dest);
			else
				LoadShader_HSPerVeretxLighting_QVertexBlend_Specular(desc,dest);
		}
		break;

	case CShaderLightingType::PER_PIXEL:
		if( desc.VertexBlendType == CVertexBlendType::NONE )
		{
			if( desc.Specular == CSpecularSource::NONE )
				LoadShader_HSPerPixelLighting(desc,dest);
			else
				LoadShader_HSPerPixelLighting_Specular(desc,dest);
		}
		else if( desc.VertexBlendType == CVertexBlendType::QUATERNION_AND_VECTOR3
		      || desc.VertexBlendType == CVertexBlendType::MATRIX )
		{
			if( desc.Specular == CSpecularSource::NONE )
				LoadShader_HSPerPixelLighting_QVertexBlend(desc,dest);
			else
				LoadShader_HSPerPixelLighting_QVertexBlend_Specular(desc,dest);
		}
		break;

	default:
		break;
	}
}


Result::Name CEmbeddedGenericHLSL::GenerateShader( CGenericShaderDesc& desc, std::string& hlsl_effect )
{
	LOG_FUNCTION_SCOPE();

	CEmbeddedHLSLEffectDesc hlsl_desc;

//	LoadShader( desc, hlsl_desc );
	LoadHSLightingShader( desc, hlsl_desc );

	if( !hlsl_desc.IsValid() )
		return Result::UNKNOWN_ERROR;

	hlsl_effect = "";
	for( size_t i=0; i<hlsl_desc.vs.pDependencies.size(); i++ ) // functions, variables, and macros used in PS
		hlsl_effect += hlsl_desc.vs.pDependencies[i];
	hlsl_effect += hlsl_desc.vs.pContent; // vertex shader
	for( size_t i=0; i<hlsl_desc.ps.pDependencies.size(); i++ ) // functions, variables, and macros used in PS
		hlsl_effect += hlsl_desc.ps.pDependencies[i];
	hlsl_effect += hlsl_desc.ps.pContent; // pixel shader

	if( desc.EnvMap != CEnvMapOption::NONE )
		hlsl_effect = ms_pEnvMapSamplerInclude + hlsl_effect;

	replace_all( hlsl_effect, "#include \"Matrix.fxh\"",                      ms_pMatrix );
	replace_all( hlsl_effect, "#include \"ColorPair.fxh\"",                   ms_pColorPair );
	replace_all( hlsl_effect, "#include \"TexDef.fxh\"",                      ms_pTexDef );
	replace_all( hlsl_effect, "#include \"LightDef.fxh\"",                    ms_pLightDef );
	replace_all( hlsl_effect, "#include \"Quaternion.fxh\"",                  ms_pQuaternion );
	replace_all( hlsl_effect, "#include \"Transform.fxh\"",                   ms_pTransform );
	replace_all( hlsl_effect, "#include \"QVertexBlendFunctions.fxh\"",       ms_pQVertexBlendFunctions );
	replace_all( hlsl_effect, "#include \"PerPixelHSLighting_Specular.fxh\"", ms_pPerPixelHSLighting_Specular );
	replace_all( hlsl_effect, "[[OPTIONS_MACROS]]", ms_pOptionsMacros );

	string tech( ms_pTechniqueTemplate );

	// All the effect has the same technique name, "Default"
	// rationale: the implementation of the effect is hidden from the client,
	// so technique names that represents the content of the shader, such as "PerPixel_HemisphericLighting",
	// cannot be not used.
	replace_first( tech, "$TECH", "Default" );
	replace_first( tech, "$(VS)", hlsl_desc.vs.pName );
	replace_first( tech, "$(PS)", hlsl_desc.ps.pName );

//	replace_first( tech, "$VS_VER",   "2_0" );
//	replace_first( tech, "$PS_VER",   "2_0" );

	// >>> Experiment - Compile with the highest shader verison
	D3DCAPS9 caps;
	HRESULT hr = DIRECT3D9.GetD3D()->GetDeviceCaps( 0, D3DDEVTYPE_HAL, &caps );
	const UINT ps_major = D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion);
	const UINT ps_minor = D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion);
	const UINT vs_major = D3DSHADER_VERSION_MAJOR(caps.VertexShaderVersion);
	const UINT vs_minor = D3DSHADER_VERSION_MINOR(caps.VertexShaderVersion);

	char ps_ver[16], vs_ver[16];
	memset( ps_ver, 0, sizeof(ps_ver) );
	memset( vs_ver, 0, sizeof(vs_ver) );
	sprintf( ps_ver, "%u_%u", ps_major, ps_minor );
	sprintf( vs_ver, "%u_%u", vs_major, vs_minor );

	replace_first( tech, "$VS_VER", ps_ver );
	replace_first( tech, "$PS_VER", vs_ver );
	// <<< Experiment - Compile with the highest shader verison

	hlsl_effect += tech;

	static int s_count = 0;
	FILE *fp = fopen( fmt_string("./debug/embedded_effect_%03d.fx",s_count++).c_str(),"w");
	if( fp )
	{
		fprintf(fp,hlsl_effect.c_str());
		fclose(fp);
	}

	return Result::SUCCESS;
}

/*
CShaderHandle GetGenericShader( CGenericShaderDesc& desc )
{
	CShaderHandle handle
	bool res = Get( desc, handle );
	if( res )
		return handle;

	handle = Load( desc );

	return handle;
}
*/

#include "Graphics/Direct3D/Direct3D9.hpp"

void ConfigureShader()
{
	D3DCAPS9 caps;
	DIRECT3D9.GetDevice()->GetDeviceCaps( &caps );

	DWORD ps_ver = caps.PixelShaderVersion;

/*	if( 3.0 <= ps_ver )
	{
	}
	else
	{
	}*/
}