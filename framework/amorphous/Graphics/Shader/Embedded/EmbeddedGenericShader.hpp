#ifndef __EmbeddedGenericShader_HPP__
#define __EmbeddedGenericShader_HPP__


#include <string>
#include <boost/shared_ptr.hpp>
#include "amorphous/base.hpp"
#include "amorphous/Graphics/fwd.hpp"


namespace amorphous
{

class blend_op;


class EmbeddedGenericShader
{
protected:

	void AppendBlendCalculations(
		const Generic2DShaderDesc& desc,
		const std::string& channels,
//		const rgba_blend_operation& blend_ops,
		const blend_op& dc_and_tex0_blend,
		const blend_op& tex0_and_tex1_blend,
		const blend_op& tex1_and_tex2_blend,
		const blend_op& tex2_and_tex3_blend,
		std::string& blend
		);

public:

	EmbeddedGenericShader() {}

	virtual ~EmbeddedGenericShader(){}

	// Derived classes must implement either
	// 1. GenerateShader()
	// or
	// 2. GenerateVertexShader() & GenerateFragmentShader()

	virtual Result::Name GenerateShader( const GenericShaderDesc& desc, std::string& shader ) { return Result::UNKNOWN_ERROR; }

	virtual Result::Name GenerateVertexShader( const GenericShaderDesc& desc, std::string& shader ) { return Result::UNKNOWN_ERROR; }

	virtual Result::Name GenerateFragmentShader( const GenericShaderDesc& desc, std::string& shader ) { return Result::UNKNOWN_ERROR; }

	virtual Result::Name Generate2DShader( const Generic2DShaderDesc& desc, std::string& shader ) { return Result::UNKNOWN_ERROR; }

	virtual Result::Name Generate2DVertexShader( const Generic2DShaderDesc& desc, std::string& shader ) { return Result::UNKNOWN_ERROR; }

	virtual Result::Name Generate2DFragmentShader( const Generic2DShaderDesc& desc, std::string& shader ) { return Result::UNKNOWN_ERROR; }
};


boost::shared_ptr<EmbeddedGenericShader> GetEmbeddedGenericShader();

void SetEmbeddedGenericShader( boost::shared_ptr<EmbeddedGenericShader> ptr );


} // namespace amorphous


#endif /* __EmbeddedGenericShader_HPP__ */
