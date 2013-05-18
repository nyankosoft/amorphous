#ifndef __GenericShaderDesc_HPP__
#define __GenericShaderDesc_HPP__


namespace amorphous
{

class ShaderLightingTechnique
{
public:
	enum Name
	{
		NORMAL,
		HEMISPHERIC,
		NUM_TYPES
	};
};


class ShaderLightingType
{
public:
	enum Name
	{
		PER_VERTEX,
		PER_PIXEL,
		NUM_TYPES
	};
};


class AlphaBlendType
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


class VertexBlendType
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

class SpecularSource
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

class EnvMapOption
{
public:
	enum Name
	{
		NONE,
		ENABLED,
		NUM_OPTIONS
	};
};

class PlanarReflectionOption
{
public:
	enum Name
	{
		NONE,
		FLAT,
		PERTURBED,
		NUM_OPTIONS
	};
};


class GenericShaderDesc
{
public:

	/// If set to true, lighting is applied based on the parameters set to the variables below
	/// If set to false, all the parameters related to lighting are ignored.
	/// Default: true
	bool Lighting;

	ShaderLightingTechnique::Name LightingTechnique;
	ShaderLightingType::Name LightingType; // per-vertex / per-pixel
	SpecularSource::Name Specular;
	VertexBlendType::Name VertexBlendType;
	AlphaBlendType::Name AlphaBlend;
	EnvMapOption::Name EnvMap;
	PlanarReflectionOption::Name PlanarReflection;
	int NormalMapTextureIndex;
	int NumPointLights;
	int NumDirectionalLights;
	int NumSpotLights;

public:

	GenericShaderDesc()
		:
	Lighting(true),
	LightingTechnique(ShaderLightingTechnique::HEMISPHERIC),
	LightingType(ShaderLightingType::PER_PIXEL),
	Specular(SpecularSource::NONE),
	VertexBlendType(VertexBlendType::NONE),
	AlphaBlend(AlphaBlendType::NONE),
	EnvMap(EnvMapOption::NONE),
	PlanarReflection(PlanarReflectionOption::NONE),
	NormalMapTextureIndex(-1),
	NumPointLights(-1),
	NumDirectionalLights(-1),
	NumSpotLights(-1)
	{}

	bool operator==( const GenericShaderDesc& rhs ) const
	{
		if( Lighting             == rhs.Lighting
		 && LightingTechnique    == rhs.LightingTechnique
		 && LightingType         == rhs.LightingType
		 && Specular             == rhs.Specular
		 && VertexBlendType      == rhs.VertexBlendType
		 && AlphaBlend           == rhs.AlphaBlend
		 && EnvMap               == rhs.EnvMap
		 && PlanarReflection     == rhs.PlanarReflection
		 && NumPointLights       == rhs.NumPointLights
		 && NumDirectionalLights == rhs.NumDirectionalLights
		 && NumSpotLights        == rhs.NumSpotLights )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

} // namespace amorphous


#endif  /* __GenericShaderDesc_HPP__ */
