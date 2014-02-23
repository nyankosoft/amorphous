#ifndef __SamplingParameter_HPP__
#define __SamplingParameter_HPP__


class TextureAddressMode
{
public:
	enum Name
	{
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER,
		MIRROR_ONCE,
		NUM_TEXTURE_ADDRESSING_MODES
	};
};


class TextureFilter
{
public:
	enum Name
	{
		NEAREST,
		LINEAR,
		NUM_TEXTURE_FILTERS
	};
};


class SamplingParameter
{
public:
	enum Name
	{
		TEXTURE_WRAP_AXIS_0, ///< U in Direct3D, or S in OpenGL
		TEXTURE_WRAP_AXIS_1, ///< V in Direct3D, or T in OpenGL
		TEXTURE_WRAP_AXIS_2,
		MIN_FILTER,
		MAG_FILTER,
		NUM_SAMPLER_PARAMETERS
	};
};



#endif /* __SamplingParameter_HPP__ */
