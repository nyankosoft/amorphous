#ifndef  __MiscEmbeddedShaderGenerator_HPP__
#define  __MiscEmbeddedShaderGenerator_HPP__


#include "MiscEmbeddedShaderGenerator.hpp"

#include "../Direct3D/Shader/Embedded/EmbeddedMiscHLSL.hpp"


namespace amorphous
{


class D3DMiscEmbeddedShaderGenerator : public MiscEmbeddedShaderGenerator
{

public:

	D3DMiscEmbeddedShaderGenerator( MiscEmbeddedShader::ID shader_id )
		:
	MiscEmbeddedShaderGenerator(shader_id)
	{}

	void GetShader( std::string& shader )
	{
		Result::Name res = EmbeddedMiscHLSL::GetShader( m_ShaderID, shader );
	}
};


} // amorphous



#endif		/*  __MiscEmbeddedShaderGenerator_HPP__  */
