#ifndef  __GenericShaderGenerator_HPP__
#define  __GenericShaderGenerator_HPP__


#include "GenericShaderDesc.hpp"
#include "ShaderGenerator.hpp"

#include "../Direct3D/Shader/Embedded/EmbeddedGenericHLSL.hpp"
#include "../Direct3D/Direct3D9.hpp"


namespace amorphous
{


class GenericShaderGenerator : public ShaderGenerator
{
	GenericShaderDesc m_Desc;

public:

	GenericShaderGenerator( const GenericShaderDesc& desc )
		:
	m_Desc(desc)
	{}

//	~GenericShaderGenerator() {}

	void GetShader( std::string& shader )
	{
		if( !DIRECT3D9.GetDevice() )
		{
			// Currently only available in HLSL.
			// TODO: Create a Cg version.
			return;
		}

		Result::Name res = EmbeddedGenericHLSL::GenerateShader( m_Desc, shader );
	}

//	void Accept( ShaderGeneratorVisitor& visitor ) { visitor.Visit( *this ); }

	bool IsSharableWith( const ShaderGenerator& other ) const
	{
		const GenericShaderGenerator *pOther = dynamic_cast<const GenericShaderGenerator *>(&other);
		if( pOther
		 && pOther->m_Desc == m_Desc )
		{
			return true;
		}
		else
			return false;
	}

	const GenericShaderDesc& GetGenericShaderDesc() const { return m_Desc; }
};


} // namespace amorphous



#endif		/*  __GenericShaderGenerator_HPP__  */
