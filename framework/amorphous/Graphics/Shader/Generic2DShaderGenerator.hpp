#ifndef  __Generic2DShaderGenerator_HPP__
#define  __Generic2DShaderGenerator_HPP__


#include "Generic2DShaderDesc.hpp"
#include "ShaderGenerator.hpp"
#include "Embedded/EmbeddedGenericShader.hpp"
#include "../ShaderHandle.hpp"


namespace amorphous
{


class Generic2DShaderGenerator : public ShaderGenerator
{
	Generic2DShaderDesc m_Desc;

public:

	Generic2DShaderGenerator( const Generic2DShaderDesc& desc )
		:
	m_Desc(desc)
	{}

//	~Generic2DShaderGenerator() {}

	void GetShader( std::string& shader )
	{
		Result::Name res = Result::UNKNOWN_ERROR;

		if( GetEmbeddedGenericShader() )
			res = GetEmbeddedGenericShader()->Generate2DShader( m_Desc, shader );
	}

	void GetVertexShader( std::string& shader )
	{
		Result::Name res = Result::UNKNOWN_ERROR;

		if( GetEmbeddedGenericShader() )
			res = GetEmbeddedGenericShader()->Generate2DVertexShader( m_Desc, shader );
	}

	void GetPixelShader( std::string& shader )
	{
		Result::Name res = Result::UNKNOWN_ERROR;

		if( GetEmbeddedGenericShader() )
			res = GetEmbeddedGenericShader()->Generate2DFragmentShader( m_Desc, shader );
	}

//	void Accept( ShaderGeneratorVisitor& visitor ) { visitor.Visit( *this ); }

	bool IsSharableWith( const ShaderGenerator& other ) const
	{
		const Generic2DShaderGenerator *pOther = dynamic_cast<const Generic2DShaderGenerator *>(&other);
		if( pOther
		 && pOther->m_Desc == m_Desc )
		{
			return true;
		}
		else
			return false;
	}

	const Generic2DShaderDesc& GetGeneric2DShaderDesc() const { return m_Desc; }
};


inline ShaderHandle CreateGeneric2DShader( const Generic2DShaderDesc& desc )
{
	ShaderResourceDesc sr_desc;
	sr_desc.pShaderGenerator.reset( new Generic2DShaderGenerator(desc) );
	ShaderHandle shader;
	bool loaded = shader.Load( sr_desc );
	if( !loaded )
		LOG_PRINT_ERROR( " Failed to create a 2D shader." );
	return shader;
}


} // namespace amorphous



#endif		/*  __Generic2DShaderGenerator_HPP__  */
