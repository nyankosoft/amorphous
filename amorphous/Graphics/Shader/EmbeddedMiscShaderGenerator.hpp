#ifndef  __EmbeddedMiscShaderGenerator_HPP__
#define  __EmbeddedMiscShaderGenerator_HPP__


#include "ShaderGenerator.hpp"

#include "../Direct3D/Shader/Embedded/EmbeddedMiscHLSL.hpp"


namespace amorphous
{


class EmbeddedMiscShaderGenerator : public ShaderGenerator
{
	EmbeddedMiscShader::ID m_ShaderID;

public:

	EmbeddedMiscShaderGenerator( EmbeddedMiscShader::ID shader_id )
		:
	m_ShaderID(shader_id)
	{}

	void GetShader( std::string& shader )
	{
		Result::Name res = EmbeddedMiscHLSL::GetShader( m_ShaderID, shader );
	}
};


} // amorphous



#endif		/*  __EmbeddedMiscShaderGenerator_HPP__  */
