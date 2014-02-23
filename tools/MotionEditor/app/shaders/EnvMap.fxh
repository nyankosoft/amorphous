//-----------------------------------------------------------------------------
// File: HDRCubeMap.fx
//
// Desc: Effect file for high dynamic range cube mapping sample.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


#include "Matrix.fxh"

texture  g_txCubeMap;
texture  g_txCubeMap2;

float    g_fReflectivity = 0.5f;            // Reflectivity value


//-----------------------------------------------------------------------------
// Sampler: samCubeMap
// Desc: Process vertex for HDR environment mapping
//-----------------------------------------------------------------------------
samplerCUBE g_samCubeMap = 
sampler_state
{
    Texture = <g_txCubeMap>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};


samplerCUBE g_samCubeMap2 = 
sampler_state
{
    Texture = <g_txCubeMap2>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};


//-----------------------------------------------------------------------------
// Vertex Shader: HDRVertEnvMap
// Desc: Process vertex for HDR environment mapping
//-----------------------------------------------------------------------------
void HDRVertEnvMap( float4 Pos : POSITION,
                    float3 Normal : NORMAL,
                    out float4 oPos : POSITION,
                    out float3 EnvTex : TEXCOORD0 )
{
    oPos = mul( Pos, WorldView );

    //
    // Compute normal in camera space
    //
    float3 vN = mul( Normal, WorldView );
    vN = normalize( vN );

    //
    // Obtain the reverse eye vector
    //
    float3 vEyeR = -normalize( oPos );

    //
    // Compute the reflection vector
    //
    float3 vRef = 2 * dot( vEyeR, vN ) * vN - vEyeR;

    //
    // Store the reflection vector in texcoord0
    //
    EnvTex = vRef;

    //
    // Apply the projection
    //
    oPos = mul( oPos, Proj );
}


float3 CalcCubeTexCoord( float3 PosVS, float3 NormalVS )
{
	float3 DirToEye = -normalize(PosVS);
	
	float3 Ref = 2.0 * dot( DirToEye, NormalVS ) * NormalVS - DirToEye;

	return Ref;
}


//-----------------------------------------------------------------------------
// Pixel Shader: HDRPixEnvMap
// Desc: Process pixel for HDR environment mapped object
//-----------------------------------------------------------------------------
float4 HDRPixEnvMap( float3 Tex : TEXCOORD0 ) : COLOR
{
    return g_fReflectivity * texCUBE( g_samCubeMap, Tex );
}


float4 HDRPixEnvMap2Tex( float3 Tex : TEXCOORD0 ) : COLOR
{
    return g_fReflectivity * float4( texCUBE( g_samCubeMap, Tex ).xy, texCUBE( g_samCubeMap2, Tex ).xy );
}



//-----------------------------------------------------------------------------
// Technique: RenderEnvMesh
// Desc: Renders the HDR environment-mapped mesh
//-----------------------------------------------------------------------------
technique RenderHDREnvMap
{
    pass p0
    {
        VertexShader = compile vs_2_0 HDRVertEnvMap();
        PixelShader = compile ps_2_0 HDRPixEnvMap();
    }
}


technique RenderHDREnvMap2Tex
{
    pass p0
    {
        VertexShader = compile vs_2_0 HDRVertEnvMap();
        PixelShader = compile ps_2_0 HDRPixEnvMap2Tex();
    }
}
