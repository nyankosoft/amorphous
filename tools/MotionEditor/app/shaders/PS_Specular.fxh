#ifndef __PS_SPECULAR_H__
#define __PS_SPECULAR_H__


// shader for per-pixel lighting with specular highlights

// Dependencies:
// --- global variables ---
// textures:
//  Sampler0
// variables:
//  float3 g_vEyePos
//  float3 g_vAmbientColor
//  CLight g_aLight[NUM_LIGHTS]
//  iLightDirIni

// --- PS input stream ---
// NormalWS  vertex normal in world space
// ToViewer  vertex - viewer vector in world space

// NOTE:
// use g_aLight[0] as a hemispheric directional light


float g_Specularity = 0.5;
float g_Glossiness = 16;


float4 PS_1HSDL_Specular( float2 Tex0 : TEXCOORD0,
                          float4 ColorDiff : COLOR0,
                          float3 NormalWS : TEXCOORD1,
                          float3 ToViewer : TEXCOORD3 ) : COLOR0
{
	float4 surface_color = tex2D( Sampler0, Tex0 ) * ColorDiff;

	const float3 Normal = normalize(NormalWS); // normal in world space
	float3 ViewDir = normalize(ToViewer);      // direction to the camera in world space

	float diff_raw = dot(Normal,-g_aLight[0].vDir);
	float diff_hs = ( diff_raw + 1.0 ) * 0.5;

	float4 diffuse
	= g_aLight[0].vDiffuseUpper * diff_hs
	+ g_aLight[0].vDiffuseLower * ( 1.0 - diff_hs );

	//////////////// specular calc ////////////////
	float diff_sat = saturate( diff_raw );
	float3 Reflect = normalize(2 * diff_sat * Normal - g_aLight[0].vDir);
	float specular = pow( saturate(dot(Reflect,ViewDir)), g_Glossiness ) * g_Specularity;

	//////////////// specular calc 2 (half angle) ////////////////	// almost the same results with the above
//	float3 half_angle = normalize( ViewDir + g_aLight[0].vDir );
//	float specular = saturate( pow( dot(half_angle,Normal), 32 ) );
//	specular = saturate( pow( dot(half_angle,float3(0,1,0)), 32 ) ); * 0.7;

	float4 color;
	color.rgb = surface_color * diffuse + g_vAmbientColor + specular;
	color.a = surface_color.a;
	return color;

//	float color_fade_start = 28000.0;
//	float color_fade_end = g_fFarClip - 3000.0;
//	float a = ( color_fade_end - dist ) / ( color_fade_end - color_fade_start );
//	color = color * a + g_vTerrainFadeColor * (1.0 - clamp( a, 0.0, 1.0 ));
//	color.a = GetFadeAlpha( dist );

//	return color;
}

/// use color texture alpha for specularity map
float4 PS_1HSDL_Specular_CTS( float2 Tex0 : TEXCOORD0,
                              float4 ColorDiff : COLOR0,
                              float3 NormalWS : TEXCOORD1,
                              float3 ToViewer : TEXCOORD3 ) : COLOR0
{
	float4 tex_color =  tex2D( Sampler0, Tex0 );
	float4 surface_color = float4( tex_color.rgb, 1 ) * ColorDiff;

	const float3 Normal = normalize(NormalWS); // normal in world space
	float3 ViewDir = normalize(ToViewer);      // direction to the camera in world space

	float diff_raw = dot(Normal,-g_aLight[0].vDir);
	float diff_hs = ( diff_raw + 1.0 ) * 0.5;

	float4 diffuse
	= g_aLight[0].vDiffuseUpper * diff_hs
	+ g_aLight[0].vDiffuseLower * ( 1.0 - diff_hs );

	//////////////// specular calc ////////////////
	float diff_sat = saturate( diff_raw );
	float3 Reflect = normalize(2 * diff_sat * Normal - g_aLight[0].vDir);
	float specular = pow( saturate(dot(Reflect,ViewDir)), g_Glossiness ) * tex_color.a;

	float4 color;
	color.rgb = surface_color * diffuse + g_vAmbientColor + specular;
	color.a = surface_color.a;
	return color;
}


float3 g_vLuminanceConv = { 0.2125f, 0.7154f, 0.0721f };

/// calc specularity from tex rgb color
/// TODO: use tex alpha for alpha blend calc
float4 PS_1HSDL_Specular_CT2S( float2 Tex0 : TEXCOORD0,
                               float4 ColorDiff : COLOR0,
                               float3 NormalWS : TEXCOORD1,
                               float3 ToViewer : TEXCOORD3 ) : COLOR0
{
	float4 tex_color =  tex2D( Sampler0, Tex0 );
	float4 surface_color = float4( tex_color.rgb, 1 ) * ColorDiff;

	const float3 Normal = normalize(NormalWS); // normal in world space
	float3 ViewDir = normalize(ToViewer);      // direction to the camera in world space

	float diff_raw = dot(Normal,-g_aLight[0].vDir);
	float diff_hs = ( diff_raw + 1.0 ) * 0.5;

	float4 diffuse
	= g_aLight[0].vDiffuseUpper * diff_hs
	+ g_aLight[0].vDiffuseLower * ( 1.0 - diff_hs );

	float specularity = dot( tex_color.rgb, g_vLuminanceConv );

	//////////////// specular calc ////////////////
	float diff_sat = saturate( diff_raw );
	float3 Reflect = normalize(2 * diff_sat * Normal - g_aLight[0].vDir);
	float specular = pow( saturate(dot(Reflect,ViewDir)), g_Glossiness ) * specularity;

	float4 color;
	color.rgb = surface_color * diffuse + g_vAmbientColor + specular;
	color.a = surface_color.a;
	return color;
}


#define USE_COLOR_TEXTURE_ALPHA_FOR_SPECULAR_MAP      0
#define USE_NORMALMAP_TEXTURE_ALPHA_FOR_SPECULAR_MAP  1


float4 CalcDiffuseForPointLight( int light_index, float diff, float d )
{
	float4 light_amount
	= g_aLight[light_index].vDiffuseUpper * diff
	+ g_aLight[light_index].vDiffuseLower * ( 1.0 - diff );

	float3 att = g_aLight[light_index].vAttenuation;
	return light_amount * 1 / ( att.x + att.y * d + att.z * d * d );
}


void CalcSingleHSPointLightWithSpecular( float3 Normal,
                                         float3 ViewDir,
                                         float3 PositionWS,
                                         out float4 diffuse,
                                         out float specular )
{
	float3 ToLight = g_aLight[0].vPos - PositionWS;
	float d = length(ToLight);
	float3 DirToLight = ToLight / d;
	float diff_raw = dot(Normal,DirToLight);
	float diff_hs = ( diff_raw + 1.0 ) * 0.5;

	diffuse = CalcDiffuseForPointLight( 0, diff_hs, d );
	diffuse = clamp( diffuse, 0.0, 1.0 );

	//////////////// specular calc ////////////////
	float diff_sat = saturate( diff_raw );
	float3 Reflect = normalize(2 * diff_sat * Normal - DirToLight);
	specular = pow( saturate(dot(Reflect,ViewDir)), g_Glossiness );
}


/// calc specularity from tex rgb color
/// TODO: use tex alpha for alpha blend calc
float4 PS_1HSPL_Specular_XTS( float2 Tex0 : TEXCOORD0,
                              float4 ColorDiff : COLOR0,
                              float3 NormalWS : TEXCOORD1,
                              float3 PositionWS : TEXCOORD2,
                              float3 ToViewer : TEXCOORD3,
                              uniform int spec_factor_type = USE_COLOR_TEXTURE_ALPHA_FOR_SPECULAR_MAP
                             ) : COLOR0
{
	float4 tex_color =  tex2D( Sampler0, Tex0 );
	float4 surface_color = float4( tex_color.rgb, 1 ) * ColorDiff;

	const float3 Normal = normalize(NormalWS); // normal in world space
	float3 ViewDir = normalize(ToViewer);      // direction to the camera in world space

	// calc diffuse and specular
	float4 diffuse;
	float specular;
	CalcSingleHSPointLightWithSpecular( Normal, ViewDir, PositionWS, diffuse, specular );

	float spec_factor;
	if( spec_factor_type == USE_COLOR_TEXTURE_ALPHA_FOR_SPECULAR_MAP )
		spec_factor = tex_color.a;
	else
		spec_factor = tex2D( Sampler1, Tex0 ).a;

	float4 color;
	color.rgb = surface_color * diffuse + g_vAmbientColor + specular * spec_factor;
	color.a = surface_color.a;
	return color;
}


/// calc specularity from tex rgb color
/// TODO: use tex alpha for alpha blend calc
float4 PS_2HSPLs_Specular_XTS( float2 Tex0 : TEXCOORD0,
                              float4 ColorDiff : COLOR0,
                              float3 NormalWS : TEXCOORD1,
                              float3 PositionWS : TEXCOORD2,
                              float3 ToViewer : TEXCOORD3,
                              uniform int spec_factor_type = USE_COLOR_TEXTURE_ALPHA_FOR_SPECULAR_MAP
                             ) : COLOR0
{
	float4 tex_color =  tex2D( Sampler0, Tex0 );
	float4 surface_color = float4( tex_color.rgb, 1 ) * ColorDiff;

	const float3 Normal = normalize(NormalWS); // normal in world space
	float3 ViewDir = normalize(ToViewer);      // direction to the camera in world space

	float4 sum_diffuse = 0;
	float sum_specular = 0;
	for( int i=0; i<2; i++ )
	{
		float4 diffuse;
		float specular;
		CalcSingleHSPointLightWithSpecular( Normal, ViewDir, PositionWS, diffuse, specular );

		sum_diffuse  += diffuse;
		sum_specular += specular;
	}

	float spec_factor;
	if( spec_factor_type == USE_COLOR_TEXTURE_ALPHA_FOR_SPECULAR_MAP )
		spec_factor = tex_color.a;
	else
		spec_factor = tex2D( Sampler1, Tex0 ).a;

	float4 color;
	color.rgb = surface_color * sum_diffuse + g_vAmbientColor + sum_specular * spec_factor;
	color.a = surface_color.a;
	return color;
}


/// calc specularity from tex rgb color
/// for environment mapping
/// TODO: use tex alpha for alpha blend calc
float4 PS_1HSPL_Specular_XTS_EM( float2 Tex0 : TEXCOORD0,
                                 float4 ColorDiff : COLOR0,
                                 float3 NormalWS : TEXCOORD1,
                                 float3 PositionWS : TEXCOORD2,
                                 float3 ToViewer : TEXCOORD3,
                                 float3 EnvTex : TEXCOORD4,
                                 uniform int spec_factor_type = USE_COLOR_TEXTURE_ALPHA_FOR_SPECULAR_MAP
                                ) : COLOR0
{
	float4 tex_color =  tex2D( Sampler0, Tex0 );
	float4 surface_color = float4( tex_color.rgb, 1 ) * ColorDiff;

	const float3 Normal = normalize(NormalWS); // normal in world space
	float3 ViewDir = normalize(ToViewer);      // direction to the camera in world space

	// calc diffuse and specular
	float4 diffuse;
	float specular;
	CalcSingleHSPointLightWithSpecular( Normal, ViewDir, PositionWS, diffuse, specular );

	float spec_factor;
	if( spec_factor_type == USE_COLOR_TEXTURE_ALPHA_FOR_SPECULAR_MAP )
		spec_factor = tex_color.a;
	else
		spec_factor = tex2D( Sampler1, Tex0 ).a;

	float4 color;
	color.rgb = surface_color * diffuse + g_vAmbientColor + specular * spec_factor;
	
	color.rgb = color.rgb + texCUBE( g_samCubeMap, EnvTex ).rgb * 0.2f;
//	color.rgb = texCUBE( g_samCubeMap, EnvTex ).rgb;
	
	color.a = surface_color.a;
	
	return color;
}


#endif /* __PS_SPECULAR_H__ */
