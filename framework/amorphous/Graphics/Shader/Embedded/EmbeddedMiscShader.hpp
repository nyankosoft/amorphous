#ifndef __EmbeddedMiscShader_HPP__
#define __EmbeddedMiscShader_HPP__


#include <string>
#include <memory>
#include "amorphous/base.hpp"
#include "amorphous/Graphics/fwd.hpp"


namespace amorphous
{


class EmbeddedMiscShader
{
public:

	EmbeddedMiscShader() {}

	virtual ~EmbeddedMiscShader(){}

	// Derived classes must implement either
	// 1. GenerateShader()
	// or
	// 2. GenerateVertexShader() & GenerateFragmentShader()

	virtual Result::Name GenerateShader( const GenericShaderDesc& desc, std::string& shader ) { return Result::UNKNOWN_ERROR; }

	virtual Result::Name GenerateVertexShader( const GenericShaderDesc& desc, std::string& shader ) { return Result::UNKNOWN_ERROR; }

	virtual Result::Name GenerateFragmentShader( const GenericShaderDesc& desc, std::string& shader ) { return Result::UNKNOWN_ERROR; }
};


} // namespace amorphous


#endif /* __EmbeddedMiscShader_HPP__ */
