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
};

} // amorphous



#endif /* __SingleColorTextureGenerator_HPP__ */
