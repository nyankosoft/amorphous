#ifndef  __GenericShaderGenerator_HPP__
#define  __GenericShaderGenerator_HPP__


#include "GenericShaderDesc.hpp"
#include "ShaderGenerator.hpp"

#include "../Direct3D/Shader/Embedded/EmbeddedGenericHLSL.hpp"


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

//	void Accept( CShaderGeneratorVisitor& visitor ) { visitor.Visit( *this ); }

	bool IsSharableWith( const CShaderGenerator& other ) const
	{
		const CGenericShaderGenerator *pOther = dynamic_cast<const CGenericShaderGenerator *>(&other);
		if( pOther
		 && pOther->m_Desc == m_Desc )
		{
			return true;
		}
		else
			return false;
	}
};



#endif		/*  __GenericShaderGenerator_HPP__  */
