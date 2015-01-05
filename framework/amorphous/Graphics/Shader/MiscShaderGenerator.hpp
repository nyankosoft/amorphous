#ifndef __amorphous_MiscShaderGenerator_HPP__
#define __amorphous_MiscShaderGenerator_HPP__


#include "ShaderGenerator.hpp"
#include "MiscShader.hpp"
#include "Embedded/EmbeddedGenericShader.hpp"
#include "../ShaderHandle.hpp"


namespace amorphous
{


class MiscShaderGenerator : public ShaderGenerator
{
protected:

	MiscShader::ID m_ShaderID;

public:

	MiscShaderGenerator( MiscShader::ID shader_id )
		:
	m_ShaderID(shader_id)
	{}

	void GetShader( std::string& shader )
	{
		Result::Name res = Result::UNKNOWN_ERROR;

		if( GetEmbeddedGenericShader() )
			res = GetEmbeddedGenericShader()->GenerateMiscShader( m_ShaderID, shader );
	}

	void GetVertexShader( std::string& shader )
	{
		Result::Name res = Result::UNKNOWN_ERROR;

		if( GetEmbeddedGenericShader() )
			res = GetEmbeddedGenericShader()->GenerateMiscVertexShader( m_ShaderID, shader );
	}

	void GetPixelShader( std::string& shader )
	{
		Result::Name res = Result::UNKNOWN_ERROR;

		if( GetEmbeddedGenericShader() )
			res = GetEmbeddedGenericShader()->GenerateMiscFragmentShader( m_ShaderID, shader );
	}

//	void Accept( ShaderGeneratorVisitor& visitor ) { visitor.Visit( *this ); }

	bool IsSharableWith( const ShaderGenerator& other ) const
	{
		const MiscShaderGenerator *pOther = dynamic_cast<const MiscShaderGenerator *>(&other);
		if( pOther
		 && pOther->m_ShaderID == m_ShaderID )
		{
			return true;
		}
		else
			return false;
	}
};


inline ShaderHandle CreateMiscShader( MiscShader::ID misc_shader_id )
{
	ShaderResourceDesc desc;
	desc.pShaderGenerator.reset( new MiscShaderGenerator(misc_shader_id) );

	ShaderHandle shader;
	shader.Load( desc );

	return shader;
}


} // namespace amorphous


#endif /* __amorphous_MiscShaderGenerator_HPP__ */
