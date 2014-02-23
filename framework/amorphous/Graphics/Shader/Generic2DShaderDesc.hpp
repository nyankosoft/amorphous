#ifndef __Generic2DShaderDesc_HPP__
#define __Generic2DShaderDesc_HPP__


#include <string>


namespace amorphous
{


class texture_sample_params
{
public:
	int sampler;
	int coord;

	texture_sample_params() : sampler(-1), coord(-1) {}

	bool is_valid() const { return (0 <= sampler && 0 <= coord); }

};


class blend_op
{
public:

	char op;

public:

	blend_op() : op(0) {}

	bool is_valid() const { return (op == '*' || op == '+' || op == '-'); }

	std::string to_string() const
	{
		std::string out = " ";
		out[0] = op;
		return out;
	}

	bool operator==( const blend_op& rhs ) const { return (op==rhs.op); }
};


class rgba_blend_operation
{
public:
	blend_op rgb;
	blend_op alpha;

	bool operator==( const rgba_blend_operation& rhs ) const { return (rgb==rhs.rgb && alpha==rhs.alpha); }
};


class Generic2DShaderDesc
{
public:

	bool diffuse_color_rgb;
	bool diffuse_color_alpha;

	texture_sample_params textures[8];

	rgba_blend_operation diffuse_color_and_tex0_blend;
	rgba_blend_operation tex0_and_tex1_blend;
	rgba_blend_operation tex1_and_tex2_blend;
	rgba_blend_operation tex2_and_tex3_blend;

public:

	Generic2DShaderDesc()
		:
	diffuse_color_rgb(true),
	diffuse_color_alpha(true)
	{}

	~Generic2DShaderDesc() {}

	bool operator==( const Generic2DShaderDesc& rhs ) const
	{
		if( diffuse_color_rgb            == rhs.diffuse_color_rgb
		 && diffuse_color_alpha          == rhs.diffuse_color_alpha
		 && diffuse_color_and_tex0_blend == rhs.diffuse_color_and_tex0_blend
		 && tex0_and_tex1_blend          == rhs.tex0_and_tex1_blend
		 && tex1_and_tex2_blend          == rhs.tex1_and_tex2_blend
		 && tex2_and_tex3_blend          == rhs.tex2_and_tex3_blend )
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


#endif /* __Generic2DShaderDesc_HPP__ */
