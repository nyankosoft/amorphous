//---------------------------------------------------------------
// vertex & pixel shader for objects which is not lit
// and is rendered with their original 
// vertex diffuse / texture color with additive alpha operation
//---------------------------------------------------------------
//  matrices:
//   WorldViewProj
//  variables:
//  //g_vAmbientColor
//  textures:
//   Sampler0	texture for color channel
//---------------------------------------------------------------


void VS_DiffColorWithNoLight( float3 Pos  : POSITION, 
					 float4 Diffuse : COLOR0,
					 out float4 oPos : POSITION,
					 out float4 oDiffuse : COLOR0 )
{
	oPos = mul( float4(Pos,1.0), WorldViewProj );

	// saturate
	oDiffuse = Diffuse;
}


float4 PS_DiffColorWithNoLight(	float4 Diffuse : COLOR0 ) : COLOR
{
	return Diffuse;
}


technique DiffColorWithNoLight_AdditiveDestAlphaBlend
{
	pass P0
	{
		SrcBlendAlpha = SrcAlpha;
		DestBlendAlpha = One;
		BlendOpAlpha = Add;

		CullMode = Ccw;

		VertexShader = compile vs_2_0 VS_DiffColorWithNoLight();
		PixelShader  = compile ps_2_0 PS_DiffColorWithNoLight();
	}
}


void VS_DiffTexColorWithNoLight( float3 Pos  : POSITION, 
					 float3 Normal : NORMAL,
					 float4 Diffuse : COLOR0,
					 float2 Tex0 : TEXCOORD0,
					 out float4 oPos : POSITION,
					 out float4 oDiffuse : COLOR0,
					 out float2 oTex : TEXCOORD0 )
{
	oPos = mul( float4(Pos,1.0), WorldViewProj );

	// saturate
	oDiffuse = Diffuse;

	// texture coordinate
	oTex = Tex0;
}


float4 PS_DiffTexColorWithNoLight(	float4 Diffuse : COLOR0,
								float2 Tex : TEXCOORD0 ) : COLOR
{
	float4 color = tex2D(Sampler0, Tex) * Diffuse;

	color.a = 1.0f;

	return color;
}


technique DiffTexColorWithNoLight_AdditiveDestAlphaBlend
{
	pass P0
	{
		SrcBlendAlpha = SrcAlpha;
		DestBlendAlpha = One;
		BlendOpAlpha = Add;

		CullMode = Ccw;

		VertexShader = compile vs_2_0 VS_DiffTexColorWithNoLight();
		PixelShader  = compile ps_2_0 PS_DiffTexColorWithNoLight();
	}
}