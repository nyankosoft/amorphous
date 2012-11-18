#ifndef __GridTextureGenerator_HPP__
#define __GridTextureGenerator_HPP__


#include "TextureFillingAlgorithm.hpp"

class CGridTextureGenerator : public CTextureFillingAlgorithm
{
public:

	SFloatRGBAColor m_BaseColor;
	SFloatRGBAColor m_LineColor;

	unsigned int m_NumRows;
	unsigned int m_NumColumns;

	unsigned int m_LineWidth;

	bool m_DoubleLineWidthAtTextureBorders;

public:

	CGridTextureGenerator()
		:
	m_BaseColor( SFloatRGBAColor::White() ),
	m_LineColor( SFloatRGBAColor::Black() ),
	m_NumRows(8),
	m_NumColumns(8),
	m_LineWidth(2),
	m_DoubleLineWidthAtTextureBorders( false )
	{}

	void DrawHorizontalLine( int y, CLockedTexture& texture )
	{
		const SFloatRGBAColor line_color = m_LineColor;
		const int w = texture.GetWidth();
		for( int x=0; x<w; x++ )
			texture.SetPixelARGB32( x, y, line_color.GetARGB32() );

	}

	void DrawVerticalLine( int x, CLockedTexture& texture )
	{
		const SFloatRGBAColor line_color = m_LineColor;
		const int h = texture.GetHeight();
		for( int y=0; y<h; y++ )
			texture.SetPixelARGB32( x, y, line_color.GetARGB32() );

	}

	void FillTexture( CLockedTexture& texture )
	{
		const SFloatRGBAColor base_color = m_BaseColor;
		const SFloatRGBAColor line_color = m_LineColor;

		// Fill the texture with the base color
		texture.Clear( base_color );

		const int w = texture.GetWidth();
		const int h = texture.GetHeight();
		const int num_rows    = m_NumRows;
		const int num_columns = m_NumColumns;
		const unsigned int row_height   = h / num_rows;
		const unsigned int column_width = w / num_columns;

		const int line_width = m_LineWidth;
		const int line_width_at_tex_border
			= m_DoubleLineWidthAtTextureBorders ? line_width * 2 : line_width;

		// Draw the horizontal lines
		for( int y=0; y<line_width_at_tex_border/2; y++ )
			DrawHorizontalLine( y, texture );

		for( int i=1; i<num_rows-1; i++ )
		{
			for( int y=0; y<line_width; y++ )
				DrawHorizontalLine( i * row_height - line_width/2 + y, texture );
		}

		for( int y=0; y<line_width_at_tex_border/2; y++ )
			DrawHorizontalLine( h - 1 - y, texture );

		// Draw the vertical lines
		for( int x=0; x<line_width_at_tex_border/2; x++ )
			DrawVerticalLine( x, texture );

		for( int i=1; i<num_columns-1; i++ )
		{
			for( int x=0; x<line_width; x++ )
				DrawVerticalLine( i * column_width - line_width/2 + x, texture );
		}

		for( int x=0; x<line_width_at_tex_border/2; x++ )
			DrawVerticalLine( w - 1 - x, texture );
	}
};


//inline CTextureHandle CreateGridTexture(
//	int width,
//	int height,
//	int num_rows,
//	int num_column,
//	int line_width,
//	const SFloatRGBAColor& base_color,
//	const SFloatRGBAColor& line_color
//	)
//{
//	CTextureResourceDesc desc;
//	desc.Width   = width
//	desc.Height  = height
//
//	boost::shared_ptr<CGridTextureGenerator> pGenerator( new CGridTextureGenerator );
//	pGenerator->m_BaseColor    = base_color;
//	pGenerator->m_LineColor    = line_color;
//	pGenerator->m_LineWidth    = line_width;
//	pGenerator->m_NumRows      = num_rows;
//	pGenerator->m_NumColumns   = num_column;
//	desc.pLoader = pGenerator;
//
//	CTextureHandle tex;
//	bool loaded = tex.Load( desc );
//	if( !loaded )
//		LOG_PRINT_ERROR( " Failed to create a grid texture." );
//
//	return tex;
//}


#endif /* __GridTextureGenerator_HPP__ */
