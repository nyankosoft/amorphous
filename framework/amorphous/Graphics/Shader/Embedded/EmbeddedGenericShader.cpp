#include "EmbeddedGenericShader.hpp"
//#include "EmbeddedMiscShader.hpp"
#include "../Generic2DShaderDesc.hpp"


namespace amorphous
{


static boost::shared_ptr<EmbeddedGenericShader> sg_pEmbeddedGenericShader;

//static boost::shared_ptr<EmbeddedMiscShader> sg_pEmbeddedGenericShader;


boost::shared_ptr<EmbeddedGenericShader> GetEmbeddedGenericShader()
{
	return sg_pEmbeddedGenericShader;
}


void SetEmbeddedGenericShader( boost::shared_ptr<EmbeddedGenericShader> ptr )
{
	sg_pEmbeddedGenericShader = ptr;
}

/*
boost::shared_ptr<MiscEmbeddedShader> GetMiscEmbeddedShader()
{
	return sg_pMiscEmbeddedShader;
}


void SetEmbeddedGenericShader( boost::shared_ptr<MiscEmbeddedShader> ptr )
{
	sg_pMiscEmbeddedShader = ptr;
}
*/

void EmbeddedGenericShader::AppendBlendCalculations(
	const Generic2DShaderDesc& desc,
	const std::string& channels,
//	const rgba_blend_operation& blend_ops,
	const blend_op& dc_and_tex0_blend,
	const blend_op& tex0_and_tex1_blend,
	const blend_op& tex1_and_tex2_blend,
	const blend_op& tex2_and_tex3_blend,
	std::string& blend
	)
{
//		if( m_Desc.diffuse_color_and_tex0.rgb.is_valid() )
	if( dc_and_tex0_blend.is_valid() )
	{
		if( desc.textures[0].is_valid() )
		{
			// blend dc rgb & tex0 rgb
//				blend += "dc.rgb" + char_to_string(m_Desc.diffuse_color_and_tex0.rgb.op) + "tc0.rgb";
			blend += "dc." + channels + dc_and_tex0_blend.to_string() + "tc0." + channels;
		}
	}
	else
	{
		if( desc.diffuse_color_rgb )
		{
			// diffuse rgb only
//			blend += "dc.rgb";
			blend += "dc." + channels;
			return;
		}
		else
		{
			// no diffuse rgb
			if( desc.textures[0].is_valid() )
//				blend += "tc0.rgb";
				blend += "tc0." + channels;
			else
				return;
		}
	}

	if( tex0_and_tex1_blend.is_valid() )
	{
		if( desc.textures[1].is_valid() )
		{
			blend += tex0_and_tex1_blend.to_string() + "tc1." + channels;
		}
	}

	if( tex1_and_tex2_blend.is_valid() )
	{
		if( desc.textures[2].is_valid() )
		{
			blend += tex1_and_tex2_blend.to_string() + "tc2." + channels;
		}
	}

	if( tex2_and_tex3_blend.is_valid() )
	{
		if( desc.textures[3].is_valid() )
		{
			blend += tex2_and_tex3_blend.to_string() + "tc3." + channels;
		}
	}
}


} // namespace amorphous
