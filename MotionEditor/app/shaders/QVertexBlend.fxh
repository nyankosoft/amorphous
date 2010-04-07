// TODO:
// - Convert "VS_VertBlend_1DL" to "VS_VertBlend_1HSDL"

// shader for vertex blend with not bump maps

// --- global variables ---
// matrices:
//  float4x4 World
//  float4x4 View
//  float4x4 Proj
// textures:
//  Sampler0
// variables:
//  g_vEyePos
//  CLight g_aLight[NUM_LIGHTS]
//  iLightDirIni
// constants:
//  NUM_MAX_BLEND_TRANSFORMS
//  NUM_MAX_BLEND_TRANSFORMS_PER_VERTEX


struct Transform
{
	float4 translation;
	float4 rotation; ///< quaternion
};


float length_sq( float4 src )
{
	return src.x*src.x + src.y*src.y + src.z*src.z + src.w*src.w;
}


float4 quat_inv( float4 src )
{
//	Scalar fNorm = (Scalar)0.0;

	float fNormSq = length_sq(src);

	float4 inv;
//	if ( fNormSq > (Scalar)0.0 )
//	{
		float fInvNormSq = 1.0 / fNormSq;
		inv.x = -src.x*fInvNormSq;
		inv.y = -src.y*fInvNormSq;
		inv.z = -src.z*fInvNormSq;
		inv.w =  src.w*fInvNormSq;
/*	}
	else
	{
		// return an invalid result to flag the error
		inv.x = (Scalar)0.0;
		inv.y = (Scalar)0.0;
		inv.z = (Scalar)0.0;
		inv.w = (Scalar)0.0;
	}*/

	// Replace to this after cofirming the above code works
//	return float4( -src.x, -src.y, -src.z, src.w ) / fNormSq;

	return inv;
}


float4 quat_mul( float4 lhs, float4 rhs )
{
    float4 res;

    res.w =
        lhs.w * rhs.w -
        lhs.x * rhs.x -
        lhs.y * rhs.y -
        lhs.z * rhs.z;

    res.x =
        lhs.w * rhs.x +
        lhs.x * rhs.w +
        lhs.y * rhs.z -
        lhs.z * rhs.y;

    res.y =
        lhs.w * rhs.y +
        lhs.y * rhs.w +
        lhs.z * rhs.x -
        lhs.x * rhs.z;

    res.z =
        lhs.w * rhs.z +
        lhs.z * rhs.w +
        lhs.x * rhs.y -
        lhs.y * rhs.x;

    return res;
}


float3 mul( Transform transform, float3 rhs )
{
	float4 res = quat_mul( quat_mul( transform.rotation, float4( rhs.x, rhs.y, rhs.z, 0 ) ), quat_inv( transform.rotation ) );
	return res.xyz + transform.translation.xyz / transform.translation.w;
}


float4 mul( Transform transform, float4 rhs )
{
	return float4( mul( transform, rhs.xyz / rhs.w ), 1 );
}


#include "PerVertexLightingsHS.fxh"
#include "QVertexBlendFunctions.fxh"
#include "PS_Specular.fxh"




Transform g_aBlendTransform[NUM_MAX_BLEND_TRANSFORMS];


struct VSIN_BLEND
{
	float4 position       :POSITION;
	float4 diffuse        :COLOR0;
	float3 normal         :NORMAL;
	float2 vTex0          :TEXCOORD0;
	int4   indices        :BLENDINDICES;
	float4 weights        :BLENDWEIGHT;
};


struct VSOUT_BLEND
{
	float4 position       :POSITION;
	float4 diffuse        :COLOR0;
	float2 vTex0          :TEXCOORD0;
	float3 NormalWS       :TEXCOORD1;
	float3 ToViewerWS     :TEXCOORD3;
	float3 positionWS     :TEXCOORD5;
};


//=============================================================================


VSOUT_BLEND VS_VertBlend_1HSDL( VSIN_BLEND In, uniform int operation=0 )
{
	VSOUT_BLEND Out = (VSOUT_BLEND) 0; 
//	float4 vWorldPos = In.position;
	float4 vModelSpacePos = In.position;
	float3 normal = In.normal;
	float4 diffuse = In.diffuse;

	SkinPoint( vModelSpacePos, g_aBlendTransform, In.indices, In.weights); // pos is transformed to world space.
	SkinVector(      normal,   g_aBlendTransform, In.indices, In.weights); // normal is transformed to world space.

//	vWorldPos = mul( In.position, g_aBlendTransform[0] );

	float4 vWorldPos = mul( vModelSpacePos, World );
//	NormalWS  = mul( normal, (float3x3)World );

//	float4 vPosVS = mul( vWorldPos, View );
//	Out.position = mul( vPosVS, Proj );
	Out.position = mul( vModelSpacePos, WorldViewProj );

	float3 vViewerVS = mul( float4(g_vEyePos,1) - vWorldPos, View );	// V

	float3 vNormalVS = mul( normal, (float3x3)WorldView );

	// apply vertex lighting
	COLOR_PAIR ColOut;
	ColOut = DoHemisphericDirLight(vNormalVS, vViewerVS, iLightDirIni + 0);
	diffuse *= ColOut.Color;
	
	Out.diffuse = diffuse;

	Out.NormalWS = normal;
	Out.vTex0    = In.vTex0;

	// debug
//	Out.diffuse = float4( -g_aLight[0].vDir, 1 );
//	Out.diffuse = float4( vNormalVS, 1.0f );

	return Out;
}


VSOUT_BLEND VS_VertBlend_HSLs( VSIN_BLEND In, uniform int operation=0 )
{
	VSOUT_BLEND Out = (VSOUT_BLEND) 0; 
//	float4 vWorldPos = In.position;
	float4 vModelSpacePos = In.position;
	float3 normal = In.normal;
	float4 diffuse = In.diffuse;

	SkinPoint( vModelSpacePos, g_aBlendTransform, In.indices, In.weights); // pos is transformed to world space.
	SkinVector( normal,        g_aBlendTransform, In.indices, In.weights); // normal is transformed to world space.

	float4 vWorldPos = mul( vModelSpacePos, World );
//	NormalWS  = mul( normal, (float3x3)World );

//	float4 vPosVS = mul( vWorldPos, View );
//	Out.position = mul( vPosVS, Proj );
	Out.position = mul( vModelSpacePos, WorldViewProj );

	float3 vViewerVS = mul( float4(g_vEyePos,1) - vWorldPos, View );	// V

	float3 vNormalVS = mul( normal, (float3x3)WorldView );

	// apply vertex lighting
	COLOR_PAIR ColOut;
	ColOut = DoHemisphericVertexLights_Specular( vWorldPos, vNormalVS, vViewerVS );
	diffuse *= ColOut.Color;
	
	Out.diffuse = diffuse;

	Out.NormalWS   = normal;
	Out.vTex0    = In.vTex0;

	return Out;
}


float4 PS_VertexBlend( float2 Tex0 : TEXCOORD0, float4 ColorDiff : COLOR0 ) : COLOR0
{
//	return ColorDiff;
	return tex2D( Sampler0, Tex0 ) * ColorDiff;
}


/**
 Calculates viewer position and world normal for specular calc in pixel shader
 - No lighting is done in this function.
 - Used for per-pixel lighting.
 */
VSOUT_BLEND VS_VertBlend( VSIN_BLEND In, uniform int operation=0 )
{
	VSOUT_BLEND Out = (VSOUT_BLEND) 0; 
//	float4 vWorldPos = In.position;
	float4 vModelSpacePos = In.position;
	float3 normal = In.normal;

	SkinPoint( vModelSpacePos, g_aBlendTransform, In.indices, In.weights); // pos is transformed to model space?
	SkinVector( normal,        g_aBlendTransform, In.indices, In.weights); // normal is transformed to model space?

	float4 vWorldPos = mul( vModelSpacePos, World );
	Out.NormalWS  = mul( normal, (float3x3)World );
	
//	float4 vPosVS = mul( vWorldPos, View );
//	Out.position = mul( vPosVS, Proj );
	Out.position = mul( vModelSpacePos, WorldViewProj );

//	float3 vNormalVS = mul( normal, (float3x3)WorldView );
//	float3 vViewerVS = mul( float4(g_vEyePos,1) - vWorldPos, View );	// V

    Out.ToViewerWS = g_vEyePos - vWorldPos;
	Out.diffuse  = In.diffuse;
	Out.vTex0    = In.vTex0;

	return Out;
}


/**
- vertex blending with one per-vertex hemispheric directional light
*/
technique VertBlend_PVL_1HSDL
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_VertBlend_1HSDL();
		PixelShader  = compile ps_2_0 PS_VertexBlend();

		CullMode = Ccw;
	}
}



/**
- vertex blending with one per-vertex hemispheric lights
*/
technique VertBlend_PVL_HSLs
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_VertBlend_HSLs();
		PixelShader  = compile ps_2_0 PS_VertexBlend();

		CullMode = Ccw;
	}
}

/*
technique VertBlend_1HSDL_Specular
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_VertBlend();
		PixelShader  = compile ps_2_0 PS_1HSDL_Specular();

		CullMode = Ccw;
	}
}
*/

technique VertBlend_1HSDL_Specular_CTS
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_VertBlend();
		PixelShader  = compile ps_2_0 PS_1HSDL_Specular_CTS();

		CullMode = Ccw;
		AlphaBlendEnable = True;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}


technique VertBlend_1HSDL_Specular_CT2S
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_VertBlend();
		PixelShader  = compile ps_2_0 PS_1HSDL_Specular_CT2S();

		CullMode = Ccw;
		AlphaBlendEnable = True;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}

