#ifndef __ParticleTextureGenerator_HPP__
#define __ParticleTextureGenerator_HPP__


#include "TextureFillingAlgorithm.hpp"


namespace amorphous
{


/**
  The generated textures are designed to be rendered with pre-multiplied alpha.
 */
class ParticleTextureGenerator : public TextureFillingAlgorithm
{
public:

	SFloatRGBAColor m_Color;

	float m_fStandardDeviation;

public:

	ParticleTextureGenerator( const SFloatRGBAColor& color = SFloatRGBAColor::White() )
		:
	m_Color(color),
	m_fStandardDeviation(0.5f)
	{
	}

	void FillTexture( LockedTexture& texture );

	unsigned int GetArchiveObjectID() const { return TG_SINGLE_COLOR_TEXTURE_GENERATOR; }

	void Serialize( IArchive& ar, const unsigned int version )
	{
		TextureFillingAlgorithm::Serialize( ar, version );

		ar & m_Color;
		ar & m_fStandardDeviation;
	}
};


} // amorphous


#endif /* __ParticleTextureGenerator_HPP__ */
