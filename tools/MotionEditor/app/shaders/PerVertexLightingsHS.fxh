#ifndef __PerVertexLightingsHS_H__
#define __PerVertexLightingsHS_H__


#include "ColorPair.fxh"
#include "LightDef.fxh"



/**
 \brief hemispheric directional light computation
 \param N normal in view space???
*/
//-----------------------------------------------------------------------------
COLOR_PAIR DoHemisphericDirLight(float3 N, float3 V, int i)
{
	COLOR_PAIR Out;
	float3 L = mul( -g_aLight[i].vDir, (float3x3)View );
//	float3 L = mul( -normalize(g_aLight[i].vDir), (float3x3)View );
	float NdotL = dot(N, L);
	float fUpper = ( NdotL + 1.0 ) / 2.0;
	Out.Color = g_aLight[i].vAmbient;
	Out.ColorSpec = 0;

	//compute diffuse color
	Out.Color += fUpper * g_aLight[i].vDiffuseUpper;
	Out.Color += (1 - fUpper) * g_aLight[i].vDiffuseLower;

	//add specular component
	if(g_bSpecular/* && NdotL >= 0.f*/)
	{
		float3 H = normalize(L + V);   // half vector
		Out.ColorSpec = pow(max(0, dot(H, N)), g_fMaterialPower) * g_aLight[i].vSpecular;
	}

	return Out;
}


/**
 \brief hemispheric point light computation
 \param N normal in view space???
*/
COLOR_PAIR DoHemisphericPointLight(float3 vWorldPos, float3 N, float3 V, int i)
{
	float3 vVertToLight = g_aLight[i].vPos - vWorldPos;
	float LD = length( vVertToLight );
	float3 L = mul( vVertToLight / LD, (float3x3)View ); // direction from vertex to point light in view space
//	float3 L = mul( normalize(g_aLight[i].vPos - vWorldPos), (float3x3)View );
	COLOR_PAIR Out;
	float NdotL = dot(N, L);
	float fUpper = ( NdotL + 1.0 ) / 2.0;
	Out.Color = g_aLight[i].vAmbient;
	Out.ColorSpec = 0;
	float fAtten = 1.f;

	// compute diffuse color
	Out.Color += fUpper * g_aLight[i].vDiffuseUpper;
	Out.Color += (1 - fUpper) * g_aLight[i].vDiffuseLower;

	// add specular component
	if(g_bSpecular /*&& NdotL >= 0.f*/)
	{
		float3 H = normalize(L + V);   // half vector
		Out.ColorSpec = pow(max(0, dot(H, N)), g_fMaterialPower) * g_aLight[i].vSpecular;
	}

//	float LD = length(g_aLight[i].vPos - vWorldPos);
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

//	Out.Color = float4( 0.0, 0.0, 0.5, 1.0 );

	return Out;
}


/// calculates one or more vertex hemispheric lights, either directional or point
/// \param ViewerVS [in] vector from vertex to viewer position in view space
COLOR_PAIR DoHemisphericVertexLights_Specular( float3 PosWS, float3 NormalVS, float3 ViewerVS )
{
	COLOR_PAIR Out;
	Out.Color = 0;
	Out.ColorSpec = 0;

	// directional lights
	for(int i = 0; i < iLightDirNum; i++)
	{
		COLOR_PAIR ColOut = DoHemisphericDirLight( NormalVS, ViewerVS, i+iLightDirIni );
		Out.Color += ColOut.Color;
		Out.ColorSpec += ColOut.ColorSpec;
	}

	// point lights
	for(int i = 0; i < iLightPointNum; i++)
	{
		COLOR_PAIR ColOut = DoHemisphericPointLight( float4(PosWS,1), NormalVS, ViewerVS, i+iLightPointIni );
		Out.Color += ColOut.Color;
		Out.ColorSpec += ColOut.ColorSpec;
	}
	
	return Out;
}


//-----------------------------------------------------------------------------
// Name: DoHemisphericDirLight()
// Desc: hemispheric directional light computation
//-----------------------------------------------------------------------------
/// Normal  normal in world space
float4 DoHemisphericDirLight_NoSpecular( float3 NormalWS, int i )
{
	COLOR_PAIR Out;
	float NdotL = dot(NormalWS, -g_aLight[i].vDir);
	float fUpper = ( NdotL + 1.0 ) / 2.0;
	float4 light_amount = g_aLight[i].vAmbient;

	//compute diffuse color
	light_amount += fUpper * g_aLight[i].vDiffuseUpper;
	light_amount += (1 - fUpper) * g_aLight[i].vDiffuseLower;

	return light_amount;
}


/**
 \brief hemispheric point light computation
 \param PosWS     vertex position in world space
 \param NormalWS  vertex normal in world space
 */
float4 DoHemisphericPointLight_NoSpecular( float3 PosWS, float3 NormalWS, int i )
{
	float3 vVertToLight = g_aLight[i].vPos - PosWS;
	float LD = length( vVertToLight );
	float3 L = vVertToLight / LD; // direction from vertex position to light in world space
	float NdotL = dot(NormalWS, L);
	float fUpper = ( NdotL + 1.0 ) / 2.0;
	float4 light_amount = g_aLight[i].vAmbient;
	float fAtten = 1.f;

	// compute diffuse color
	light_amount += fUpper * g_aLight[i].vDiffuseUpper;
	light_amount += (1 - fUpper) * g_aLight[i].vDiffuseLower;

//	float LD = length(g_aLight[i].vPos - vWorldPos);
	if(LD > g_aLight[i].fRange)
	{
		fAtten = 0.f;
	}
	else
	{
		fAtten *= 1.f/(g_aLight[i].vAttenuation.x + g_aLight[i].vAttenuation.y*LD + g_aLight[i].vAttenuation.z*LD*LD);
	}

	light_amount *= fAtten;

//	Out.Color = float4( 0.0, 0.0, 0.5, 1.0 );

	return light_amount;
}


float4 DoHemisphericVertexLights_NoSpecular( float3 PosWS, float3 NormalWS )
{
	float4 light_amount = 0;

	// directional lights
	for(int i = 0; i < iLightDirNum; i++)
	{
		light_amount += DoHemisphericDirLight_NoSpecular( NormalWS, i+iLightDirIni );
	}

	// point lights
	for(int i = 0; i < iLightPointNum; i++)
	{
		light_amount += DoHemisphericPointLight_NoSpecular( float4(PosWS,1), NormalWS, i+iLightPointIni );
	}

	return light_amount;
}



#endif /* __PerVertexLightingsHS_H__ */