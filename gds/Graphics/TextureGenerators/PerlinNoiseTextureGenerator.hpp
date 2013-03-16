#ifndef __PerlinNoiseTextureGenerator_HPP__
#define __PerlinNoiseTextureGenerator_HPP__


#include "TextureFillingAlgorithm.hpp"
#include "../../Utilities/PerlinAux.hpp"


namespace amorphous
{


class PerlinNoiseTextureGenerator : public TextureFillingAlgorithm
{
public:

	PerlinNoiseParams m_Params;

public:

	PerlinNoiseTextureGenerator() {}

	inline PerlinNoiseTextureGenerator( const PerlinNoiseParams& pn_params )
		:
	m_Params(pn_params)
	{}

	virtual ~PerlinNoiseTextureGenerator() {}

	virtual void FillTexture( LockedTexture& texture );

	virtual unsigned int GetArchiveObjectID() const { return TG_SINGLE_COLOR_TEXTURE_GENERATOR; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		TextureFillingAlgorithm::Serialize( ar, version );

		ar & m_Params;
	}
};


class PerlinNoiseNormalMapGenerator : public PerlinNoiseTextureGenerator
{
public:

	inline PerlinNoiseNormalMapGenerator( const PerlinNoiseParams& pn_params )
		:
	PerlinNoiseTextureGenerator(pn_params)
	{}

	virtual ~PerlinNoiseNormalMapGenerator() {}

	void FillTexture( LockedTexture& texture );

	unsigned int GetArchiveObjectID() const { return TG_SINGLE_COLOR_TEXTURE_GENERATOR; }
};


} // namespace amorphous



#endif /* __PerlinNoiseTextureGenerator_HPP__ */
