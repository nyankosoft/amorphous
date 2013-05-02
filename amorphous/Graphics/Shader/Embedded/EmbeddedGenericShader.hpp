#ifndef __EmbeddedGenericShader_HPP__
#define __EmbeddedGenericShader_HPP__


#include <string>
#include <boost/shared_ptr.hpp>
#include "amorphous/base.hpp"
#include "amorphous/Graphics/Shader/GenericShaderDesc.hpp"


namespace amorphous
{


class EmbeddedGenericShader
{
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
};


boost::shared_ptr<EmbeddedGenericShader> GetEmbeddedGenericShader();

void SetEmbeddedGenericShader( boost::shared_ptr<EmbeddedGenericShader> ptr );


} // namespace amorphous


#endif /* __EmbeddedGenericShader_HPP__ */
