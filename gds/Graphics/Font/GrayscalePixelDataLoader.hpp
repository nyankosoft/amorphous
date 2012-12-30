#ifndef  __GrayscaleImageLoader_HPP__
#define  __GrayscaleImageLoader_HPP__


#include "../TextureGenerators/TextureFillingAlgorithm.hpp"


namespace amorphous
{


class CGrayscaleImageLoader : public CTextureFillingAlgorithm
{
	const int m_Width;
	const int m_Height;
	const int m_StartRow;
	const int m_EndRow;
	const char **m_papPixelRow; ///< Pointer to the Array of char Pointers
	const int m_Levels;

public:

	CGrayscaleImageLoader( int width, int height, int start_row, int end_row, const char **papTexelRow, int levels = 16 )
		:
	m_Width(width),
	m_Height(height),
	m_StartRow(start_row),
	m_EndRow(end_row),
	m_papPixelRow(papTexelRow),
	m_Levels(levels)
	{}

	void FillTexture( CLockedTexture& texture )
	{
		// Clear the texture with transparent white
		texture.Clear( SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 0.0f ) );

		const int w = m_Width;
		const int h = m_Height;
		const int start_row = m_StartRow;
		const int end_row = m_EndRow;
		const char **papPixelRow = m_papPixelRow;
		const int scale = 256 / m_Levels;

		int row = 0;
		for( int y=start_row; y<end_row; y++, row++ )
		{
			for( int x=0; x<w; x++ )
			{
				U8 alpha = get_clamped( (papPixelRow[row][x] - '0')* scale, 0, 255 );
				U32 color = (alpha << 24) | 0x00FFFFFF;
				texture.SetPixelARGB32( x, y, color );
			}
		}
	}
};

} // namespace amorphous



#endif /* __GrayscaleImageLoader_HPP__ */
