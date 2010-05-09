#ifndef  __NoiseTextureGenerators_HPP__
#define  __NoiseTextureGenerators_HPP__


#include "fwd.hpp"
#include "GraphicsResources.hpp"
#include "GraphicsResourceDescs.hpp"
#include "Support/MTRand.hpp"


class CUniformNoiseTextureGenerator : public CTextureFillingAlgorithm
{
public:

	float m_fDensity;

	float m_fMin;
	float m_fMax;

public:

	inline CUniformNoiseTextureGenerator()
		:
	m_fDensity(0),
	m_fMin(0),
	m_fMax(0)
	{}

	virtual ~CUniformNoiseTextureGenerator() {}
};



class CUniformSingleColorNoiseTextureGenerator : public CUniformNoiseTextureGenerator
{
public:

	SFloatRGBAColor m_Color;

	CUniformSingleColorNoiseTextureGenerator()
		:
	m_Color( SFloatRGBAColor::Black() )
	{}

	~CUniformSingleColorNoiseTextureGenerator() {}

	void FillTexture( CLockedTexture& texture );
};


/**
- No separate width for color 0 and 1
  - rationale: easier to keep the texture size to 2^n
- Calculates the texture size from CStripeTextureGenerator::m_Width
  - texture width/height: m_Width*2
  - m_Width should be 2^n
*/
class CStripeTextureGenerator : public CTextureFillingAlgorithm
{
public:
/*	enum Directions
	{
		DIR_HORIZONTAL,
		DIR_VERTICAL,
		NUM_DIRECTIONS
	};*/

	SFloatRGBAColor m_Color0;
	SFloatRGBAColor m_Color1;
	uint m_StripeWidth;

	CStripeTextureGenerator()
		:
	m_Color0( SFloatRGBAColor(0.0f,0.0f,0.0f,0.5f) ),
	m_Color1( SFloatRGBAColor(0.0f,0.0f,0.0f,0.0f) ),
	m_StripeWidth(4)
	{}

	void FillTexture( CLockedTexture& texture );
};


#endif  /* __NoiseTextureGenerators_HPP__ */
