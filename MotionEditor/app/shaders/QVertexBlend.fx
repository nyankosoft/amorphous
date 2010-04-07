//
// QVertexBlend.fx
//

#define PI  3.14f

//float4 g_vMaterialColor = float4(192.f/255.f, 128.f/255.f, 96.f/255.f, 1.f);
float4 g_vMaterialColor = float4( 1.0f, 1.0f, 1.0f, 1.0f );
float g_fMaterialPower = 16.f;

float3 g_vAmbientColor = float3(0.1f, 0.1f, 0.1f);

bool g_bSpecular : register(b0) = false;

//float4 g_vDLightMapColor;
//float  g_fDLightMapIntensity;


float4 vSpecColor = {1.0f, 1.0f, 1.0f, 1.0f};


float3 g_vEyePos;	// the position of the eye(camera) in world space


//--------------------------------------------------------------------------------
// transformation matrices
//--------------------------------------------------------------------------------

#include "Matrix.fxh"


//--------------------------------------------------------------------------------
// textures
//--------------------------------------------------------------------------------

#include "TexDef.fxh"


//--------------------------------------------------------------------------------
// light
//--------------------------------------------------------------------------------

#include "LightDef.fxh"
#define NUM_LIGHTS	5
CLight g_aLight[NUM_LIGHTS];

#include "VertexLightFunctions.fxh"
//#include "PerVertexLightingsHS.fxh"

#include "EnvMap.fxh"

// per-pixel lighting with specular reflection
// uses the primary point light (g_aLight[iLightPointIni])
// as a single light source
//#include "PerPixelSinglePointLight.fxh"

#define NUM_MAX_BLEND_TRANSFORMS	72
#define NUM_MAX_BLEND_TRANSFORMS_PER_VERTEX	2

#include "QVertexBlend.fxh"


//#include "PS_Default.fxh"
//#include "PS_Specular.fxh"
//#include "VS_ForPerPixelLightsWithSpecular.fxh"
//#include "PerPixelLightingTechniques.fxh"


//--------------------------------------------------------------------------------
// Name: Default
// Desc: render with fixed function pipeline
//--------------------------------------------------------------------------------

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
