#ifndef  __FreeTypeAux_H__
#define  __FreeTypeAux_H__


#include "2DArray.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H


namespace amorphous
{

#ifdef _DEBUG
#pragma comment( lib, "freetype244_D.lib" )
#else
#pragma comment( lib, "freetype244.lib" )
#endif


class CFreeTypeLibrary
{
	FT_Library m_FTLibrary;

public:

	CFreeTypeLibrary()
	{
		/* handle to face object */
		int error = FT_Init_FreeType( &m_FTLibrary );

		if ( error )
		{
		}
	}

	~CFreeTypeLibrary()
	{
		FT_Done_FreeType( m_FTLibrary );
	}

	FT_Library GetFTLibrary() { return m_FTLibrary; }
};


class MonoBitmapCalc
{
public:

	inline static U8 CalcPixel( int x, int y, int width, int height, U8 *pBuffer )
	{
		int r = x % 8;
		int index = x / 8;
		U8 grayscale = pBuffer[width*y + index] >> (7 - r);
		return grayscale;
	}
};


class GrayBitmapCalc
{
public:

	inline static U8 CalcPixel( int x, int y, int width, int height, U8 *pBuffer )
	{
		U8 grayscale = pBuffer[width*y + x];
		return grayscale;
	}
};


template<typename BitmapCalc>
inline void DrawBitmap_Gen( FT_Bitmap *bitmap, int left, int top, C2DArray<U8>& dest_buffer )
{
	int x,y;//,r;
//	int index;
	int height = bitmap->rows;
	int width = bitmap->width;
	U8 *pBuffer = (U8 *)bitmap->buffer;
	U8 grayscale = 0;
	for( y=0; y<height ; y++ )
	{
		for( x=0; x<width; x++ )
		{
			dest_buffer( left + x, top + y ) = BitmapCalc::CalcPixel( x, y, width, height, pBuffer );
		}
	}
}


inline void DrawBitmap( FT_Bitmap *bitmap, int left, int top, C2DArray<U8>& dest_buffer )
{
	switch( bitmap->pixel_mode )
	{
	case FT_PIXEL_MODE_MONO:
		DrawBitmap_Gen<MonoBitmapCalc>( bitmap, left, top, dest_buffer );
		break;
	case FT_PIXEL_MODE_GRAY:
		DrawBitmap_Gen<GrayBitmapCalc>( bitmap, left, top, dest_buffer );
		break;

	default:
		break;
	}
}






} // amorphous



#endif /* __FreeTypeAux_H__ */
