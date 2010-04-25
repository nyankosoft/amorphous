#ifndef  __GenericShaderGenerator_HPP__
#define  __GenericShaderGenerator_HPP__


#include "GenericShaderDesc.hpp"
#include "Graphics/Shader/ShaderGenerator.hpp"

#include "Graphics/Direct3D/Shader/Embedded/EmbeddedGenericHLSL.hpp"


class CGenericShaderGenerator : public CShaderGenerator
{
	CGenericShaderDesc m_Desc;

public:

	CGenericShaderGenerator( const CGenericShaderDesc& desc )
		:
	m_Desc(desc)
	{}

//	~CGenericShaderGenerator() {}

	void GetShader( std::string& shader )
	{
		Result::Name res = CEmbeddedGenericHLSL::GenerateShader( m_Desc, shader );
	}
};



#endif		/*  __GenericShaderGenerator_HPP__  */
