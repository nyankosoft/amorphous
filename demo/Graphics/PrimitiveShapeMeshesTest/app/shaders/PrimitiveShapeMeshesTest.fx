
float4 g_vAmbientColor = float4(0.1f, 0.1f, 0.1f, 1.f);

bool g_bSpecular : register(b0) = false;

float4 vSpecColor = {1.0f, 1.0f, 1.0f, 1.0f};

float g_fMaterialPower = 1.0f;

float3 g_vEyePos;	// the position of the eye(camera) in world space

float3 g_vLightDir = { 0.56568f, 0.70711f, 0.42426f };


//--------------------------------------------------------------------------------
// vertex & pixel shaders for vertex blending
//--------------------------------------------------------------------------------


#define NUM_MAX_BLEND_MATRICES_PER_VERTEX	2


#include "../../../../../Shaders/Matrix.fxh"
#include "../../../../../Shaders/TexDef.fxh"

#include "../../../../../Shaders/NozzleFlame.fxh"

// lights
#include "../../../../../Shaders/LightDef.fxh"
#define NUM_LIGHTS	5
CLight g_aLight[NUM_LIGHTS];

//#include "../../../../../Shaders/QuickTest.fxh"
//#include "../../../../../Shaders/SkyBox.fxh"

#include "../../../../../Shaders/EnvMap.fxh"
#include "../../../../../Shaders/VS_ForPerPixelLightsWithSpecular.fxh"
#include "../../../../../Shaders/PS_Specular.fxh"
#include "../../../../../Shaders/PerPixelLightingTechniques.fxh"


technique NullShader
{
	pass P0
	{
		VertexShader = NULL;
		PixelShader  = NULL;
	}
}


//--------------------------------------------------------------------------------
// Name: VS_NoLighting()
// Desc: vertex shader
//--------------------------------------------------------------------------------

void VS_NoLighting(
	float3 Pos  : POSITION, 
	float3 Normal : NORMAL,
	float4 Diffuse : COLOR0,
	float2 Tex0 : TEXCOORD0,
	out float4 oPos : POSITION,
	out float4 oDiffuse : COLOR0,
	out float2 oTex : TEXCOORD0,
	out float oDist : TEXCOORD1 )
{
	oPos = mul( float4(Pos,1), WorldViewProj );

	oTex = Tex0;

	float3 ViewPos = mul( float4(Pos,1), WorldView );
	oDist = ViewPos.z;

	// adjust transparency
	oDiffuse = float4( 1,1,1,1 );//Diffuse.r, Diffuse.g, Diffuse.b, 0.5f + fCamDir );
}


//--------------------------------------------------------------------------------
// Name: PS_NoLighting()
// Desc: pixel shader
//--------------------------------------------------------------------------------

float4 PS_NoLighting(
       float4 Diffuse : COLOR0,
       float2 Tex0 : TEXCOORD0,
	   float Dist : TEXCOORD1 ) : COLOR
{
	// just output texture color
	float fog_blend = ( Dist - 100 )/ 300;

	fog_blend = clamp( fog_blend, 0, 1 );

//	return tex2D(Sampler0, Tex0);
	return tex2D(Sampler0, Tex0) * (1.0 - fog_blend) + float4( 0.7, 0.7, 0.7, 1.0 ) * fog_blend;
}


technique NoLighting
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS_NoLighting();
		PixelShader  = compile ps_2_0 PS_NoLighting();
//		CullMode = Ccw;
	}
}
