
//
// SimpleShadowMap.fx
//

#define PI  3.14f

// this file contains light and texture types
//#include "Default.fxh"

//float4 g_vMaterialColor = float4(192.f/255.f, 128.f/255.f, 96.f/255.f, 1.f);
float4 g_vMaterialColor = float4( 1.0f, 1.0f, 1.0f, 1.0f );
float g_fMaterialPower = 16.f;

//float4 g_vAmbientColor = float4(128.f/255.f, 128.f/255.f, 128.f/255.f, 1.f);
float4 g_vAmbientColor = float4(0.1f, 0.1f, 0.1f, 1.f);


float3 g_vEyePos;	// the position of the eye(camera) in world space


// transformation matrices
float4x4 WorldViewProj	: WORLDVIEWPROJ;
float4x4 World			: WORLD;
//float4x4 View			: VIEW;
//float4x4 Proj			: PROJ;

float4x4 WorldView      : WORLDVIEW;


//--------------------------------------------------------------------------------
// textures
//--------------------------------------------------------------------------------

#include "TexDef.fxh"


#include "SimpleShadowMap.fxh"
