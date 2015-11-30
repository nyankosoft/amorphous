#ifndef  __FreeTypeAux_H__
#define  __FreeTypeAux_H__


#include "array2d.hpp"
#include "Log/DefaultLog.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H


namespace amorphous
{

#ifdef _DEBUG
#pragma comment( lib, "freetype26MTd.lib" )
#else
#pragma comment( lib, "freetype26MT.lib" )
#endif


class FreeTypeLibrary
{
	FT_Library m_FTLibrary;

public:

	FreeTypeLibrary()
	{
		FT_Error error = FT_Init_FreeType( &m_FTLibrary );

		if( error != 0 )
		{
			LOG_PRINTF_ERROR(( "FT_Init_FreeType() returned an error: %d", (int)error ));
		}
	}

	~FreeTypeLibrary()
	{
		FT_Error error = FT_Done_FreeType( m_FTLibrary );

		if( error != 0 )
		{
			LOG_PRINTF_ERROR(( "FT_Done_FreeType() returned an error: %d", (int)error ));
		}
	}

	FT_Library GetFTLibrary() { return m_FTLibrary; }

	void LogLibraryVersion()
	{
		FT_Int major = 0, minor = 0, patch = 0;
		FT_Library_Version( m_FTLibrary, &major, &minor, &patch );

		LOG_PRINTF(( "FreeType version: %d.%d.%d", major, minor, patch ));
	}
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


inline void LogFreeTypeLibraryVersion()
{
	FreeTypeLibrary ft_library;
	ft_library.LogLibraryVersion();
}


template<typename BitmapCalc>
inline void DrawBitmap_Gen( FT_Bitmap *bitmap, int left, int top, array2d<U8>& dest_buffer )
{
	int x,y;//,r;
//	int index;
	int height = bitmap->rows;
	int width = bitmap->width;
	U8 *pBuffer = (U8 *)bitmap->buffer;
//	U8 grayscale = 0;
	for( y=0; y<height ; y++ )
	{
		for( x=0; x<width; x++ )
		{
			dest_buffer( left + x, top + y ) = BitmapCalc::CalcPixel( x, y, width, height, pBuffer );
		}
	}
}


inline void DrawBitmap( FT_Bitmap *bitmap, int left, int top, array2d<U8>& dest_buffer )
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
