
// in order to use these functions, the following variable must be declared in advance
// World : a 4x4 world matrix
// View : a 4x4 view matrix
// g_aLight : an array of CLight objects
// bool g_bSpecular : a boolean variable that indicates whether the specular lighting is enabled or not
// float g_fMaterialPower


#include "ColorPair.fxh"


//-----------------------------------------------------------------------------
// Name: DoDirLight()
// Desc: Directional light computation
//-----------------------------------------------------------------------------
COLOR_PAIR DoDirLight(float3 N, float3 V, int i)
{
	COLOR_PAIR Out;
	float3 L = mul( -g_aLight[i].vDir, (float3x3)View );
//	float3 L = mul( -normalize(g_aLight[i].vDir), (float3x3)View );
	float NdotL = dot(N, L);
	Out.Color = g_aLight[i].vAmbient;
	Out.ColorSpec = 0;
	if(NdotL > 0.f)
	{
		//compute diffuse color
		Out.Color += NdotL * g_aLight[i].vDiffuseUpper;

		//add specular component
		if(g_bSpecular)
		{
			float3 H = normalize(L + V);   // half vector
			Out.ColorSpec = pow(max(0, dot(H, N)), g_fMaterialPower) * g_aLight[i].vSpecular;
		}
	}
	return Out;
}


//-----------------------------------------------------------------------------
// Name: DoPointLight()
// Desc: Point light computation
//-----------------------------------------------------------------------------
COLOR_PAIR DoPointLight(float4 vWorldPos, float3 N, float3 V, int i)
{
	float3 vVertToLight = g_aLight[i].vPos - vWorldPos;
	float LD = length( vVertToLight );
	float3 L = mul( vVertToLight / LD, (float3x3)View );
//	float3 L = mul( normalize(), (float3x3)View );

	COLOR_PAIR Out;
	float NdotL = dot(N, L);
	Out.Color = g_aLight[i].vAmbient;
	Out.ColorSpec = 0;
	float fAtten = 1.f;
	if(NdotL >= 0.f)
	{
		// compute diffuse color
		Out.Color += NdotL * g_aLight[i].vDiffuseUpper;

		// add specular component
		if(g_bSpecular)
		{
			float3 H = normalize(L + V);   // half vector
			Out.ColorSpec = pow(max(0, dot(H, N)), g_fMaterialPower) * g_aLight[i].vSpecular;
		}

//		float LD = length(g_aLight[i].vPos-(float3)mul( vPosition, World ));
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

