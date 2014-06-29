// PerPixelSingleHSDirectionalLight_PR_WS.fx
// planer reflection for water surface
// - Shift the texture coordinates for perturbation texture to make the planer reflection surface look like waves

float4 g_vAmbientColor = float4(0.1f, 0.1f, 0.1f, 1.f);

bool g_bSpecular : register(b0) = false;

float4 vSpecColor = {1.0f, 1.0f, 1.0f, 1.0f};

float g_fMaterialPower = 1.0f;

float3 g_vEyePos;	// the position of the eye(camera) in world space

float3 g_vLightDir = { 0.56568f, -0.70711f, 0.42426f };
float3 g_vUpperDiffuseColor = { 1,1,1 };
float3 g_vLowerDiffuseColor = { 0.3, 0.3, 0.3 };

// transformation matrices
float4x4 WorldViewProj	: WORLDVIEWPROJ;
float4x4 World			: WORLD;
float4x4 View			: VIEW;
float4x4 Proj			: PROJ;

float4x4 WorldView		: WORLDVIEW;

float2 g_vPerturbationTextureUVShift = float2(0,0);

float g_fPlanarReflection = 0.2f;


//--------------------------------------------------------------------------------
// textures
//--------------------------------------------------------------------------------

#include "TexDef.fxh"


/**
 @brief A vertex shader
 */
void VS_PerPixelSingleHSDirectionalLIght_PR(
	float3 Pos     : POSITION, 
	float3 Normal  : NORMAL,
	float4 Diffuse : COLOR0,
	float2 Tex0    : TEXCOORD0,
	out float4 oPos      : POSITION,
	out float4 oDiffuse  : COLOR0,
	out float2 oTex      : TEXCOORD0,
	out float3 oNormalWS : TEXCOORD1,
	out float4 oPosPS    : TEXCOORD2 )
{
	oPos = mul( float4(Pos,1), WorldViewProj );

	oTex = Tex0;

//	float4 ViewPos = mul( float4(Pos,1), WorldView );
//	oDist = ViewPos.z / ViewPos.w;

	// adjust transparency
	oDiffuse = float4( 1,1,1,1 );//Diffuse.r, Diffuse.g, Diffuse.b, 0.5f + fCamDir );

	oNormalWS = mul( Normal, (float3x3)World );

	oPosPS = oPos;
}



/**
 - Sampler0: surface color texture
 - Sampler1: planer reflection texture
 - Sampler2: normal map texture
 */
float4 PS_PerPixelSingleHSDirectionalLIght_PR(
       float4 Diffuse  : COLOR0,
       float2 Tex0     : TEXCOORD0,
       float3 NormalWS : TEXCOORD1,
	   float4 PosPS    : TEXCOORD2 ) : COLOR
{
	// just output texture color
//	float fog_blend = ( Dist - 100 ) / 300;

//	fog_blend = clamp( fog_blend, 0, 1 );

	// d: [0,1]
	// d = d = 0: least bright / 1: brightest
	float d = ( dot( NormalWS, -g_vLightDir ) + 1.0f ) * 0.5;
	float3 light_diffuse
		= g_vUpperDiffuseColor * d
		+ g_vLowerDiffuseColor * (1-d);

	float2 perturbation_tex_coord = Tex0 + g_vPerturbationTextureUVShift;
	float2 perturbation = ( tex2D(Sampler2, perturbation_tex_coord).xy - float2(0.5,0.5) ) * 0.1;
	float2 tex = Tex0 + perturbation;

	float4 tex_color = tex2D(Sampler0, tex);
	float4 surface_color;
	surface_color.rgb = tex_color.rgb * light_diffuse;
	surface_color.a   = tex_color.a;

	float2 pr_tex;
	pr_tex.x =  (PosPS.x / PosPS.w) * 0.5 + 0.5;
	pr_tex.y = -(PosPS.y / PosPS.w) * 0.5 + 0.5;
	pr_tex.xy += perturbation;
	float4 pr_tex_color = tex2D(Sampler1, pr_tex);
	pr_tex_color.a = 1.0;

	float pr_factor = g_fPlanarReflection;

	return surface_color * (1-pr_factor) + pr_tex_color * pr_factor;

//	return float4( pr_tex.x, pr_tex.y, 0, 1 );
//	return float4(1,0,0,1);
//	return pr_tex_color;
//	return surface_color;
//	return surface_color * (1.0 - fog_blend) + float4( 0.7, 0.7, 0.7, 1.0 ) * fog_blend;
}


technique Default
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS_PerPixelSingleHSDirectionalLIght_PR();
		PixelShader  = compile ps_2_0 PS_PerPixelSingleHSDirectionalLIght_PR();
//		CullMode = Ccw;
	}
}
