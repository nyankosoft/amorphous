#ifndef  __GenericShaderGenerator_HPP__
#define  __GenericShaderGenerator_HPP__


#include "GenericShaderDesc.hpp"
#include "ShaderGenerator.hpp"

#include "../Direct3D/Shader/Embedded/EmbeddedGenericHLSL.hpp"


namespace amorphous
{


class CGenericShaderGenerator : public ShaderGenerator
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

//	void Accept( ShaderGeneratorVisitor& visitor ) { visitor.Visit( *this ); }

	bool IsSharableWith( const ShaderGenerator& other ) const
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

	const CGenericShaderDesc& GetGenericShaderDesc() const { return m_Desc; }
};


} // namespace amorphous



#endif		/*  __GenericShaderGenerator_HPP__  */
