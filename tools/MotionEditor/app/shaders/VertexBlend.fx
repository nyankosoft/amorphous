//
// Default.fx
//

#define PI  3.14f

//float4 g_vMaterialColor = float4(192.f/255.f, 128.f/255.f, 96.f/255.f, 1.f);
float4 g_vMaterialColor = float4( 1.0f, 1.0f, 1.0f, 1.0f );
float g_fMaterialPower = 16.f;

float3 g_vAmbientColor = float3(0.1f, 0.1f, 0.1f);

bool g_bSpecular : register(b0) = false;

// light direction (view space)
//float3 lightDir <  string UIDirectional = "Light Direction"; > = {0.577, -0.577, 0.577};

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

#define NUM_MAX_BLEND_MATRICES	36
#define NUM_MAX_BLEND_MATRICES_PER_VERTEX	2

#include "VertexBlend.fxh"


#define MESH_RENDER_NORMAL		0
#define MESH_RENDER_PSEUDO_NIGHT_VISION	1


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

/*
technique DefaultLight
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Default();
		PixelShader  = compile ps_2_0 PS_Default();

		CullMode = Ccw;
		Sampler[0] = (Sampler0);
	}
}
*/


/// assumes lighting calcs have been done in VS except for specular calc
/// calculates specular using g_aLight[0] as a directional light
//void PS_ForVertexLights_Specular_1DL(
