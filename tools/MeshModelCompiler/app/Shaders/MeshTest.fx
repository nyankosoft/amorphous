//
// MeshTest.fx
//

#define PI  3.14f

// this file contains light and texture types
#include "MeshTest.fxh"

//float4 vMaterialColor = float4(192.f/255.f, 128.f/255.f, 96.f/255.f, 1.f);
float4 vMaterialColor = float4( 1.0f, 1.0f, 1.0f, 1.0f );
float fMaterialPower = 16.f;

//float4 vAmbientColor = float4(128.f/255.f, 128.f/255.f, 128.f/255.f, 1.f);
float4 vAmbientColor = float4(0.3f, 0.3f, 0.3f, 1.f);

bool bSpecular : register(b0) = false;


// light direction (view space)
//float3 lightDir <  string UIDirectional = "Light Direction"; > = {0.577, -0.577, 0.577};

float4 g_vDLightMapColor;
float  g_fDLightMapIntensity;


float4 vSpecColor = {1.0f, 1.0f, 1.0f, 1.0f};


float3 g_vEyePos ;


float3 g_vBumpOffset = {0.5f, 0.5f, 0.5f};
float3 g_vBumpScale  = {2.0f, 2.0f, 2.0f};

float4 g_v4BumpOffset = {0.5f, 0.5f, 0.5f, 0.5f};
float4 g_v4BumpScale  = {2.0f, 2.0f, 2.0f, 0.5f};
float4 g_v4QuatScale  = {2.0f, 2.0f, 2.0f, 2.0f};


// transformation matrices
float4x4 WorldViewProj	: WORLDVIEWPROJ;
float4x4 World			: WORLD;
float4x4 View			: VIEW;
float4x4 Proj			: PROJ;

float4x4 WorldView      : WORLDVIEW;
float4x4 WorldViewIT;
float4x4 ViewIT;


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

texture Texture2;
sampler Sampler2 = sampler_state
{
    Texture   = <Texture2>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

texture Texture3;
sampler Sampler3 = sampler_state
{
    Texture   = <Texture3>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

texture Texture4;
sampler Sampler4 = sampler_state
{
    Texture   = <Texture4>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

texture Texture5;
sampler Sampler5 = sampler_state
{
    Texture   = <Texture5>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};



//-----------------------------------------------------------------------------
// vertex shader input
//-----------------------------------------------------------------------------
struct VS_INPUT
{
    float4 vPosition       :POSITION;
    float4 ColorDiff       :COLOR;
    float3 vNormal         :NORMAL;
    float3 vTangent        :TANGENT;
    float3 vBinormal       :BINORMAL;
    float4 vTexCoord       :TEXCOORD0;
 //   int4   indices        :BLENDINDICES;
 //   float4 weights        :BLENDWEIGHT;
};


//-----------------------------------------------------------------------------
// vertex shader output
//-----------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 ColorDiff : COLOR0;
    float4 ColorSpec : COLOR1;
    float2 Tex0  : TEXCOORD0;
    
    float3 vLightTS	: TEXCOORD1;
	float3 vViewTS  : TEXCOORD2;	// store direction to the camera for each vertex
};


//-----------------------------------------------------------------------------
// pixel shader input
//-----------------------------------------------------------------------------
struct PS_INPUT
{
    float4 ColorDiff	:COLOR0;
    float2 Tex			:TEXCOORD0;
    float3 vLightTS		:TEXCOORD1;
    float3 vViewTS		:TEXCOORD2;
};


//-----------------------------------------------------------------------------
// vertex structure for static geometry
//-----------------------------------------------------------------------------
struct VS_SG_OUTPUT
{
    float4 Pos  : POSITION;
    float4 Diff : COLOR0;
    float4 Spec : COLOR1;
    float2 Tex0  : TEXCOORD0;
    float2 Tex1  : TEXCOORD1;
    float2 Tex2  : TEXCOORD2;
};


//-----------------------------------------------------------------------------
// color pair (diffuse & specular)
//-----------------------------------------------------------------------------
struct COLOR_PAIR
{
   float4 Color         : COLOR0;
   float4 ColorSpec     : COLOR1;
};


//-----------------------------------------------------------------------------
// light structure
//-----------------------------------------------------------------------------
struct CLight
{
   int iType;
   float3 vPos;
   float3 vDir;
   float4 vAmbient;
   float4 vDiffuse;
   float4 vSpecular;
   float  fRange;
   float3 vAttenuation; //1, D, D^2;
   float3 vSpot;        //cos(theta/2), cos(phi/2), falloff
};

CLight g_aLight[3];

//initial and range of directional, point and spot lights within the light array
int iLightDirIni   = 0;
int iLightDirNum   = 0;
int iLightPointIni = 1;
int iLightPointNum = 0;
//int iLightSpotIni;
//int iLightSpotNum;


//-----------------------------------------------------------------------------
// Name: VS_StaticGeometry()
// Desc: vertex shader for static geometry
//-----------------------------------------------------------------------------
VS_SG_OUTPUT VS_StaticGeometry(
    float3 Pos  : POSITION, 
    float3 Norm : NORMAL,
	float4 Diffuse : COLOR0,
    float2 Tex0 : TEXCOORD0,
    float2 Tex1 : TEXCOORD1,
    float2 Tex2 : TEXCOORD2 )
{
    VS_SG_OUTPUT Out = (VS_SG_OUTPUT)0;

	float3 vWorldPos = mul(float4(Pos, 1), (float4x3)World);

	float3 P = mul(float4(Pos, 1), (float4x3)WorldView);  // position (view space)

	float3 N = normalize(mul(Norm, (float3x3)WorldView)); // normal (view space)

	float3 V = normalize(g_vEyePos  - vWorldPos);                // view direction (world space)

	Out.Diff = float4( V, 1 );

//    Out.Pos  = mul(float4(P, 1), Proj);             // position (projected)

	Out.Pos = mul( float4(Pos, 1), WorldViewProj );

    Out.Tex0  = Tex0;
    Out.Tex1  = Tex1;
    Out.Tex2  = Tex2;

    return Out;
}


//-----------------------------------------------------------------------------
// Name: PS_StaticGeometry()
// Desc: pixel shader for static geometry
//-----------------------------------------------------------------------------
// Tex0 : decal texture
// Tex1 : static lightmap
// Tex2 : dynamic lightmap

float4 PS_StaticGeometry(
    float4 Diff : COLOR0,
    float4 Spec : COLOR1,
    float2 Tex0  : TEXCOORD0,
    float2 Tex1  : TEXCOORD1, 
    float2 Tex2  : TEXCOORD2 ) : COLOR
{

	// bump mapping from decal texture
    float4 vBumpNormal   = (tex2D(Sampler3, Tex0) - g_v4BumpOffset) * g_v4BumpScale;

	// bump mapping for static geometry
    float4 vLightDir = (tex2D(Sampler4, Tex1) - g_v4BumpOffset) * g_v4BumpScale;
    float fBumpSG =    dot( vBumpNormal, vLightDir );

	// (experiment) specular color - Diff is the direction to the camera in local space
	float4 H = ( vLightDir + Diff ) / 2.0f;
	float4 vSpec = vSpecColor * pow( max(0, dot(vBumpNormal, H)), 8 ); 

	// bump mapping of dynamic lightmap
	vLightDir = (tex2D(Sampler5, Tex2)- g_v4BumpOffset) * g_v4BumpScale;
    float fBumpDL = dot( vBumpNormal, vLightDir );

	// intensity of dynamic lightmap
	float4 vDynamicLight = mul( g_fDLightMapIntensity, g_vDLightMapColor );

	return tex2D(Sampler0, Tex0) * ( tex2D(Sampler1, Tex1) * fBumpSG + tex2D(Sampler2, Tex2) * vDynamicLight * fBumpDL); + vSpec;

//	return Diff;
}


//-----------------------------------------------------------------------------
// Name: PS_PseudoNightVisionSG()
// Desc: pixel shader for static geometry when the night vision is on
//-----------------------------------------------------------------------------
float4 PS_PseudoNightVisionSG(
    float4 Diff : COLOR0,
    float4 Spec : COLOR1,
    float2 Tex0  : TEXCOORD0,
    float2 Tex1  : TEXCOORD1, 
    float2 Tex2  : TEXCOORD2 ) : COLOR
{

	// sample the normal map texture
    float3 vBump   = (tex2D(Sampler3, Tex0).xyz - g_vBumpOffset) * g_vBumpScale;

	// sample the light direction map texture
    float3 vBumpSG = (tex2D(Sampler4, Tex1).xyz - g_vBumpOffset) * g_vBumpScale;

	// calculate the bump of static geometry
    float fBump =    dot( vBump, vBumpSG );

	// calc intensity of dynamic light
	float4 vDynamicLight = mul( g_fDLightMapIntensity, g_vDLightMapColor );

	float4 vBrightness = ( tex2D(Sampler1, Tex1) * fBump + tex2D(Sampler2, Tex2) * vDynamicLight );

    // increase brightness
	vBrightness += float4(0.2f, 0.2f, 0.2f, 1.0f);
	vBrightness *= float4(4.0f, 4.0f, 4.0f, 1.0f);

	// convert pixel into grayscale
	float fBrightness = dot( tex2D(Sampler0, Tex0) * vBrightness, float4( 0.299f, 0.275f, 0.114f, 0.0f ) );

	// make the image green by subtracting red & blue component
	return float4( fBrightness - 0.35f, fBrightness, fBrightness - 0.35f, 1.0f );
}


VS_OUTPUT OC_TestVS(
    float3 Pos  : POSITION, 
    float3 Norm : NORMAL,
	float4 Diffuse : COLOR0,
    float2 Tex0 : TEXCOORD0 )
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    float4x4 WorldView = mul(World, View);

    float3 vWorldNormal = normalize(mul(Norm, (float3x3)World)); // normal (world space)

    float3 vCameraDir = float3( View._13, View._23, View._33 );
	float fCamDir = dot(vWorldNormal, vCameraDir);

    float3 P = mul(float4(Pos, 1), (float4x3)WorldView);  // position (view space)
//    float3 N = normalize(mul(Norm, (float3x3)WorldView)); // normal (view space)

    Out.Pos  = mul(float4(P, 1), Proj);             // position (projected)

    Out.Tex0  = Tex0;

	// adjust transparency
	Out.ColorDiff = float4( Diffuse.r, Diffuse.g, Diffuse.b, 0.5f + fCamDir );

    return Out;
}



//-----------------------------------------------------------------------------
// Name: DoDirLight()
// Desc: Directional light computation
//-----------------------------------------------------------------------------
COLOR_PAIR DoDirLight(float3 N, float3 V, int i)
{
   COLOR_PAIR Out;
   float3 L = mul( -normalize(g_aLight[i].vDir), (float3x3)View );
// float3 L = mul( -normalize(g_aLight[i].vDir), (float3x3)ViewIT );
   float NdotL = dot(N, L);
   Out.Color = g_aLight[i].vAmbient;
   Out.ColorSpec = 0;
   if(NdotL > 0.f)
   {
      //compute diffuse color
      Out.Color += NdotL * g_aLight[i].vDiffuse;

      //add specular component
      if(bSpecular)
      {
         float3 H = normalize(L + V);   // half vector
         Out.ColorSpec = pow(max(0, dot(H, N)), fMaterialPower) * g_aLight[i].vSpecular;
      }
   }
   return Out;
}


//-----------------------------------------------------------------------------
// Name: DoPointLight()
// Desc: Point light computation
//-----------------------------------------------------------------------------
COLOR_PAIR DoPointLight(float4 vPosition, float3 N, float3 V, int i)
{
   float3 L = mul( normalize((g_aLight[i].vPos-(float3)mul(vPosition,World))), (float3x3)View );
// float3 L = mul( normalize((g_aLight[i].vPos-(float3)mul(vPosition,World))), (float3x3)ViewIT );
   COLOR_PAIR Out;
   float NdotL = dot(N, L);
   Out.Color = g_aLight[i].vAmbient;
   Out.ColorSpec = 0;
   float fAtten = 1.f;
   if(NdotL >= 0.f)
   {
      // compute diffuse color
      Out.Color += NdotL * g_aLight[i].vDiffuse;

      // add specular component
      if(bSpecular)
      {
         float3 H = normalize(L + V);   // half vector
         Out.ColorSpec = pow(max(0, dot(H, N)), fMaterialPower) * g_aLight[i].vSpecular;
      }

      float LD = length(g_aLight[i].vPos-(float3)mul( vPosition, World ));
      if(LD > g_aLight[i].fRange)
      {
         fAtten = 0.f;
      }
      else
      {
         fAtten *= 1.f/(g_aLight[i].vAttenuation.x + g_aLight[i].vAttenuation.y*LD + g_aLight[i].vAttenuation.z*LD*LD);
      }
      Out.Color *= fAtten;
      Out.ColorSpec *= fAtten;
   }
   return Out;
}


//-----------------------------------------------------------------------------
// Name: VS_Main()
// Desc: The vertex shader
//-----------------------------------------------------------------------------

VS_OUTPUT VS_Main ( VS_INPUT In )
{
	VS_OUTPUT Out = (VS_OUTPUT) 0;

	In.vNormal = normalize(In.vNormal);

	Out.Pos = mul( In.vPosition, WorldViewProj );

	float3 vWorldPos = mul( In.vPosition, World );		// position in world space

	float3 vViewerWS = g_vEyePos  - vWorldPos;	// V
	
	float3 vViewerVS = mul( vViewerWS, (float3x3)View );

//	float3 vPosVS = mul( In.vPosition, WorldView );		// position in view space

	float3 vNormalVS = mul( In.vNormal, (float3x3)WorldView );	// normal in view space

	float3x3 WorldToTangentSpace;
	WorldToTangentSpace[0] = mul( In.vTangent, (float3x3)World );
	WorldToTangentSpace[1] = mul( cross(In.vTangent,In.vNormal), (float3x3)World );
	WorldToTangentSpace[2] = mul( In.vNormal,  (float3x3)World );

	// calc viewer and light position in tangent space
	Out.vViewTS  = mul( WorldToTangentSpace, vViewerWS );
	Out.vLightTS = mul( WorldToTangentSpace, float3(3.0, 2.0, -2.0) );

	// texture coordinate
	Out.Tex0.xy = In.vTexCoord.xy;

	// light computation
//	Out.ColorDiff = vAmbientColor; // + In.ColorDiff;
	Out.ColorDiff = In.ColorDiff;
	Out.ColorSpec = 0;

	// directional lights
	for(int i = 0; i < iLightDirNum; i++)
	{
		COLOR_PAIR ColOut = DoDirLight(vNormalVS, vViewerVS, i+iLightDirIni);
		Out.ColorDiff += ColOut.Color;
		Out.ColorSpec += ColOut.ColorSpec;
	}

	// point lights
	for(int i = 0; i < iLightPointNum; i++)
	{
		COLOR_PAIR ColOut = DoPointLight(In.vPosition, vNormalVS, vViewerVS, i+iLightPointIni);
		Out.ColorDiff += ColOut.Color;
		Out.ColorSpec += ColOut.ColorSpec;
	}

   // apply material color
//   Out.ColorDiff *= ColorDiff;	// vMaterialColor
//   Out.ColorSpec *= vMaterialColor;

   // saturate
   Out.ColorDiff = min(1, Out.ColorDiff);
   Out.ColorSpec = min(1, Out.ColorSpec);
   
   return Out;
}



//-----------------------------------------------------------------------------
// Name: PS_Main()
// Desc: The pixel shader
//-----------------------------------------------------------------------------

float4 PS_Main( PS_INPUT In ) : COLOR
{
	float4 color = tex2D(Sampler0, In.Tex) * In.ColorDiff;

	float3 vBumpNormal = 2 * ( tex2D(Sampler1, In.Tex) - 0.5 );
	
	float3 vLightDirTS = normalize(In.vLightTS);
	
	float3 vViewDirTS = normalize(In.vViewTS);
	
	float4 diff = saturate( dot(vBumpNormal,vLightDirTS) );
	
	// --- self-shadowing term ---
	float shadow = saturate( 4 * diff );
	
	float3 vReflectDirTS = normalize( 2 * diff * vBumpNormal - vLightDirTS );
	
	float4 spec = min( pow(saturate(dot(vReflectDirTS,vViewDirTS)), 3), 1 /*color.w*/ );
	
//	return color;	// door sample - almost black
	
	return 0.2 * color + shadow * (color * diff + spec);

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
        // static geometry
        VertexShader = compile vs_2_0 VS_StaticGeometry();
        PixelShader  = compile ps_2_0 PS_StaticGeometry();
    }

    pass P2
    {
        // entity
		PixelShader  = compile ps_2_0 PS_Main();
        VertexShader = compile vs_2_0 VS_Main();
    }

}