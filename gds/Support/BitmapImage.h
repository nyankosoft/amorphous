#ifndef __BitmapImage_H__
#define __BitmapImage_H__


#include <string>
#include <boost/shared_ptr.hpp>
#include "FreeImage.h"

#include "3DCommon/FloatRGBColor.h"
#include "3DCommon/FloatRGBAColor.h"
#include "Support/ImageArchive.h"
#include "Support/2DArray.h"
#include "Support/Macro.h"
//#include "Support/Log/DefaultLog.h"

#pragma comment( lib, "FreeImage.lib" )


class CBitmapImage;

inline unsigned int DLL_CALLCONVImageReadProc( void *buffer, unsigned int size, unsigned count, fi_handle handle );
inline unsigned int DLL_CALLCONV ImageWriteProc( void *buffer, unsigned size, unsigned int count, fi_handle handle );
inline int DLL_CALLCONV ImageSeekProc( fi_handle handle, long offset, int origin );
inline long DLL_CALLCONV ImageTellProc( fi_handle handle );


class CImageStreamBufferHolder
{
	stream_buffer *m_pStreamBuffer;

public:

	friend class CBitmapImage;
	friend unsigned int DLL_CALLCONV ImageReadProc( void *buffer, unsigned int size, unsigned count, fi_handle handle );
	friend unsigned int DLL_CALLCONV ImageWriteProc( void *buffer, unsigned size, unsigned int count, fi_handle handle );
	friend int DLL_CALLCONV ImageSeekProc( fi_handle handle, long offset, int origin );
	friend long DLL_CALLCONV ImageTellProc( fi_handle handle );
};


/// The singleton instance of CImageStreamBufferHolder
inline CImageStreamBufferHolder& ImageStreamBufferHolder()
{
	static CImageStreamBufferHolder holder;
	return holder;
}


inline unsigned int DLL_CALLCONV ImageReadProc( void *buffer, unsigned int size, unsigned int count, fi_handle handle )
{
	return ImageStreamBufferHolder().m_pStreamBuffer->read( buffer, size * count );
}

inline unsigned int DLL_CALLCONV ImageWriteProc( void *buffer, unsigned int size, unsigned int count, fi_handle handle )
{
	return ImageStreamBufferHolder().m_pStreamBuffer->write( buffer, size * count );
}

inline int DLL_CALLCONV ImageSeekProc( fi_handle handle, long offset, int origin )
{
	stream_buffer *pStreamBuffer = ImageStreamBufferHolder().m_pStreamBuffer;
	switch(origin)
	{
	case SEEK_SET: pStreamBuffer->seek_pos( offset ); break;
	case SEEK_CUR: pStreamBuffer->seek_pos( pStreamBuffer->get_current_pos() + offset ); break;
	case SEEK_END: pStreamBuffer->seek_pos( (int)pStreamBuffer->buffer().size() + offset ); break;
	default:
		return 0;
	}
	
	return 0; // assume it went successful
}

inline long DLL_CALLCONV ImageTellProc( fi_handle handle )
{
	return (long)ImageStreamBufferHolder().m_pStreamBuffer->get_current_pos();
}


inline FREE_IMAGE_FORMAT ToFIF( CImageArchive::ImageFormat img_fmt )
{
	switch(img_fmt)
	{
	case CImageArchive::IMGFMT_BMP24: return FIF_BMP;
	case CImageArchive::IMGFMT_BMP32: return FIF_BMP;
	case CImageArchive::IMGFMT_JPEG:  return FIF_JPEG;
	case CImageArchive::IMGFMT_TGA:   return FIF_TARGA;
	case CImageArchive::IMGFMT_PNG:   return FIF_PNG;
//	case CImageArchive::IMGFMT_ : return FIF_DDS,
//	case CImageArchive::IMGFMT_ : return FIF_PPM,
//	case CImageArchive::IMGFMT_ : return FIF_DIB,
//	case CImageArchive::IMGFMT_ : return FIF_HDR,       ///< high dynamic range formats
//	case CImageArchive::IMGFMT_ : return FIF_PFM,       ///
	default: return FIF_UNKNOWN;
	}

	return FIF_UNKNOWN;
}


/**
 - Loads image form files (.bmp, .jpg, etc.)
 - Uses FreeImage library

*/
class CBitmapImage
{
	FIBITMAP* m_pFreeImageBitMap;

	int m_BitsPerPixel; ///< the size of one pixel in the bitmap in bits

public:

	CBitmapImage() : m_pFreeImageBitMap(NULL), m_BitsPerPixel(0) {}

	inline CBitmapImage( int width, int height, int bpp );

	/// \param bpp bits per pixel. Must support RGBA format
	inline CBitmapImage( int width, int height, int bpp, const SFloatRGBAColor& color );

	/// TODO: Make the argument const
	/// - Need to do sth to CImageStreamBufferHolder. See the function definition
	inline CBitmapImage( CImageArchive& img_archive );

//	inline CBitmapImage( const C2DArray<SFloatRGBColor>& texel_buffer, int bpp );

	~CBitmapImage() { FreeImage_Unload( m_pFreeImageBitMap ); }

	inline bool LoadFromFile( const std::string& pathname, int flag = 0 );

	inline bool SaveToFile( const std::string& pathname, int flag = 0 );

	inline bool CreateFromImageArchive( CImageArchive& img_archive );

	inline U32 GetPixelARGB32( int x, int y );

	inline void GetPixel( int x, int y, U8& r, U8& g, U8& b, U8& a );

	inline void FillColor( const SFloatRGBAColor& color );

	inline void SetPixel( int x, int y, const SFloatRGBColor& color );

	inline void SetPixel( int x, int y, U8 r, U8 g, U8 b, U8 a );

	/// \param grayscale must be [0,255]
	inline void SetGrayscalePixel( int x, int y, U8 grayscale );

	/// \param alpha alpha component [0,255]
	inline void SetAlpha( int x, int y, U8 alpha );

	FIBITMAP *GetFBITMAP() { return m_pFreeImageBitMap; }

	inline int GetWidth() const;

	inline int GetHeight() const;
};

/*
inline CBitmapImage::CBitmapImage( const C2DArray<SFloatRGBColor>& texel_buffer, int bpp )
{
	m_pFreeImageBitMap = FreeImage_Allocate( width, height, bpp );
}
*/


inline CBitmapImage::CBitmapImage( int width, int height, int bpp )
:
m_BitsPerPixel(bpp)
{
	m_pFreeImageBitMap = FreeImage_Allocate( width, height, bpp );
}


inline CBitmapImage::CBitmapImage( int width, int height, int bpp, const SFloatRGBAColor& color )
:
m_BitsPerPixel(bpp)
{
	m_pFreeImageBitMap = FreeImage_Allocate( width, height, bpp );

	FillColor( color );

}


inline CBitmapImage::CBitmapImage( CImageArchive& img_archive )
:
m_pFreeImageBitMap(NULL),
m_BitsPerPixel(0)
{
	CreateFromImageArchive( img_archive );
}



/// only valid for bitmap of 32 bpp
inline U32 CBitmapImage::GetPixelARGB32( int x, int y )
{
	int bytes_per_pixel = FreeImage_GetBPP(m_pFreeImageBitMap) / sizeof(BYTE);

	if( bytes_per_pixel != 4 )
	{
		int GetPixelARGB32_does_not_work_if_bpp_is_not_32 = 1;
		return 0;
	}

	BYTE *bits = FreeImage_GetBits(m_pFreeImageBitMap);
	bits += ( y * GetWidth() + x ) * bytes_per_pixel;

	U32 argb32
		= (U32)bits[FI_RGBA_ALPHA] << 24
		| (U32)bits[FI_RGBA_RED]   << 16
		| (U32)bits[FI_RGBA_GREEN] << 8
		| (U32)bits[FI_RGBA_BLUE];

	return argb32;
}


inline void CBitmapImage::GetPixel( int x, int y, U8& r, U8& g, U8& b, U8& a )
{
	BYTE *bits = FreeImage_GetScanLine(m_pFreeImageBitMap, y) + m_BitsPerPixel * x;

	r = bits[FI_RGBA_RED];
	g = bits[FI_RGBA_GREEN];
	b = bits[FI_RGBA_BLUE];

	if( m_BitsPerPixel == 32 )
		a = bits[FI_RGBA_ALPHA];
}


inline void CBitmapImage::FillColor( const SFloatRGBAColor& color )
{
	if( !m_pFreeImageBitMap )
		return;

	U8 r = (U8)(color.fRed   * 255);
	U8 g = (U8)(color.fGreen * 255);
	U8 b = (U8)(color.fBlue  * 255);
	U8 a = (U8)(color.fAlpha * 255);

	const int w = GetWidth();
	const int h = GetHeight();
	const int bytespp = FreeImage_GetLine(m_pFreeImageBitMap) / FreeImage_GetWidth(m_pFreeImageBitMap);
	for( int y = 0; y < h; y++)
	{
		BYTE *bits = FreeImage_GetScanLine(m_pFreeImageBitMap, y);
		for( int x = 0; x < w; x++)
		{
			// Set pixel color
			bits[FI_RGBA_RED]   = r;
			bits[FI_RGBA_GREEN] = g;
			bits[FI_RGBA_BLUE]  = b;
			bits[FI_RGBA_ALPHA] = a;

			// jump to next pixel
			bits += bytespp;
		}
	}
}


inline void CBitmapImage::SetPixel( int x, int y, const SFloatRGBColor& color )
{
	RGBQUAD quad;
	quad.rgbRed   = color.GetRedByte();
	quad.rgbGreen = color.GetGreenByte();
	quad.rgbBlue  = color.GetBlueByte();

	FreeImage_SetPixelColor( m_pFreeImageBitMap, x, y, &quad );
}


inline void CBitmapImage::SetPixel( int x, int y, U8 r, U8 g, U8 b, U8 a )
{
	BYTE *bits = FreeImage_GetScanLine(m_pFreeImageBitMap, y) + m_BitsPerPixel * x;

	bits[FI_RGBA_RED]   = r;
	bits[FI_RGBA_GREEN] = g;
	bits[FI_RGBA_BLUE]  = b;

	if( m_BitsPerPixel == 32 )
		bits[FI_RGBA_ALPHA] = a;
}


inline void CBitmapImage::SetGrayscalePixel( int x, int y, U8 grayscale )
{
	RGBQUAD quad;
	quad.rgbRed   = grayscale;
	quad.rgbGreen = grayscale;
	quad.rgbBlue  = grayscale;

//	FreeImage_SetPixelColor( m_pFreeImageBitMap, x, y, &quad );
	FreeImage_SetPixelColor( m_pFreeImageBitMap, x, GetHeight() - y - 1, &quad );
}


inline void CBitmapImage::SetAlpha( int x, int y, U8 alpha )
{
//	int bytespp = FreeImage_GetLine(dib) / FreeImage_GetWidth(dib);

	BYTE *bits = FreeImage_GetScanLine(m_pFreeImageBitMap, y) + m_BitsPerPixel * x;

	bits[FI_RGBA_ALPHA] = alpha;
}


inline int CBitmapImage::GetWidth() const
{
	if( m_pFreeImageBitMap )
		return (int)FreeImage_GetWidth(m_pFreeImageBitMap);
	else
		return 0;
}


inline int CBitmapImage::GetHeight() const
{
	if( m_pFreeImageBitMap )
		return (int)FreeImage_GetHeight(m_pFreeImageBitMap);
	else
		return 0;
}



// ----------------------------------------------------------

/** Generic image loader
	@param pathname Pointer to the full file name
	@param flag Optional load flag constant
	@return Returns true on success
*/
inline bool CBitmapImage::LoadFromFile( const std::string& pathname, int flag )
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	// check the file signature and deduce its format
	// (the second argument is currently not used by FreeImage)
	fif = FreeImage_GetFileType( pathname.c_str(), 0 );
	if(fif == FIF_UNKNOWN)
	{
		// no signature ?
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename( pathname.c_str() );
	}

	// check that the plugin has reading capabilities ...
	if( (fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif) )
	{
		// ok, let's load the file
		m_pFreeImageBitMap = FreeImage_Load(fif, pathname.c_str(), flag);

		// unless a bad file format, we are done !
		if( m_pFreeImageBitMap )
		{
			m_BitsPerPixel = FreeImage_GetBPP( m_pFreeImageBitMap );
			return true;
		}
		else
			return false;
	}

	return false;
}

/** Generic image writer
	@param dib Pointer to the dib to be saved
	@param pathname Pointer to the full file name
	@param flag Optional save flag constant
	@return Returns true if successful, returns false otherwise
*/
inline bool CBitmapImage::SaveToFile( const std::string& pathname, int flag )
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	BOOL bSuccess = FALSE;

	if(m_pFreeImageBitMap)
	{
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename( pathname.c_str() );
		if(fif != FIF_UNKNOWN )
		{
			// check that the plugin has sufficient writing and export capabilities ...
			WORD bpp = FreeImage_GetBPP(m_pFreeImageBitMap);
			if(FreeImage_FIFSupportsWriting(fif) && FreeImage_FIFSupportsExportBPP(fif, bpp))
			{
				// ok, we can save the file
				bSuccess = FreeImage_Save(fif, m_pFreeImageBitMap, pathname.c_str(), flag);
				LOG_PRINT( "Saved an image file to disk: " + pathname );

				// unless an abnormal bug, we are done !
			}
			else
			{
				LOG_PRINT_ERROR( "Cannot save an image to disk: " + pathname );
			}
		}
		else
			LOG_PRINT_ERROR( "Failed to save an image file to disk: " + pathname );
	}

	return (bSuccess == TRUE) ? true : false;
}


inline bool CBitmapImage::CreateFromImageArchive( CImageArchive& img_archive )
{

//	lock

	ImageStreamBufferHolder().m_pStreamBuffer = &(img_archive.m_Buffer);
	FreeImageIO img_io;
	img_io.read_proc  = ImageReadProc;
	img_io.write_proc = ImageWriteProc;
	img_io.seek_proc  = ImageSeekProc;
	img_io.tell_proc  = ImageTellProc;

	int sth = 0;

	int flags = 0;
	m_pFreeImageBitMap = FreeImage_LoadFromHandle( ToFIF(img_archive.m_Format), &img_io, &sth, flags );

	if( m_pFreeImageBitMap )
	{
		m_BitsPerPixel = FreeImage_GetBPP( m_pFreeImageBitMap );
		return true;
	}
	else
		return false;
}

//
// Global Functions
//

inline boost::shared_ptr<CBitmapImage> CreateBitmapImage( const std::string& pathname, int flag = 0 )
{
	boost::shared_ptr<CBitmapImage> pImage
		= boost::shared_ptr<CBitmapImage>( new CBitmapImage() );

	bool bSuccess = pImage->LoadFromFile( pathname, flag );

	return pImage;
}


inline bool SaveToImageFile( const C2DArray<SFloatRGBColor>& texel, const std::string& filepath )
{
	int x,y;
	int width  = texel.size_x();
	int height = texel.size_y();
	const int depth = 24;

	CBitmapImage img( width, height, depth );

	for( y=0; y<height ; y++ )
	{
		for( x=0; x<width; x++ )
		{
			img.SetPixel( x, y, texel(x,y) );
		}
	}

	return img.SaveToFile( filepath );
}


inline bool SaveGrayscaleToImageFile( const C2DArray<U8>& texel, const std::string& filepath )
{
	int x,y;
	int width  = texel.size_x();
	int height = texel.size_y();
	const int depth = 24;

	CBitmapImage img( width, height, depth );

	for( y=0; y<height ; y++ )
	{
		for( x=0; x<width; x++ )
		{
			img.SetGrayscalePixel( x, y, texel(x,y) );
		}
	}

	return img.SaveToFile( filepath );
}


#endif /* __BitmapImage_H__ */
