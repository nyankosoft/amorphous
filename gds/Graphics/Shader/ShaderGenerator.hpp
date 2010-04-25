#ifndef  __ShaderGenerator_HPP__
#define  __ShaderGenerator_HPP__


#include <string>


class CShaderGenerator
{

public:

	CShaderGenerator() {}

	virtual ~CShaderGenerator() {}

	/// HLSL effect file
	virtual void GetShader( std::string& shader ) {};

	/// vertex shader
	virtual void GetVertexShader( std::string& shader ) {};

	/// pixel(fragment) shader
	virtual void GetPixelShader( std::string& shader ) {};
};



#endif		/*  __ShaderGenerator_HPP__  */
