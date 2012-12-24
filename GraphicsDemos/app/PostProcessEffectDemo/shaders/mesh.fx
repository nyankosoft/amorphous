//-----------------------------------------------------------------------------
// File: Scene.fx
//
// Desc: Effect file for image post-processing sample.  This effect contains
//       a technique that renders a scene with vertex and pixel shaders.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


float4x4 WorldViewProj	: WORLDVIEWPROJ;
float4x4 World			: WORLD;
float4x4 View			: VIEW;
float4x4 Proj			: PROJ;

float4x4 WorldView		: WORLDVIEW;

texture g_txScene;

sampler2D g_samScene =
sampler_state
{
    Texture = <g_txScene>;
    AddressU = Wrap;
    AddressV = Wrap;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = Linear;
};


//--------------------------------------------------------------------------------
// textures
//--------------------------------------------------------------------------------

#include "TexDef.fxh"



//--------------------------------------------------------------------------------
// vertex & pixel shaders for vertex blending
//--------------------------------------------------------------------------------


//#define NUM_MAX_BLEND_MATRICES_PER_VERTEX	2

//#include "VertexBlend.fxh"
//#include "BumpVertexBlend.fxh"


//#include "VS_Default.fxh"
//#include "PS_Default.fxh"

//#include "QuickTest.fxh"

#include "SkyBox.fxh"


technique NullShader
{
	pass P0
	{
		VertexShader = NULL;
		PixelShader  = NULL;
	}
}

