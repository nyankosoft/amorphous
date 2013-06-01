#ifndef __BitmapImage_H__
#define __BitmapImage_H__


#include <string>
#include <boost/shared_ptr.hpp>
#include "FreeImage.h"

#include "../Graphics/FloatRGBColor.hpp"
#include "../Graphics/FloatRGBAColor.hpp"
#include "ImageArchive.hpp"
#include "array2d.hpp"
#include "Macro.h"


namespace amorphous
{

// Comment out this header inclusion and do the following replacings
// if you want to use BitmapImage class without the log system.
// 1. Replace 'LOG_PRINT_ERROR(x)' with 'cout << x'.
// 2. Replace 'g_Log.Print(x)' with 'printf(x)'.
#include "Log/DefaultLog.hpp"

#pragma comment( lib, "FreeImage.lib" )


class BitmapImage;

inline unsigned int DLL_CALLCONVImageReadProc( void *buffer, unsigned int size, unsigned count, fi_handle handle );
inline unsigned int DLL_CALLCONV ImageWriteProc( void *buffer, unsigned size, unsigned int count, fi_handle handle );
inline int DLL_CALLCONV ImageSeekProc( fi_handle handle, long offset, int origin );
inline long DLL_CALLCONV ImageTellProc( fi_handle handle );


class CImageStreamBufferHolder
{
	stream_buffer *m_pStreamBuffer;

public:

	friend class BitmapImage;
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


inline FREE_IMAGE_FORMAT ToFIF( ImageArchive::ImageFormat img_fmt )
{
	switch(img_fmt)
	{
	case ImageArchive::IMGFMT_BMP24: return FIF_BMP;
	case ImageArchive::IMGFMT_BMP32: return FIF_BMP;
	case ImageArchive::IMGFMT_JPEG:  return FIF_JPEG;
	case ImageArchive::IMGFMT_TGA:   return FIF_TARGA;
	case ImageArchive::IMGFMT_PNG:   return FIF_PNG;
//	case ImageArchive::IMGFMT_ : return FIF_DDS,
//	case ImageArchive::IMGFMT_ : return FIF_PPM,
//	case ImageArchive::IMGFMT_ : return FIF_DIB,
//	case ImageArchive::IMGFMT_ : return FIF_HDR,       ///< high dynamic range formats
//	case ImageArchive::IMGFMT_ : return FIF_PFM,       ///
	default: return FIF_UNKNOWN;
	}

	return FIF_UNKNOWN;
}


/**
 - Loads image form files (.bmp, .jpg, etc.)
 - Uses FreeImage library

*/
class BitmapImage
{
	FIBITMAP* m_pFreeImageBitMap;

	int m_BitsPerPixel; ///< the size of one pixel in the bitmap in bits

private:

	BitmapImage( FIBITMAP *pFreeImageBitMap )
		:
	m_pFreeImageBitMap(NULL),
	m_BitsPerPixel(0)
	{
		Reset(pFreeImageBitMap);
	}

	void Reset( FIBITMAP *pFreeImageBitMap )
	{
		Release();
		m_pFreeImageBitMap = pFreeImageBitMap;
		if( m_pFreeImageBitMap )
			m_BitsPerPixel = FreeImage_GetBPP( m_pFreeImageBitMap );
	}

	void Release()
	{
		if( m_pFreeImageBitMap )
		{
			FreeImage_Unload( m_pFreeImageBitMap );
			m_pFreeImageBitMap = NULL;
			m_BitsPerPixel = 0;
		}
	}

public:

	BitmapImage() : m_pFreeImageBitMap(NULL), m_BitsPerPixel(0) {}

	inline BitmapImage( int width, int height, int bpp );

	/// \param bpp bits per pixel. Must support RGBA format
	inline BitmapImage( int width, int height, int bpp, const SFloatRGBAColor& color );

	/// TODO: Make the argument const
	/// - Need to do sth to CImageStreamBufferHolder. See the function definition
	inline BitmapImage( ImageArchive& img_archive );

//	inline BitmapImage( const array2d<SFloatRGBColor>& texel_buffer, int bpp );

	~BitmapImage() { Release(); }

	inline bool LoadFromFile( const std::string& pathname, int flag = 0 );

	inline bool SaveToFile( const std::string& pathname, int flag = 0 );

	inline bool CreateFromImageArchive( ImageArchive& img_archive );

	inline void FillColor( const SFloatRGBAColor& color );

	inline U32 GetPixelARGB32( int x, int y );

	inline void GetPixel( int x, int y, SFloatRGBAColor& color );

	inline SFloatRGBAColor GetPixel( int x, int y );

	inline void GetPixel( int x, int y, U8& r, U8& g, U8& b );

	inline void GetPixel( int x, int y, U8& r, U8& g, U8& b, U8& a );

	/// Support only 24-bit/32-bit images. Float values are internally converted to U8 values.
	inline void SetPixel( int x, int y, const SFloatRGBAColor& color );

	/// Support only 24-bit/32-bit images. Float values are internally converted to U8 values.
	inline void SetPixel( int x, int y, const SFloatRGBColor& color );

	inline void SetPixel( int x, int y, U8 r, U8 g, U8 b );

	inline void SetPixel( int x, int y, U8 r, U8 g, U8 b, U8 a );

	/// \param grayscale must be [0,255]
	inline void SetGrayscalePixel( int x, int y, U8 grayscale );

	/// \param alpha alpha component [0,255]
	inline void SetAlpha( int x, int y, U8 alpha );

	FIBITMAP *GetFBITMAP() { return m_pFreeImageBitMap; }

	inline int GetWidth() const;

	inline int GetHeight() const;

	inline bool Rescale( int dest_width, int dest_height/*, CImageFilter::Name filter */ );

	inline bool FlipVertical();

	inline boost::shared_ptr<BitmapImage> GetRescaled( int dest_width, int dest_height/*, CImageFilter::Name filter */ ) const;

	inline const char *GetColorTypeName() const;
};

/*
inline BitmapImage::BitmapImage( const array2d<SFloatRGBColor>& texel_buffer, int bpp )
{
	m_pFreeImageBitMap = FreeImage_Allocate( width, height, bpp );
}
*/


inline void LogFreeImageError( FREE_IMAGE_FORMAT fif, const char *message )
{
	g_Log.Print( WL_ERROR, "FreeImage: %s (image format: %s).",
		message,
		(fif != FIF_UNKNOWN) ? FreeImage_GetFormatFromFIF(fif) : "unknown"
		);
}


inline void InitFreeImage()
{
	FreeImage_SetOutputMessage(LogFreeImageError);

	LOG_PRINT( std::string("FreeImage version: ") + FreeImage_GetVersion() );
}


inline BitmapImage::BitmapImage( int width, int height, int bpp )
:
m_BitsPerPixel(bpp)
{
	m_pFreeImageBitMap = FreeImage_Allocate( width, height, bpp );
}


inline BitmapImage::BitmapImage( int width, int height, int bpp, const SFloatRGBAColor& color )
:
m_BitsPerPixel(bpp)
{
	m_pFreeImageBitMap = FreeImage_Allocate( width, height, bpp );

	FillColor( color );

}


inline BitmapImage::BitmapImage( ImageArchive& img_archive )
:
m_pFreeImageBitMap(NULL),
m_BitsPerPixel(0)
{
	CreateFromImageArchive( img_archive );
}



/// only valid for bitmap of 32 bpp
inline U32 BitmapImage::GetPixelARGB32( int x, int y )
{
	U8 r=0, g=0, b=0, a=255;
	GetPixel( x, y, r, g, b, a );
	U32 argb32
		= (U32)a << 24
		| (U32)r << 16
		| (U32)g << 8
		| (U32)b;

	return argb32;

/*	int bytes_per_pixel = FreeImage_GetBPP(m_pFreeImageBitMap) / sizeof(BYTE);

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

	return argb32;*/
}


inline void BitmapImage::GetPixel( int x, int y, SFloatRGBAColor& color )
{
	U8 r=0, g=0, b=0, a=0;
	GetPixel( x, y, r, g, b, a );
	color.red   = (float)r / (float)255.0f;
	color.green = (float)g / (float)255.0f;
	color.blue  = (float)b / (float)255.0f;
	color.alpha = (float)a / (float)255.0f;
}


inline SFloatRGBAColor BitmapImage::GetPixel( int x, int y )
{
	SFloatRGBAColor dest;
	GetPixel( x, y, dest );
	return dest;
}


inline void BitmapImage::GetPixel( int x, int y, U8& r, U8& g, U8& b )
{
	RGBQUAD quad;
	memset( &quad, 0, sizeof(RGBQUAD) );

	FreeImage_GetPixelColor( m_pFreeImageBitMap, x, GetHeight() - y - 1, &quad );
	r = quad.rgbRed;
	g = quad.rgbGreen;
	b = quad.rgbBlue;
}


inline void BitmapImage::GetPixel( int x, int y, U8& r, U8& g, U8& b, U8& a )
{
	BYTE *bits = FreeImage_GetScanLine(m_pFreeImageBitMap, y) + m_BitsPerPixel / 8 * x;

	r = bits[FI_RGBA_RED];
	g = bits[FI_RGBA_GREEN];
	b = bits[FI_RGBA_BLUE];

	if( m_BitsPerPixel == 32 )
		a = bits[FI_RGBA_ALPHA];
}


inline void BitmapImage::FillColor( const SFloatRGBAColor& color )
{
	if( !m_pFreeImageBitMap )
		return;

	U8 r = (U8)(color.red   * 255);
	U8 g = (U8)(color.green * 255);
	U8 b = (U8)(color.blue  * 255);
	U8 a = (U8)(color.alpha * 255);

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


inline void BitmapImage::SetPixel( int x, int y, const SFloatRGBAColor& color )
{
	// TODO: support float RGBA images
	if( m_BitsPerPixel == 24
	 || m_BitsPerPixel == 32 )
	{
		SetPixel( x, y, color.GetRedByte(), color.GetGreenByte(), color.GetBlueByte(), color.GetAlphaByte() );
	}
}


inline void BitmapImage::SetPixel( int x, int y, const SFloatRGBColor& color )
{
	RGBQUAD quad;
	quad.rgbRed   = color.GetRedByte();
	quad.rgbGreen = color.GetGreenByte();
	quad.rgbBlue  = color.GetBlueByte();

	FreeImage_SetPixelColor( m_pFreeImageBitMap, x, GetHeight() - y - 1, &quad );
}


inline void BitmapImage::SetPixel( int x, int y, U8 r, U8 g, U8 b )
{
	RGBQUAD quad;
	quad.rgbRed   = r;
	quad.rgbGreen = g;
	quad.rgbBlue  = b;

	FreeImage_SetPixelColor( m_pFreeImageBitMap, x, GetHeight() - y - 1, &quad );
}


inline void BitmapImage::SetPixel( int x, int y, U8 r, U8 g, U8 b, U8 a )
{
	if( !m_pFreeImageBitMap )
		return;

	if( m_BitsPerPixel == 24 )
	{
		SetPixel( x, y, r, g, b );
	}
	else if( m_BitsPerPixel == 32 )
	{
		// This code causes crash when the bpp is 32. Why?

		BYTE *bits = FreeImage_GetScanLine(m_pFreeImageBitMap, y) + m_BitsPerPixel / 8 * x;

		int bytespp = FreeImage_GetLine(m_pFreeImageBitMap) / FreeImage_GetWidth(m_pFreeImageBitMap);

		bits[FI_RGBA_RED]   = r;
		bits[FI_RGBA_GREEN] = g;
		bits[FI_RGBA_BLUE]  = b;
		bits[FI_RGBA_ALPHA] = a;
	}
}


inline void BitmapImage::SetGrayscalePixel( int x, int y, U8 grayscale )
{
	RGBQUAD quad;
	quad.rgbRed   = grayscale;
	quad.rgbGreen = grayscale;
	quad.rgbBlue  = grayscale;

//	FreeImage_SetPixelColor( m_pFreeImageBitMap, x, y, &quad );
	FreeImage_SetPixelColor( m_pFreeImageBitMap, x, GetHeight() - y - 1, &quad );
}


inline void BitmapImage::SetAlpha( int x, int y, U8 alpha )
{
//	int bytespp = FreeImage_GetLine(dib) / FreeImage_GetWidth(dib);

	BYTE *bits = FreeImage_GetScanLine(m_pFreeImageBitMap, y) + m_BitsPerPixel / 8 * x;

	bits[FI_RGBA_ALPHA] = alpha;
}


inline int BitmapImage::GetWidth() const
{
	if( m_pFreeImageBitMap )
		return (int)FreeImage_GetWidth(m_pFreeImageBitMap);
	else
		return 0;
}


inline int BitmapImage::GetHeight() const
{
	if( m_pFreeImageBitMap )
		return (int)FreeImage_GetHeight(m_pFreeImageBitMap);
	else
		return 0;
}


inline bool BitmapImage::Rescale( int dest_width, int dest_height/*, CImageFilter::Name filter */ )
{
	if( m_pFreeImageBitMap )
	{
		// FreeImage_Rescale() does not change the image specified as the first argument.
		// It returns the scaled image.
		FIBITMAP *pScaledImage = FreeImage_Rescale( m_pFreeImageBitMap, dest_width, dest_height, FILTER_BILINEAR/*filter*/ );
		Reset( pScaledImage );
		return true;
	}
	else
		return false;
}


inline bool BitmapImage::FlipVertical()
{
	if( m_pFreeImageBitMap )
	{
		BOOL res = FreeImage_FlipVertical( m_pFreeImageBitMap );
		return res ? true : false;
	}
	else
		return false;
}


inline boost::shared_ptr<BitmapImage> BitmapImage::GetRescaled( int dest_width, int dest_height/*, CImageFilter::Name filter */ ) const
{
	if( m_pFreeImageBitMap )
	{
		// FreeImage_Rescale() does not change the image specified as the first argument.
		// It returns the scaled image.
		FIBITMAP *pScaledBitMap = FreeImage_Rescale( m_pFreeImageBitMap, dest_width, dest_height, FILTER_BILINEAR/*filter*/ );
		boost::shared_ptr<BitmapImage> pScaledImage( new BitmapImage( pScaledBitMap ) );
		return pScaledImage;
	}
	else
		return boost::shared_ptr<BitmapImage>();
}


inline const char *BitmapImage::GetColorTypeName() const
{
	if( !m_pFreeImageBitMap )
		return "(image is not loaded)";

	FREE_IMAGE_COLOR_TYPE color_type = FreeImage_GetColorType( m_pFreeImageBitMap );

	switch( color_type )
	{
	case FIC_MINISWHITE: return "MINISWHITE";
	case FIC_MINISBLACK: return "MINISBLACK";
	case FIC_RGB:        return "RGB";
	case FIC_PALETTE:    return "PALETTE";
	case FIC_RGBALPHA:   return "RGBA";
	case FIC_CMYK:       return "CMYK";
	default:
		return "(an unknown color type)";
	}
}


// ----------------------------------------------------------

/** Generic image loader
	@param pathname Pointer to the full file name
	@param flag Optional load flag constant
	@return Returns true on success
*/
inline bool BitmapImage::LoadFromFile( const std::string& pathname, int flag )
{
	Release();

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

	if(fif == FIF_UNKNOWN)
	{
		LOG_PRINT_ERROR( " An unsupported image file format." );
		return false;
	}

	// check that the plugin has reading capabilities ...
	if( !FreeImage_FIFSupportsReading(fif) )
	{
		LOG_PRINT_ERROR( " FreeImage_FIFSupportsReading() failed on the following image file: " + pathname );
		return false;
	}

	// ok, let's load the file
	m_pFreeImageBitMap = FreeImage_Load(fif, pathname.c_str(), flag);

	// unless a bad file format, we are done !
	if( m_pFreeImageBitMap )
	{
		m_BitsPerPixel = FreeImage_GetBPP( m_pFreeImageBitMap );
		return true;
	}
	else
	{
		LOG_PRINT_ERROR( " Failed to load a file: " + pathname );
		return false;
	}
}

/** Generic image writer
	@param dib Pointer to the dib to be saved
	@param pathname Pointer to the full file name
	@param flag Optional save flag constant
	@return Returns true if successful, returns false otherwise
*/
inline bool BitmapImage::SaveToFile( const std::string& pathname, int flag )
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	BOOL bSuccess = FALSE;

	if( !m_pFreeImageBitMap )
	{
		LOG_PRINT_ERROR( " Has no valid bitmap." );
		return false;
	}

	// try to guess the file format from the file extension
	fif = FreeImage_GetFIFFromFilename( pathname.c_str() );
	if(fif == FIF_UNKNOWN )
	{
		LOG_PRINT_ERROR( " An unsupported image format: " + pathname );
		return false;
	}

	// check that the plugin has sufficient writing and export capabilities ...
	WORD bpp = FreeImage_GetBPP(m_pFreeImageBitMap);
	BOOL supports_writing = FreeImage_FIFSupportsWriting(fif);
	if( !supports_writing )
	{
		LOG_PRINT_ERROR( " FreeImage_FIFSupportsWriting() returned false. Cannot save the image to disk as " + pathname );
		return false;
	}

	BOOL supports_export_bpp = FreeImage_FIFSupportsExportBPP(fif, bpp);
	if( !supports_export_bpp )
	{
		LOG_PRINT_ERROR( " Cannot save an image to disk: " + pathname );
		return false;
	}

	// ok, we can save the file
	bSuccess = FreeImage_Save(fif, m_pFreeImageBitMap, pathname.c_str(), flag);

	// unless an abnormal bug, we are done !
	if( bSuccess == TRUE )
	{
		LOG_PRINT_VERBOSE( " Saved an image file to disk: " + pathname );
		return true;
	}
	else
	{
		LOG_PRINT_ERROR( " Failed to save an image file to disk: " + pathname );
		return false;
	}
}


inline bool BitmapImage::CreateFromImageArchive( ImageArchive& img_archive )
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

inline boost::shared_ptr<BitmapImage> CreateBitmapImage( const std::string& pathname, int flag = 0 )
{
	boost::shared_ptr<BitmapImage> pImage( new BitmapImage() );

	bool bSuccess = pImage->LoadFromFile( pathname, flag );

	return bSuccess ? pImage : boost::shared_ptr<BitmapImage>();
}


inline bool SaveToImageFile( const array2d<SFloatRGBColor>& texel, const std::string& filepath )
{
	int x,y;
	int width  = texel.size_x();
	int height = texel.size_y();
	const int depth = 24;

	BitmapImage img( width, height, depth );

	for( y=0; y<height ; y++ )
	{
		for( x=0; x<width; x++ )
		{
			img.SetPixel( x, y, texel(x,y) );
		}
	}

	return img.SaveToFile( filepath );
}


inline bool SaveToImageFile( const array2d<SFloatRGBAColor>& texel, const std::string& filepath )
{
	int x,y;
	int width  = texel.size_x();
	int height = texel.size_y();
	const int depth = 32;

	BitmapImage img( width, height, depth );

	for( y=0; y<height ; y++ )
	{
		for( x=0; x<width; x++ )
		{
			img.SetPixel( x, y, texel(x,y) );
		}
	}

	return img.SaveToFile( filepath );
}


inline bool SaveGrayscaleToImageFile( const array2d<U8>& texel, const std::string& filepath )
{
	int x,y;
	int width  = texel.size_x();
	int height = texel.size_y();
	const int depth = 24;

	BitmapImage img( width, height, depth );

	for( y=0; y<height ; y++ )
	{
		for( x=0; x<width; x++ )
		{
			img.SetGrayscalePixel( x, y, texel(x,y) );
		}
	}

	return img.SaveToFile( filepath );
}

} // namespace amorphous



#endif /* __BitmapImage_H__ */
