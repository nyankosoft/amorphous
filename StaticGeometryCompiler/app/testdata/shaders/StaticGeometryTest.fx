//
// Default.fx
//

#define PI  3.14f

//float4 g_vAmbientColor = float4(128.f/255.f, 128.f/255.f, 128.f/255.f, 1.f);
float3 g_vAmbientColor = float3(0.1f, 0.1f, 0.1f);

bool g_bNightVision : register(b2) = false;

float3 g_vEyePos;	// the position of the eye(camera) in world space


// transformation matrices
#include "Matrix.fxh"

// textures
#include "TexDef.fxh"


//--------------------------------------------------------------------------------
// vertex structure for skybox
//--------------------------------------------------------------------------------
struct VS_OUTPUT_SKYBOX
{
    float4 Pos  : POSITION;
    float2 Tex0 : TEXCOORD0;
};


//--------------------------------------------------------------------------------
// light
//--------------------------------------------------------------------------------

//#define NUM_LIGHTS	5

//#include "LightDef.fxh"

//CLight g_aLight[NUM_LIGHTS];

//#include "VertexLightFunctions.fxh"
//#include "PerVertexLightingsHS.fxh"

//#include "EnvMap.fxh"

	
//#include "StaticGeometry.fxh"

/*
// skybox shader
#include "SkyBox.fxh"
#include "SkySphereFG.fxh"


// billboard shader
#include "Billboard.fxh"
#include "StaticBillboard.fxh"
#include "Particle.fxh"


// membrane effect
#include "Membrane.fxh"
*/

// for mesh objects rendered with their original vertex/texture color
//#include "NoLighting.fxh"

technique NoShader
{
	pass P0
	{
		// default - no shader geometry
		VertexShader = NULL;
		PixelShader  = NULL;

		CullMode = Ccw;
		Sampler[0] = (Sampler0);
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



//--------------------------------------------------------------------------------
// Name: VS_StaticLightmap()
// Desc: vertex shader
//--------------------------------------------------------------------------------

void VS_StaticLightmap(
	float3 Pos  : POSITION, 
	float3 Normal : NORMAL,
	float4 Diffuse : COLOR0,
	float2 Tex0 : TEXCOORD0,
	float2 Tex1 : TEXCOORD1,
	out float4 oPos : POSITION,
	out float4 oDiffuse : COLOR0,
	out float2 oTex0 : TEXCOORD0,
	out float2 oTex1 : TEXCOORD1,
	out float oDist : TEXCOORD2 )
{
	oPos = mul( float4(Pos,1), WorldViewProj );

	oTex0 = Tex0;
	oTex1 = Tex1;

	float3 ViewPos = mul( float4(Pos,1), WorldView );
	oDist = ViewPos.z;

	// adjust transparency
	oDiffuse = float4( 1,1,1,1 );//Diffuse.r, Diffuse.g, Diffuse.b, 0.5f + fCamDir );
}


//--------------------------------------------------------------------------------
// Name: PS_NoLighting()
// Desc: pixel shader
//--------------------------------------------------------------------------------

float4 PS_StaticLightmap(
       float4 Diffuse : COLOR0,
       float2 Tex0 : TEXCOORD0,
       float2 Tex1 : TEXCOORD1,
	   float Dist : TEXCOORD2 ) : COLOR
{
	// just output texture color
	float fog_blend = ( Dist - 100 )/ 300;

	fog_blend = clamp( fog_blend, 0, 1 );

	float4 lightmap_color = float4(1,1,1,1); //tex2D(Sampler1, Tex1);

//	return tex2D(Sampler0, Tex0);
	return tex2D(Sampler0, Tex0) * lightmap_color * (1.0 - fog_blend) + float4( 0.7, 0.7, 0.7, 1.0 ) * fog_blend;
}



technique Default
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS_NoLighting();
		PixelShader  = compile ps_2_0 PS_NoLighting();

		CullMode = Ccw;
	}
}


technique StaticLightmap
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS_StaticLightmap();
		PixelShader  = compile ps_2_0 PS_StaticLightmap();

		CullMode = Ccw;
	}
}


technique Terrain
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS_NoLighting();
		PixelShader  = compile ps_2_0 PS_NoLighting();

		CullMode = Ccw;
	}
}


technique Building
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS_NoLighting();
		PixelShader  = compile ps_2_0 PS_NoLighting();

		CullMode = Ccw;
	}
}
