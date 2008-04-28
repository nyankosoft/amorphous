//
// MeshTest.fx
//

#define PI  3.14f


//float4 vMaterialColor = float4(192.f/255.f, 128.f/255.f, 96.f/255.f, 1.f);
float4 vMaterialColor = float4( 1.0f, 1.0f, 1.0f, 1.0f );
float fMaterialPower = 16.f;

//float4 vAmbientColor = float4(128.f/255.f, 128.f/255.f, 128.f/255.f, 1.f);
float4 vAmbientColor = float4(0.1f, 0.1f, 0.1f, 1.f);

bool bSpecular : register(b0) = false;


float4 vSpecColor = {1.0f, 1.0f, 1.0f, 1.0f};


// transformation matrices
float4x4 WorldViewProj	: WORLDVIEWPROJ;
float4x4 World		: WORLD;
float4x4 View		: VIEW;
float4x4 Proj		: PROJ;

float4x4 WorldView      : WORLDVIEW;

//-----------------------------------------------------------------------------
// textures
//-----------------------------------------------------------------------------
texture Texture0;
sampler Sampler0 = sampler_state
{
    Texture   = <Texture0>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

texture Texture1;
sampler Sampler1 = sampler_state
{
    Texture   = <Texture1>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};


struct VS_INPUT
{
    float4 vPosition       :POSITION;
    float4 ColorDiff       :COLOR;
    float3 vNormal         :NORMAL;
//    float3 vTangent        :TANGENT;
//    float3 vBinormal       :BINORMAL;
    float4 vTexCoord       :TEXCOORD0;
 //   int4   indices        :BLENDINDICES;
 //   float4 weights        :BLENDWEIGHT;
};


//-----------------------------------------------------------------------------
// single texture output - used by entities
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos  	: POSITION;
    float4 ColorDiff 	: COLOR0;
    float2 Tex0  	: TEXCOORD0;

};


struct PS_INPUT
{
    float4 ColorDiff	:COLOR0;
    float2 Tex		:TEXCOORD0;
};


//-----------------------------------------------------------------------------
// Name: VS_Main()
// Desc: The vertex shader
//-----------------------------------------------------------------------------

VS_OUTPUT VS_Main ( VS_INPUT In )
{
   VS_OUTPUT Out = (VS_OUTPUT) 0;

   In.vNormal = normalize(In.vNormal);
   Out.Pos = mul( In.vPosition, WorldViewProj );

   float4 P = mul( In.vPosition, WorldView );           // position in view space
//   float3 P = p.xyz;
   float3 N = mul( In.vNormal, (float3x3)WorldView ); // normal in view space
   float3 V = -normalize(P);                          // viewer

   // texture coordinate
   Out.Tex0.xy = In.vTexCoord.xy;

   // light computation
   Out.ColorDiff = In.ColorDiff + vAmbientColor;


   // saturate
   Out.ColorDiff = min(1, Out.ColorDiff);

   return Out;
}


float4 PS_Main( PS_INPUT In ) : COLOR
{
	float4 color = tex2D(Sampler0, In.Tex) * In.ColorDiff;

	return color;
}



technique MultiPassShader
{
    pass P0
    {
        // default
        VertexShader = NULL;
        PixelShader  = NULL;
    }

	pass P1
    {
        // entity
        VertexShader = compile vs_2_0 VS_Main();
 		PixelShader  = compile ps_2_0 PS_Main();
   }

}