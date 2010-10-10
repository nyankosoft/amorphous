#ifndef __GenericShaderDesc_HPP__
#define __GenericShaderDesc_HPP__


class CLightingTechnique
{
public:
	enum Name
	{
		NORMAL,
		HEMISPHERIC,
		NUM_TYPES
	};
};


class CShaderLightingType
{
public:
	enum Name
	{
		PER_VERTEX,
		PER_PIXEL,
		NUM_TYPES
	};
};


class CAlphaBlendType
{
public:
	enum Name
	{
		NONE,
//		UNIFORM,
		DIFFUSE_ALPHA,                         ///< use alpha of diffuse color
		DECAL_TEX_ALPHA,
//		NORMAL_MAP_ALPHA,
		MOD_DIFFUSE_ALPHA_AND_DECAL_TEX_ALPHA, ///< modulate diffuse and decal texture alpha
//		TEX0_ALPHA,
//		TEX1_ALPHA,
//		TEX2_ALPHA,
//		TEX3_ALPHA,
//		MOD_DIFFUSE_ALPHA_AND_TEX0_ALPHA,
//		MOD_DIFFUSE_ALPHA_AND_TEX1_ALPHA,
//		MOD_DIFFUSE_ALPHA_AND_TEX0_ALPHA_AND_TEX1_ALPHA,
		NUM_ALPHA_BLEND_TYPES
	};
};


class CVertexBlendType
{
public:
	enum Name
	{
		NONE,
		QUATERNION_AND_VECTOR3, ///< 3DMath/Transform class
		MATRIX,                 ///< 3DMath/Matrix34 class
		NUM_TYPES
	};
};

class CSpecularSource
{
public:
	enum Name
	{
		NONE,
		UNIFORM,                       ///< uniform specularity
		TEX0_ALPHA,
		DECAL_TEX_ALPHA = TEX0_ALPHA,  ///< alpha channel of decal texture (stage 0)
		TEX1_RED,
		TEX1_GREEN,
		TEX1_BLUE,
		TEX1_ALPHA,
		NORMAL_MAP_ALPHA = TEX1_ALPHA, ///< alpha channel of normal map texture (stage 1)
		NUM_TYPES
	};
};


class CGenericShaderDesc
{
public:

	CLightingTechnique::Name LightingTechnique;
	CShaderLightingType::Name ShaderLightingType; // per-vertex / per-pixel
	CSpecularSource::Name Specular;
	CVertexBlendType::Name VertexBlendType;
	CAlphaBlendType::Name AlphaBlend;
//	bool env_map;
	int NumPointLights;
	int NumDirectionalLights;
	int NumSpotLights;

public:

	CGenericShaderDesc()
		:
	LightingTechnique(CLightingTechnique::HEMISPHERIC),
	ShaderLightingType(CShaderLightingType::PER_PIXEL),
	Specular(CSpecularSource::NONE),
	VertexBlendType(CVertexBlendType::NONE),
	AlphaBlend(CAlphaBlendType::NONE),
//	env_map(false),
	NumPointLights(-1),
	NumDirectionalLights(-1),
	NumSpotLights(-1)
	{}
};


#endif  /* __GenericShaderDesc_HPP__ */
