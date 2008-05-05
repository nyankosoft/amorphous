//==========================================================
//  SkySphereFG.fxh
// ---------------------------------------------------------
//  matrices:
//   WorldViewProj
//  textures:
//   Sampler0
//==========================================================


/*

 ### tex coord shifting ###

 tex.v = g_CameraHeight * 0.000005

				tex v shift factor
cam height		0.000005
--------------------------------------
100,000m		0.5000
 50,000m		0.2500
 20,000m		0.1000
 10,000m		0.0500
  5,000m		0.0250
  1,000m		0.0050
    500m		0.0025


 ### Layers of the Earth's atmosphere ###

up to x[m]	is	y
--------------------------------------
350,000m		Ionosphere(Aurora)
 90,000m		Mesosphere
 50,000m		Stratosphere
 14,000m		Troposphere


 ### Ceiling of Some Famous Aircrafts (from FAS) ###

name	ceiling[m]	ceiling[ft]		(+ = above, s=service ceiling)
-----------------------------------------
F-14				53,000+
F-15	19,697		65,000
Mi-24	 4,500s
B-1		 9,000+		30,000+
B-2		15,152		50,000


*/

float g_TexVShiftFactor = 0.000005;
//float g_TexVShiftFactor = 0.00005;

float g_CameraHeight = 500;	// height of the camera [m]


//--------------------------------------------------------------------------------
// Name: VS_SkySphereFG()
// Desc: vertex shader for skybox
//--------------------------------------------------------------------------------
void VS_SkySphereFG( float4 Pos  : POSITION, 
                float2 Tex0 : TEXCOORD0,
                out float4 oPos : POSITION,
                out float2 oTex0 : TEXCOORD0 )
{
	oPos = mul( Pos, WorldViewProj );

	oTex0 = Tex0 + float2( 0, -g_CameraHeight * g_TexVShiftFactor );
}


//--------------------------------------------------------------------------------
// Name: PS_SkySphereFG()
// Desc: pixel shader for skybox
//--------------------------------------------------------------------------------
float4 PS_SkySphereFG( float2 Tex0 : TEXCOORD0 ) : COLOR
{
	// just output texture color
//	return tex2D(Sampler0, Tex0);

	return float4( tex2D(Sampler0, Tex0).rgb, 1 );
}


technique SkySphereFG
{
	pass P0
	{
		AddressV[0] = Clamp;

		VertexShader = compile vs_1_1 VS_SkySphereFG();
		PixelShader  = compile ps_2_0 PS_SkySphereFG();

		CullMode = Ccw;
		ZWriteEnable = False;
		AlphaBlendEnable = False;
	}
}