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


class CGenericShaderDesc
{
public:

	CLightingTechnique::Name LightingTechnique;
	CShaderLightingType::Name ShaderLightingType; // per-vertex / per-pixel
	bool Specular;
	CVertexBlendType::Name VertexBlendType;
//	bool env_map;
	int NumPointLights;
	int NumDirectionalLights;
	int NumSpotLights;

public:

	CGenericShaderDesc()
		:
	LightingTechnique(CLightingTechnique::HEMISPHERIC),
	ShaderLightingType(CShaderLightingType::PER_VERTEX),
	Specular(false),
	VertexBlendType(CVertexBlendType::NONE),
//	env_map(false),
	NumPointLights(-1),
	NumDirectionalLights(-1),
	NumSpotLights(-1)
	{}
};


#endif  /* __GenericShaderDesc_HPP__ */
