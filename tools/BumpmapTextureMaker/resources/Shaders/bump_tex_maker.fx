

// transformation matrices
float4x4 WorldViewProj	: WORLDVIEWPROJ;
float4x4 World			: WORLD;
float4x4 View			: VIEW;
float4x4 Proj			: PROJ;
float4x4 WorldView      	: WORLDVIEW;

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


float3 g_vLightDir;


//--------------------------------------------------------------------------------
// Name: VS_NormalMap()
// Desc: vertex shader for rendering normal maps
//--------------------------------------------------------------------------------

void VS_NormalMap(
	float3 Pos  : POSITION, 
	float3 Normal : NORMAL,
	float4 Diffuse : COLOR0,
	out float4 oPos : POSITION,
	out float4 oDiffuse : COLOR0 )
{

	oPos = mul( float4(Pos,1), WorldViewProj );

	float3 vNormal = normalize(Normal);

//	float3 vWorldNormal = normalize(mul(Normal, (float3x3)World)); // normal (world space)

//	oDiffuse = ( vWorldNormal + float4( 1.0f, 1.0f, 1.0f, 1.0f ) ) * 0.5f;
	oDiffuse.rgb = ( vNormal + float3( 1.0f, 1.0f, 1.0f ) ) * 0.5f;

	oDiffuse.a = 1.0f;
}


//--------------------------------------------------------------------------------
// Name: PS_NormalMap()
// Desc: pixel shader for rendering normal maps
//--------------------------------------------------------------------------------

float4 PS_NormalMap(
       float4 Diffuse : COLOR0,
       float2 Tex0 : TEXCOORD0 ) : COLOR
{
	return Diffuse;
}


technique NormalMap
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS_NormalMap();
		PixelShader  = compile ps_2_0 PS_NormalMap();
//		CullMode = Ccw;
	}
}



//--------------------------------------------------------------------------------
// Name: VS_FlatTexSurface()
// Desc: vertex shader for FlatTexSurface effect
//--------------------------------------------------------------------------------

void VS_FlatTexSurface(
	float3 Pos  : POSITION, 
	float3 Normal : NORMAL,
	float4 Diffuse : COLOR0,
	float2 Tex0 : TEXCOORD0,
	out float4 oPos : POSITION,
	out float4 oDiffuse : COLOR0,
	out float2 oTex : TEXCOORD0 )
{
	oPos = mul( float4(Pos,1), WorldViewProj );

	oTex = Tex0;

	oDiffuse = Diffuse;

}


//--------------------------------------------------------------------------------
// Name: PS_FlatTexSurface()
// Desc: pixel shader for FlatTexSurface effect
//--------------------------------------------------------------------------------

float4 PS_FlatTexSurface(
       float4 Diffuse : COLOR0,
       float2 Tex0 : TEXCOORD0 ) : COLOR
{
	// just output texture color
	float4 color = tex2D(Sampler0, Tex0) * Diffuse;

	color.a = 1.0;

	return color;
}


technique FlatTexSurface
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS_FlatTexSurface();
		PixelShader  = compile ps_2_0 PS_FlatTexSurface();
//		CullMode = Ccw;
	}
}




//--------------------------------------------------------------------------------
// Name: VS_BumpyTexturedSurface()
// Desc: vertex shader for FlatTexSurface effect
//--------------------------------------------------------------------------------

void VS_BumpyTexturedSurface(
	float3 Pos  : POSITION, 
	float3 Normal : NORMAL,
	float4 Diffuse : COLOR0,
	float2 Tex0 : TEXCOORD0,
	out float4 oPos : POSITION,
	out float4 oDiffuse : COLOR0,
	out float2 oTex : TEXCOORD0,
	out float3 oWorldNormal : TEXCOORD1 )
{
	oPos = mul( float4(Pos,1), WorldViewProj );

	oWorldNormal = normalize(mul(Normal, (float3x3)World)); // normal (world space)

	oTex = Tex0;

	oDiffuse = Diffuse;

}


//--------------------------------------------------------------------------------
// Name: PS_BumpyTexturedSurface()
// Desc: pixel shader for FlatTexSurface effect
//--------------------------------------------------------------------------------

float4 PS_BumpyTexturedSurface(
       float4 Diffuse : COLOR0,
       float2 Tex0 : TEXCOORD0,
	   float3 WorldNormal : TEXCOORD1 ) : COLOR
{
	float3 Normal = normalize( WorldNormal );

	// just output texture color
	return tex2D(Sampler0, Tex0) * Diffuse * saturate( dot(g_vLightDir,Normal) );


}


technique BumpyTexturedSurface
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS_BumpyTexturedSurface();
		PixelShader  = compile ps_2_0 PS_BumpyTexturedSurface();
//		CullMode = Ccw;
	}
}




//--------------------------------------------------------------------------------
// Name: VS_AlphaSurface()
// Desc: vertex shader for AlphaSurface effect
//--------------------------------------------------------------------------------

void VS_AlphaSurface(
	float3 Pos  : POSITION, 
	float3 Normal : NORMAL,
	float4 Diffuse : COLOR0,
	float2 Tex0 : TEXCOORD0,
	out float4 oPos : POSITION,
	out float4 oDiffuse : COLOR0,
	out float2 oTex : TEXCOORD0 )
{
	oPos = mul( float4(Pos,1), WorldViewProj );

	float3 vNormal = normalize(Normal);

	oTex = Tex0;

	oDiffuse.rgb = Diffuse.rgb;

//	oDiffuse.a = vNormal.z * 0.25f;

	float z = vNormal.z;
	oDiffuse.a = ( 1.0 - sqrt( 1.0 - z*z ) ) * 0.5;
}


//--------------------------------------------------------------------------------
// Name: PS_AlphaSurface()
// Desc: pixel shader for AlphaSurface effect
//--------------------------------------------------------------------------------

float4 PS_AlphaSurface(
       float4 Diffuse : COLOR0,
       float2 Tex0 : TEXCOORD0 ) : COLOR
{
	// just output texture color
	float4 color = tex2D(Sampler0, Tex0) * Diffuse;

	color.a = Diffuse.a;

	return color;
}


technique AlphaSurface
{
	pass P0
	{
		AlphaBlendEnable = True;
		SrcBlendAlpha = SrcAlpha;			// not functioning - what's wrong?
		DestBlendAlpha = InvSrcAlpha;		// not functioning - what's wrong?
		VertexShader = compile vs_1_1 VS_AlphaSurface();
		PixelShader  = compile ps_2_0 PS_AlphaSurface();
//		CullMode = Ccw;
	}
}