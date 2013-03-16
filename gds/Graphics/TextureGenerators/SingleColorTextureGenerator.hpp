#ifndef __SingleColorTextureGenerator_HPP__
#define __SingleColorTextureGenerator_HPP__


#include "TextureFillingAlgorithm.hpp"


namespace amorphous
{


class SingleColorTextureGenerator : public TextureFillingAlgorithm
{
	SFloatRGBAColor m_Color;

public:

	SingleColorTextureGenerator( const SFloatRGBAColor& color = SFloatRGBAColor::White() )
		:
	m_Color(color)
	{
	}

	void FillTexture( LockedTexture& texture )
	{
		texture.Clear( m_Color );
	}

	unsigned int GetArchiveObjectID() const { return TG_SINGLE_COLOR_TEXTURE_GENERATOR; }

	void Serialize( IArchive& ar, const unsigned int version )
	{
		TextureFillingAlgorithm::Serialize( ar, version );

		ar & m_Color;
	}
};

} // amorphous



#endif /* __SingleColorTextureGenerator_HPP__ */
