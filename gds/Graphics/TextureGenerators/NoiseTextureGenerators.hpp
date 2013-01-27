#ifndef  __NoiseTextureGenerators_HPP__
#define  __NoiseTextureGenerators_HPP__


#include "TextureFillingAlgorithm.hpp"
#include "../../Support/MTRand.hpp"


namespace amorphous
{


class UniformNoiseTextureGenerator : public TextureFillingAlgorithm
{
public:

	float m_fDensity;

	float m_fMin;
	float m_fMax;

public:

	inline UniformNoiseTextureGenerator()
		:
	m_fDensity(0),
	m_fMin(0),
	m_fMax(0)
	{}

	virtual ~UniformNoiseTextureGenerator() {}
};



class UniformSingleColorNoiseTextureGenerator : public UniformNoiseTextureGenerator
{
public:

	SFloatRGBAColor m_Color;

	UniformSingleColorNoiseTextureGenerator()
		:
	m_Color( SFloatRGBAColor::Black() )
	{}

	~UniformSingleColorNoiseTextureGenerator() {}

	void FillTexture( LockedTexture& texture );
};


/**
- No separate width for color 0 and 1
  - rationale: easier to keep the texture size to 2^n
- Calculates the texture size from StripeTextureGenerator::m_Width
  - texture width/height: m_Width*2
  - m_Width should be 2^n
*/
class StripeTextureGenerator : public TextureFillingAlgorithm
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

	StripeTextureGenerator()
		:
	m_Color0( SFloatRGBAColor(0.0f,0.0f,0.0f,0.5f) ),
	m_Color1( SFloatRGBAColor(0.0f,0.0f,0.0f,0.0f) ),
	m_StripeWidth(4)
	{}

	void FillTexture( LockedTexture& texture );
};

} // namespace amorphous



#endif  /* __NoiseTextureGenerators_HPP__ */
