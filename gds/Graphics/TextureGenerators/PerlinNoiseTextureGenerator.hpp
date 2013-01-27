#ifndef __PerlinNoiseTextureGenerator_HPP__
#define __PerlinNoiseTextureGenerator_HPP__


#include "TextureFillingAlgorithm.hpp"
#include "../../Utilities/PerlinAux.hpp"


namespace amorphous
{


class PerlinNoiseTextureGenerator : public TextureFillingAlgorithm
{
public:

	CPerlinNoiseParams m_Params;

public:

	inline PerlinNoiseTextureGenerator( const CPerlinNoiseParams& pn_params )
		:
	m_Params(pn_params)
	{}

	virtual ~PerlinNoiseTextureGenerator() {}

	virtual void FillTexture( LockedTexture& texture );
};


class PerlinNoiseNormalMapGenerator : public PerlinNoiseTextureGenerator
{
public:

	inline PerlinNoiseNormalMapGenerator( const CPerlinNoiseParams& pn_params )
		:
	PerlinNoiseTextureGenerator(pn_params)
	{}

	virtual ~PerlinNoiseNormalMapGenerator() {}

	void FillTexture( LockedTexture& texture );
};


} // namespace amorphous



#endif /* __PerlinNoiseTextureGenerator_HPP__ */
