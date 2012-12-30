#ifndef  __ShaderGenerator_HPP__
#define  __ShaderGenerator_HPP__


#include <string>


namespace amorphous
{


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

	virtual bool IsSharableWith( const CShaderGenerator& other ) const { return false; }
};


} // namespace amorphous



#endif		/*  __ShaderGenerator_HPP__  */
