#ifndef __SimpleShadowMap_FXH__
#define __SimpleShadowMap_FXH__


#define NUM_MAX_BLEND_TRANSFORMS	72
#define NUM_MAX_BLEND_TRANSFORMS_PER_VERTEX	2

#include "QVertexBlendFunctions.fxh"

Transform g_aBlendTransform[NUM_MAX_BLEND_TRANSFORMS];


// shadow pojection modes
#define SPM_PERSPECTIVE  0
#define SPM_ORTHOGRAPHIC 1


int g_ShadowMapSize = 512; // width and height of the shadow map texture
float g_fShadowMapDistTolerance = 0.005f;
float g_fShadowOpacity = 0.6f;
float g_fCosTheta = 0.1f;
//float4x4 g_mViewToLightProj;
float4x4 g_mWorldToLightProj;
float3 g_vLightDir = { -0.56568f, -0.70711f, -0.42426f };
float3 g_vLightPos = {  5.00000f, 10.00000f,  6.00000f };
//float3 g_vLightDir = { -1.0, 0.0, 0.0 };
//float3 g_vLightPos = {  0.5, 0.0, 0.0 };

// Light plane in world space
// - Used by ortho shadow map
float4 g_LightPlane = { -0.56568f, -0.70711f, -0.42426f, -50.0f };

//#define SHADOW_EPSILON 0.00005f
//float g_fFarClip = 200.0f;

//texture  g_txScene;
texture  g_txShadow;

/*
sampler2D g_samScene =
sampler_state
{
    Texture = <g_txScene>;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = Linear;
};*/


sampler2D g_samShadow =
sampler_state
{
    Texture = <g_txShadow>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;
};


//-----------------------------------------------------------------------------
// Vertex Shader: VertScene
// Desc: Process vertex for scene
//-----------------------------------------------------------------------------
void VS_SceneShadowMap( float4 Position       : POSITION,
                        float3 Normal         : NORMAL,
                        out float4 oPos       : POSITION,
                        out float4 WorldPos   : TEXCOORD1,
                        out float3 NormalWS   : TEXCOORD2,
						out float4 LightSpacePos : TEXCOORD3 )
{
	// Transform position to view space
	oPos = mul( Position, WorldViewProj );

	// Calc world position
	WorldPos = mul( Position, World );

	// world normal
	NormalWS = mul( Normal, (float3x3)World );

	// calc pos in light projection space
	LightSpacePos = mul( WorldPos, g_mWorldToLightProj );
}

// transformation of iPos > vPosLight
// iPos > (world space) > (view space of the light) > (projection space) > vPosLight


//-----------------------------------------------------------------------------
// Vertex Shader: VertScene
// Desc: Process vertex for scene
//-----------------------------------------------------------------------------
void VS_SceneShadowMap_VertexBlend( float4 Position       : POSITION,
                                    float3 Normal         : NORMAL,
                                    int4   Indices        : BLENDINDICES,
                                    float4 Weights        : BLENDWEIGHT,
                                    out float4 oPos       : POSITION,
                                    out float4 WorldPos   : TEXCOORD1,
                                    out float3 NormalWS   : TEXCOORD2,
                                    out float4 LightSpacePos : TEXCOORD3 )
{
	float4 PosMStoWS = Position;
	float3 NormalMStoWS = Normal;

	SkinPoint(  PosMStoWS,    g_aBlendTransform, Indices, Weights );
	SkinVector( NormalMStoWS, g_aBlendTransform, Indices, Weights );

	// Transform position to view space
	oPos = mul( PosMStoWS, WorldViewProj );

	// Calc world position
	WorldPos = mul( PosMStoWS, World );

	// world normal
	NormalWS = mul( NormalMStoWS, (float3x3)World );

	// calc pos in light projection space
	LightSpacePos = mul( WorldPos, g_mWorldToLightProj );
}


float CalculateDistFromLightToVertex( uniform uint proj_mode, float3 vWorldPos )
{
	if( proj_mode == SPM_PERSPECTIVE )
	{
		return length(g_vLightPos - vWorldPos);
	}
	else
	{
		return dot( vWorldPos, g_LightPlane.xyz ) - g_LightPlane.w;
	}
}


//-----------------------------------------------------------------------------
// Pixel Shader: PixScene
// Desc: Process pixel (do per-pixel lighting) for enabled scene
//-----------------------------------------------------------------------------
float4 PS_SceneShadowMap( float4 vWorldPos : TEXCOORD1,
                          float3 vNormal   : TEXCOORD2,
                          float4 vPosLight : TEXCOORD3,
						  uniform uint proj_mode ) : COLOR
{
	// debug
//	return float4(1,1,1,1);

	if( 0.0 < dot( vNormal, g_vLightDir ) )
		return float4(1,1,1,1);

	float2 ProjectedTexCoords;

//	ProjectedTexCoords = 0.5 * vPosLight.xy / vPosLight.w + float2( 0.5, 0.5 );
	ProjectedTexCoords = 0.5 * vPosLight.xy + float2( 0.5, 0.5 );
	ProjectedTexCoords.y = 1.0f - ProjectedTexCoords.y;

	if( ProjectedTexCoords.x <= 0 || 1 <= ProjectedTexCoords.x
	 || ProjectedTexCoords.y <= 0 || 1 <= ProjectedTexCoords.y )
	{
		return float4(1,1,1,1);
	}

	float fade_margin = 0.2;
	float shadow_fade = clamp( (1.0 - abs(vPosLight.x / vPosLight.w)) / fade_margin, 0, 1 );

	vWorldPos /= vWorldPos.w;

//	float len = length(g_vLightPos - vWorldPos) / g_fFarClip;
//	float len = length(g_vLightPos - vWorldPos.xyz);// / g_fFarClip;
	float len = CalculateDistFromLightToVertex( proj_mode, vWorldPos.xyz );

	float3 color = 1.0f - len;

	// Sample a single shadowmap texel
//	float4 moments = tex2D(g_samShadow, ProjectedTexCoords);

	// Sample 9 shadowmap texels and interpolate
	float t = 1.0 / (float)g_ShadowMapSize;
	float4 moments
		= tex2D(g_samShadow, ProjectedTexCoords)
		+ tex2D(g_samShadow, ProjectedTexCoords + float2(-t,-t)) * 0.367879 // e ^ (-1)
		+ tex2D(g_samShadow, ProjectedTexCoords + float2( 0,-t)) * 0.606531 // e ^ (-1/2)
		+ tex2D(g_samShadow, ProjectedTexCoords + float2( t,-t)) * 0.367879
		+ tex2D(g_samShadow, ProjectedTexCoords + float2( t, 0)) * 0.606531
		+ tex2D(g_samShadow, ProjectedTexCoords + float2( t, t)) * 0.367879
		+ tex2D(g_samShadow, ProjectedTexCoords + float2( 0, t)) * 0.606531
		+ tex2D(g_samShadow, ProjectedTexCoords + float2(-t, t)) * 0.367879
		+ tex2D(g_samShadow, ProjectedTexCoords + float2(-t, 0)) * 0.606531;
//	moments /= 9.0;
	moments /= ( 1.0 + 0.367879 * 4.0 + 0.606531 * 4.0 );

//	moments.x += 0.01;

//	const float bias = 0.005;
	float bias = g_fShadowMapDistTolerance;
//	float bias = 1.0 + dot( g_LightDir, (Normal * float3x3(World)) );
	float shadow = 0.0;
	if( moments.x + bias < len )
	{
		// shadowed
		shadow = g_fShadowOpacity;
	}
	else
	{
		// not shadowed
		shadow = 0.0;
	}

	float inv_shadow = 1.0 - shadow;

	return float4( float3(1,1,1) * inv_shadow, 1 );

//	return float4(1,1,1,1); // debug
//	return float4( float3(1,1,1) * len * 0.01, 1 ); // debug
//	return float4( ProjectedTexCoords, 1, 1 ); // debug
//	return float4( color * shadow, 1 ); // pixels get darker when they are away from the light position
//	return float4( float3(1,1,1) * shadow, 1 ); // for directional light, there should be no attenuation
//	return float4( float3(1,1,1) * shadow, 1 ); // fade the shadow toward the edge of the shadowmap
//	return float4( float3(1,1,1) * moments.x, 1 ); // debug
//	return float4( float3(1,1,1) * moments.y, 1 ); // debug
//	return float4( float3(p,p,p), 1 ); // debug
//	return float4( float3(1,1,1) * shadow, 1 ); // debug
//	return float4( vPosLight.xyz, 1 ); // debug
//	return float4( moments.xyz, 1 ); // debug
//	return float4( color, 1 ); // debug
//	return float4( float3(1,1,1) * moments.x + 0.005 < len ? 0 : 1, 1 ); // debug
}



//-----------------------------------------------------------------------------
// Vertex Shader: VertShadow
// Desc: Process vertex for the shadow map
//-----------------------------------------------------------------------------
void VS_Shadow( float4     Pos       : POSITION,
                float3     Normal    : NORMAL,
                out float4 oPos      : POSITION,
                out float4 WorldPos  : TEXCOORD0 )
{
	// Compute the projected coordinates
	oPos = mul( Pos, WorldViewProj );

	// Store z and w in our spare texcoord
//	WorldPos = mul( Pos, World );

	// from 'GenerateVSM.fx' of NVIDIA SDK
    WorldPos = mul( float4( Pos.xyz, 1.0f ), World );
	WorldPos = float4( length( g_vLightPos.xyz - WorldPos.xyz ), 0, 0, 0 );
}


void VS_OrthoShadow( float4     Pos       : POSITION,
                     float3     Normal    : NORMAL,
                     out float4 oPos      : POSITION,
                     out float4 WorldPos  : TEXCOORD0 )
{
	// Compute the projected coordinates
	oPos = mul( Pos, WorldViewProj );

	// from 'GenerateVSM.fx' of NVIDIA SDK
    WorldPos = mul( float4( Pos.xyz, 1.0f ), World );

	// calculate the distance from the light plane in world space
	float d = dot( WorldPos.xyz / WorldPos.w, g_LightPlane.xyz ) - g_LightPlane.w;
	WorldPos = float4(d,0,0,0);
}


// from 'GenerateVSM.fx' of NVIDIA SDK
void PS_Shadow( float4 WorldPos : TEXCOORD0,
                out float4 Color : COLOR )
{
	float depth = WorldPos.x;
    Color = float4( depth, 0.0f, 0.0f, 1.0f );
}


//-----------------------------------------------------------------------------
// Vertex Shader: VS_SceneWithShadow
// Desc: 
//-----------------------------------------------------------------------------
void VS_SceneWithShadow( float4 Pos : POSITION,
                float2 Tex : TEXCOORD0,
                out float4 oPos : POSITION,
                out float2 oTex : TEXCOORD0 )
{
	oPos = Pos;
	oTex = Tex;
}


//-----------------------------------------------------------------------------
// Pixel Shader: PS_SceneWithShadow
// Desc: calc the pixel color of the shadowed scene 
//-----------------------------------------------------------------------------
void PS_SceneWithShadow( float2 tex : TEXCOORD0,
                         out float4 Color : COLOR )
{
///	Color = float4(1,0,0,1);
	Color = tex2D( Sampler0, tex ) * tex2D( Sampler1, tex );
//	Color.a = 1.0;
}


void VS_Shadow_VertexBlend( float4     Pos       : POSITION,
                            int4       Indices   : BLENDINDICES,
                            float4     Weights   : BLENDWEIGHT,
                            out float4 oPos      : POSITION,
                            out float4 WorldPos  : TEXCOORD0 )
{

	float4 PosMStoWS = Pos;

	SkinPoint( PosMStoWS, g_aBlendTransform, Indices, Weights );

    // Store z and w in our spare texcoord
    WorldPos = mul( PosMStoWS, World );

    oPos = mul( PosMStoWS, WorldViewProj );

	// from 'GenerateVSM.fx' of NVIDIA SDK
    WorldPos = mul( float4( PosMStoWS.xyz, 1.0f ), World );
	WorldPos = float4( length( g_vLightPos.xyz - WorldPos.xyz ), 0, 0, 0 );
}


//-----------------------------------------------------------------------------
// Technique: SceneShadow
// Desc: calculate shadowed pixels in the scene
//-----------------------------------------------------------------------------
technique SceneShadowMap
{
    pass p0
    {
		ZEnable = True;
		ZWriteEnable = True;
		CullMode = Ccw;

        VertexShader = compile vs_2_0 VS_SceneShadowMap();
        PixelShader = compile ps_2_0 PS_SceneShadowMap( SPM_PERSPECTIVE );
    }
}


//-----------------------------------------------------------------------------
// Technique: SceneShadow
// Desc: calculate shadowed pixels in the scene
//-----------------------------------------------------------------------------
technique OrthoSceneShadowMap
{
    pass p0
    {
		ZEnable = True;
		ZWriteEnable = True;
		CullMode = Ccw;

        VertexShader = compile vs_2_0 VS_SceneShadowMap();
        PixelShader = compile ps_2_0 PS_SceneShadowMap( SPM_ORTHOGRAPHIC );
    }
}


//-----------------------------------------------------------------------------
// Technique: SceneShadow
// Desc: calculate shadowed pixels in the scene
//-----------------------------------------------------------------------------
technique SceneShadowMap_VertexBlend
{
    pass p0
    {
		ZEnable = True;
		ZWriteEnable = True;
		CullMode = Ccw;

        VertexShader = compile vs_2_0 VS_SceneShadowMap_VertexBlend();
        PixelShader = compile ps_2_0 PS_SceneShadowMap( SPM_PERSPECTIVE );
    }
}


//-----------------------------------------------------------------------------
// Technique: ShadowMap
// Desc: Renders the shadow map
//-----------------------------------------------------------------------------
technique ShadowMap
{
    pass p0
    {
		ZEnable = True;
		ZWriteEnable = True;
		CullMode = Ccw;
		AlphaBlendEnable = False;
		AlphaTestEnable = False;

        VertexShader = compile vs_2_0 VS_Shadow();
        PixelShader = compile ps_2_0 PS_Shadow();
    }
}


//-----------------------------------------------------------------------------
// Technique: ShadowMap
// Desc: Renders the shadow map
//-----------------------------------------------------------------------------
technique OrthoShadowMap
{
    pass p0
    {
		ZEnable = True;
		ZWriteEnable = True;
		CullMode = Ccw;
		AlphaBlendEnable = False;
		AlphaTestEnable = False;

        VertexShader = compile vs_2_0 VS_OrthoShadow();
        PixelShader = compile ps_2_0 PS_Shadow();
    }
}


//-----------------------------------------------------------------------------
// Technique: ShadowMap_VertexBlend
// Desc: Renders the shadow map
//-----------------------------------------------------------------------------
technique ShadowMap_VertexBlend
{
    pass p0
    {
		ZEnable = True;
		ZWriteEnable = True;
		CullMode = Ccw;
		AlphaBlendEnable = False;
		AlphaTestEnable = False;

        VertexShader = compile vs_2_0 VS_Shadow_VertexBlend();
        PixelShader = compile ps_2_0 PS_Shadow();
    }
}


//-----------------------------------------------------------------------------
// Technique: 
// Desc: 
//-----------------------------------------------------------------------------
technique SceneWithShadow
{
    pass p0
    {
		ZEnable = False;
		ZWriteEnable = False;

        VertexShader = NULL;//compile vs_2_0 VS_SceneWithShadow();
        PixelShader = compile ps_2_0 PS_SceneWithShadow();
    }
}


#endif  /* __SimpleShadowMap_FXH__ */
