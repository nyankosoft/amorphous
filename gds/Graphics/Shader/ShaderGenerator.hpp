#ifndef  __ShaderGenerator_HPP__
#define  __ShaderGenerator_HPP__


#include <string>


namespace amorphous
{


class ShaderGenerator
{

public:

	ShaderGenerator() {}

	virtual ~ShaderGenerator() {}

	/// HLSL effect file
	virtual void GetShader( std::string& shader ) {};

	/// vertex shader
	virtual void GetVertexShader( std::string& shader ) {};

	/// pixel(fragment) shader
	virtual void GetPixelShader( std::string& shader ) {};

	virtual bool IsSharableWith( const ShaderGenerator& other ) const { return false; }
};


} // namespace amorphous



#endif		/*  __ShaderGenerator_HPP__  */
