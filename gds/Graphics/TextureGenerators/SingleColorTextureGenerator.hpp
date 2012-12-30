#ifndef __SingleColorTextureGenerator_HPP__
#define __SingleColorTextureGenerator_HPP__


#include "TextureFillingAlgorithm.hpp"


namespace amorphous
{


class CSingleColorTextureGenerator : public CTextureFillingAlgorithm
{
	SFloatRGBAColor m_Color;

public:

	CSingleColorTextureGenerator( const SFloatRGBAColor& color = SFloatRGBAColor::White() )
		:
	m_Color(color)
	{
	}

	void FillTexture( CLockedTexture& texture )
	{
		texture.Clear( m_Color );
	}
};

} // amorphous



#endif /* __SingleColorTextureGenerator_HPP__ */
