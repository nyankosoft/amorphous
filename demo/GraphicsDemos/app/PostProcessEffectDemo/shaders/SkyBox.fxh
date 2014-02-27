//==========================================================
//  SkyBox.fxh
// ---------------------------------------------------------
//  matrices:
//   WorldViewProj
//  textures:
//   Sampler0
//==========================================================


//--------------------------------------------------------------------------------
// Name: VS_SkyBox()
// Desc: vertex shader for skybox
//--------------------------------------------------------------------------------
void VS_SkyBox( float4 Pos  : POSITION, 
                float2 Tex0 : TEXCOORD0,
                out float4 oPos : POSITION,
                out float2 oTex0 : TEXCOORD0 )
{
	oPos = mul( Pos, WorldViewProj );

	oTex0 = Tex0;
}


//--------------------------------------------------------------------------------
// Name: PS_SkyBox()
// Desc: pixel shader for skybox
//--------------------------------------------------------------------------------
float4 PS_SkyBox( float2 Tex0 : TEXCOORD0 ) : COLOR
{
	// just output texture color
//	return tex2D(Sampler0, Tex0);
	return float4( tex2D(Sampler0, Tex0).rgb, 1 );
}


technique SkyBox
{
	pass P0
	{
		CullMode = CCW;
//		ZEnable = False;
		ZWriteEnable = False;
		AlphaBlendEnable = False;

		VertexShader = compile vs_1_1 VS_SkyBox();
		PixelShader  = compile ps_2_0 PS_SkyBox();
	}
}