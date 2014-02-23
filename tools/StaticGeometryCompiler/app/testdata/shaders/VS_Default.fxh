

// shader for models rendered with
//  multiple directional/point lights
//  no bump mapping


//--------------------------------------------------------------------------------
// vertex shader input
//--------------------------------------------------------------------------------
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


//--------------------------------------------------------------------------------
// vertex shader output
//--------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 ColorDiff : COLOR0;
    float4 ColorSpec : COLOR1;
    float2 Tex0  : TEXCOORD0;
    
    float3 vLightTS	: TEXCOORD1;
	float3 vViewTS  : TEXCOORD2;	// store direction to the camera for each vertex
};


//--------------------------------------------------------------------------------
// pixel shader input
//--------------------------------------------------------------------------------
struct PS_INPUT
{
    float4 ColorDiff	:COLOR0;
    float2 Tex			:TEXCOORD0;
    float3 vLightTS		:TEXCOORD1;
    float3 vViewTS		:TEXCOORD2;
};


VS_OUTPUT VS_Default( VS_INPUT In )
{
	VS_OUTPUT Out = (VS_OUTPUT) 0;

	Out.Pos = mul( In.vPosition, WorldViewProj );

	float3 vWorldPos = mul( In.vPosition, World );		// position in world space

	float3 vViewerWS = g_vEyePos - vWorldPos;	// V
	
	float3 vViewerVS = mul( vViewerWS, (float3x3)View );

	float3 vNormalVS = mul( In.vNormal, (float3x3)WorldView );	// normal in view space

	// calc viewer and light position in tangent space
	Out.vViewTS  = float3(0,0,0);
//	Out.vLightTS = float3(0,0,0);

	// light computation
	Out.ColorDiff = 0;
	Out.ColorSpec = 0;
	
	float diffuse_light_amount = 0;

	// directional lights
	for(int i = 0; i < iLightDirNum; i++)
	{
		COLOR_PAIR ColOut = DoHemisphericDirLight(vNormalVS, vViewerVS, i+iLightDirIni);
		diffuse_light_amount += ColOut.Color;
		Out.ColorSpec += ColOut.ColorSpec;
	}

	// point lights
	for(int i = 0; i < iLightPointNum; i++)
	{
		COLOR_PAIR ColOut = DoHemisphericPointLight(float4(vWorldPos,1), vNormalVS, vViewerVS, i+iLightPointIni);
		diffuse_light_amount += ColOut.Color;
		Out.ColorSpec += ColOut.ColorSpec;
	}

	// apply material color
//	Out.ColorDiff *= ColorDiff;	// g_vMaterialColor
//	Out.ColorSpec *= g_vMaterialColor;

	// saturate
	Out.ColorDiff = diffuse_light_amount + min(1, Out.ColorDiff);
	Out.ColorSpec = min(1, Out.ColorSpec);

	// texture coordinate
	Out.Tex0.xy = In.vTexCoord.xy;

	return Out;
}