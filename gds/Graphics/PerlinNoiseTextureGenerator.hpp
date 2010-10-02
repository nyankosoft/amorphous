#ifndef __PerlinNoiseTextureGenerator_HPP__
#define __PerlinNoiseTextureGenerator_HPP__


#include "GraphicsResources.hpp"
#include "../Utilities/PerlinAux.hpp"


class CPerlinNoiseTextureGenerator : public CTextureFillingAlgorithm
{
public:

	CPerlinNoiseParams m_Params;

public:

	inline CPerlinNoiseTextureGenerator( const CPerlinNoiseParams& pn_params )
		:
	m_Params(pn_params)
	{}

	virtual ~CPerlinNoiseTextureGenerator() {}

	virtual void FillTexture( CLockedTexture& texture );
};


class CPerlinNoiseNormalMapGenerator : public CPerlinNoiseTextureGenerator
{
public:

	inline CPerlinNoiseNormalMapGenerator( const CPerlinNoiseParams& pn_params )
		:
	CPerlinNoiseTextureGenerator(pn_params)
	{}

	virtual ~CPerlinNoiseNormalMapGenerator() {}

	void FillTexture( CLockedTexture& texture );
};



#endif /* __PerlinNoiseTextureGenerator_HPP__ */
