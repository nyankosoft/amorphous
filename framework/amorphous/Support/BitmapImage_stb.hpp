#ifndef __amorphous_BitmapImage_stb_HPP__
#define __amorphous_BitmapImage_stb_HPP__


#include <memory>

#include "stream_buffer.hpp"

// Comment out this header inclusion and do the following replacings
// if you want to use BitmapImage class without the log system.
// 1. Define the following macros
//   #define LOG_PRINT(x)         std::cout << x
//   #define LOG_PRINT_VERBOSE(x) std::cout << x
//   #define LOG_PRINT_ERROR(x)   std::cout << x
// 2. Replace 'g_Log.Print(x)' with 'printf(x)'.
#include "Log/DefaultLog.hpp"


namespace amorphous
{
using std::shared_ptr;

class BitmapImage;


/**
 \brief Loads and saves images.

 - Loads image form files (.bmp, .jpg, etc.)
 - Uses stb_image single header image library

*/
class BitmapImage
{
	unsigned char *m_pStbImage;

    int m_ImageWidth;
    
    int m_ImageHeight;
    
    int m_NumChannels; /// Either 4(RGBA) or 3(RGB) most of the time.

	//int m_BitsPerPixel; ///< the size of one pixel in the bitmap in bits

private:

	BitmapImage( unsigned char *pStbImage )
		:
	m_pStbImage(nullptr),
	m_ImageWidth(0),
	m_ImageHeight(0),
	m_NumChannels(0)
	{
		Reset(pStbImage);
	}

	void Reset( unsigned char *pStbImage )
	{
		Release();
		m_pStbImage = pStbImage;
	}

    // I don't remember why this had to be declared private.
	void Release();

	inline bool Save32BPPImageTo24JPEGFile( const std::string& image_file_pathname, int flag );

public:

	BitmapImage() : m_pStbImage(nullptr), m_ImageWidth(0), m_ImageHeight(0), m_NumChannels(0) {}

	/**
	\param bits_per_pixel 32 for RGBA or 24 RGB image
	*/
	BitmapImage( int width, int height, int bits_per_pixel );

	BitmapImage( unsigned char *pToBeCopied, int width, int height, int num_channels );

	BitmapImage( const BitmapImage& img );

	/// \param bpp bits per pixel. Must support RGBA format
//	inline BitmapImage( int width, int height, int bpp, const RGBAColorType& color );

	/// TODO: Make the argument const
	/// - Need to do sth to ImageStreamBufferHolder. See the function definition
	inline BitmapImage( stream_buffer& image_data, const std::string& image_format );

	~BitmapImage() { Release(); }

	/**
	@brief Loads an image from a file

	@param pathname Pointer to the full file name
	@param flag Optional load flag constant

	@return Returns true on success
	*/
	bool LoadFromFile( const std::string& pathname, int flag = 0 );

	/** 
	@brief Saves the image to a file

	@param pathname Pointer to the full file name
	@param flag Optional save flag constant

	@return Returns true if successful, returns false otherwise
	*/
	bool SaveToFile( const std::string& pathname, int flag = 0 );

	bool CreateFromImageDataStream( stream_buffer& image_data, const std::string& image_format );

	bool LoadFromMemory( const void *buffer, int size_in_bytes );

	/**
	\brief Paints the entire image with the specified color

	All the color components shall be in the [0,255] range 
	*/
	inline void FillColor( U8 r, U8 g, U8 b, U8 a );

	/**
	\brief Paints the entire image with the specified color

	All the color components shall be in the [0.0,1.0] range 
	*/
	inline void FillColor( float r, float g, float b, float a );

	/**
	\brief 'RGBAColorType' shall have the following public class members,
	each of which shall be floating point type in the range of [0.0,1.0]

	- RGBAColorType::red
	- RGBAColorType::green
	- RGBAColorType::blue
	- RGBAColorType::alpha
	*/
	template<class RGBAColorType>
	inline void FillFRGBAColor( const RGBAColorType& color );

	inline U32 GetPixelARGB32( int x, int y );

//	inline void GetPixel( int x, int y, SFloatRGBAColor& color );

//	inline SFloatRGBAColor GetPixel( int x, int y );

	inline void GetPixel( int x, int y, U8& r, U8& g, U8& b );

	inline void GetPixel( int x, int y, U8& r, U8& g, U8& b, U8& a );

	inline void GetPixel( int x, int y, float& r, float& g, float& b );

	inline void GetPixel( int x, int y, float& r, float& g, float& b, float& a );

	/**
	  \brief Support only 24-bit/32-bit images. Float values are internally converted to U8 values.

	  See FillFRGBAColor for the requirements to RGBAColorType
	*/
	template<class RGBAColorType>
	inline void SetFRGBAPixel( int x, int y, const RGBAColorType& color )
	{
		SetPixel( x, y, color.red, color.green, color.blue, color.alpha );
	}

	inline void SetPixel( int x, int y, U8 r, U8 g, U8 b );

	inline void SetPixel( int x, int y, U8 r, U8 g, U8 b, U8 a );

	inline void SetPixel( int x, int y, float r, float g, float b );

	inline void SetPixel( int x, int y, float r, float g, float b, float a );

	/// \param grayscale must be [0,255]
	inline void SetGrayscalePixel( int x, int y, U8 grayscale );

	/// \param alpha alpha component [0,255]
	inline void SetAlpha( int x, int y, U8 alpha );

	unsigned char *GetImageData() const { return m_pStbImage; }

	inline int GetWidth() const;

	inline int GetHeight() const;

	inline int GetBitsPerPixel() const;

	bool Rescale( int dest_width, int dest_height/*, CImageFilter::Name filter */ );

	bool FlipVertical();

	shared_ptr<BitmapImage> CreateCopy() const;

	shared_ptr<BitmapImage> GetRescaled( int dest_width, int dest_height/*, CImageFilter::Name filter */ ) const;

	inline const char *GetColorTypeName() const;
};


inline BitmapImage::BitmapImage( stream_buffer& image_data, const std::string& image_format )
:
m_pStbImage(nullptr),
m_ImageWidth(0),
m_ImageHeight(0),
m_NumChannels(0)
{
	CreateFromImageDataStream( image_data, image_format );
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

/*	int bytes_per_pixel = FreeImage_GetBPP(m_pStbImage) / sizeof(BYTE);

	if( bytes_per_pixel != 4 )
	{
		int GetPixelARGB32_does_not_work_if_bpp_is_not_32 = 1;
		return 0;
	}

	BYTE *bits = FreeImage_GetBits(m_pStbImage);
	bits += ( y * GetWidth() + x ) * bytes_per_pixel;

	U32 argb32
		= (U32)bits[FI_RGBA_ALPHA] << 24
		| (U32)bits[FI_RGBA_RED]   << 16
		| (U32)bits[FI_RGBA_GREEN] << 8
		| (U32)bits[FI_RGBA_BLUE];

	return argb32;*/
}


//inline void BitmapImage::GetPixel( int x, int y, SFloatRGBAColor& color )
//{
//	U8 r=0, g=0, b=0, a=0;
//	GetPixel( x, y, r, g, b, a );
//	color.red   = (float)r / (float)255.0f;
//	color.green = (float)g / (float)255.0f;
//	color.blue  = (float)b / (float)255.0f;
//	color.alpha = (float)a / (float)255.0f;
//}


//inline SFloatRGBAColor BitmapImage::GetPixel( int x, int y )
//{
//	SFloatRGBAColor dest;
//	GetPixel( x, y, dest );
//	return dest;
//}


inline void BitmapImage::GetPixel( int x, int y, float& r, float& g, float& b )
{
	U8 _r=0, _g=0, _b=0;
	GetPixel( x, y, _r, _g, _b );
	r = (float)_r / (float)255.0f;
	g = (float)_g / (float)255.0f;
	b = (float)_b / (float)255.0f;
}


inline void BitmapImage::GetPixel( int x, int y, float& r, float& g, float& b, float& a )
{
	U8 _r=0, _g=0, _b=0, _a=0;
	GetPixel( x, y, _r, _g, _b, _a );
	r = (float)_r / (float)255.0f;
	g = (float)_g / (float)255.0f;
	b = (float)_b / (float)255.0f;
	a = (float)_a / (float)255.0f;
}


inline void BitmapImage::GetPixel( int x, int y, U8& r, U8& g, U8& b )
{
	if( !m_pStbImage )
	    return;

    const unsigned char *pixel = m_pStbImage + m_ImageWidth * y + x;

	r = pixel[0];
	g = pixel[1];
	b = pixel[2];
}


inline void BitmapImage::GetPixel( int x, int y, U8& r, U8& g, U8& b, U8& a )
{
	if( !m_pStbImage )
	    return;

    const unsigned char *pixel = m_pStbImage + m_ImageWidth * y + x;

	r = pixel[0];
	g = pixel[1];
	b = pixel[2];
	a = pixel[3];
}


inline void BitmapImage::FillColor( U8 r, U8 g, U8 b, U8 a )
{
	const int w = GetWidth();
	const int h = GetHeight();
	const int bytes_per_pixel = m_NumChannels;
	for( int y = 0; y < h; y++)
	{
		for( int x = 0; x < w; x++)
		{
			unsigned char *pixel = m_pStbImage + (w * y + x) * bytes_per_pixel;

			// Set pixel color
			pixel[0] = r;
			pixel[1] = g;
			pixel[2] = b;

			if( bytes_per_pixel == 4 )
				pixel[3] = a;

			// jump to next pixel
			//pixel += 4;
		}
	}
}


template<class RGBAColorType>
inline void BitmapImage::FillFRGBAColor( const RGBAColorType& color )
{
	if( !m_pStbImage )
		return;

	U8 r = (U8)get_clamped( (int)(color.red   * 255), 0, 255 );
	U8 g = (U8)get_clamped( (int)(color.green * 255), 0, 255 );
	U8 b = (U8)get_clamped( (int)(color.blue  * 255), 0, 255 );
	U8 a = (U8)get_clamped( (int)(color.alpha * 255), 0, 255 );

	FillColor(r,g,b,a);
}


inline void BitmapImage::SetPixel( int x, int y, U8 r, U8 g, U8 b )
{
    if( !m_pStbImage )
        return;

    unsigned char *pixel = m_pStbImage + m_ImageWidth * y + x;
    
    pixel[0] = r;
    pixel[1] = g;
    pixel[2] = b;
}


inline void BitmapImage::SetPixel( int x, int y, U8 r, U8 g, U8 b, U8 a )
{
	if( !m_pStbImage )
		return;

    unsigned char *pixel = m_pStbImage + m_ImageWidth * y + x;
    
    pixel[0] = r;
    pixel[1] = g;
    pixel[2] = b;
    pixel[3] = a;
}


void BitmapImage::SetPixel( int x, int y, float r, float g, float b )
{
	SetPixel(
		x, y,
		(U8)(get_clamped(r,0.0f,1.0f) * 255.0f),
		(U8)(get_clamped(g,0.0f,1.0f) * 255.0f),
		(U8)(get_clamped(b,0.0f,1.0f) * 255.0f)
		);
}


void BitmapImage::SetPixel( int x, int y, float r, float g, float b, float a )
{
	SetPixel(
		x, y,
		(U8)(get_clamped(r,0.0f,1.0f) * 255.0f),
		(U8)(get_clamped(g,0.0f,1.0f) * 255.0f),
		(U8)(get_clamped(b,0.0f,1.0f) * 255.0f),
		(U8)(get_clamped(a,0.0f,1.0f) * 255.0f)
	);
}


inline void BitmapImage::SetGrayscalePixel( int x, int y, U8 grayscale )
{
    SetPixel(x,y,grayscale,grayscale,grayscale,grayscale);
}


inline void BitmapImage::SetAlpha( int x, int y, U8 alpha )
{
	if( !m_pStbImage )
		return;

    unsigned char *pixel = m_pStbImage + m_ImageWidth * y + x;
    
    pixel[3] = alpha;
}


inline int BitmapImage::GetWidth() const
{
	if( m_pStbImage )
		return (int)m_ImageWidth;
	else
		return 0;
}


inline int BitmapImage::GetHeight() const
{
	if( m_pStbImage )
		return (int)m_ImageHeight;
	else
		return 0;
}


inline int BitmapImage::GetBitsPerPixel() const
{
	if( m_pStbImage )
		return (int)m_NumChannels * 8;
	else
		return 0;
}

inline const char *BitmapImage::GetColorTypeName() const
{
	if( !m_pStbImage )
		return "(image is not loaded)";

    return (m_NumChannels == 4) ? "RGBA" : "RGB";

//	FREE_IMAGE_COLOR_TYPE color_type = FreeImage_GetColorType( m_pStbImage );

//	switch( color_type )
//	{
//	case FIC_MINISWHITE: return "MINISWHITE";
//	case FIC_MINISBLACK: return "MINISBLACK";
//	case FIC_RGB:        return "RGB";
//	case FIC_PALETTE:    return "PALETTE";
//	case FIC_RGBALPHA:   return "RGBA";
//	case FIC_CMYK:       return "CMYK";
//	default:
//		return "(an unknown color type)";
//	}
}


inline bool BitmapImage::Save32BPPImageTo24JPEGFile( const std::string& image_file_pathname, int flag )
{
	LOG_PRINT_ERROR( "Not implemented." );
	return false;
}

//
// Global Functions
//

inline shared_ptr<BitmapImage> CreateBitmapImage( const std::string& pathname, int flag = 0 )
{
	shared_ptr<BitmapImage> pImage( new BitmapImage() );

	bool bSuccess = pImage->LoadFromFile( pathname, flag );

	return bSuccess ? pImage : shared_ptr<BitmapImage>();
}


} // namespace amorphous


#endif /* __amorphous_BitmapImage_stb_HPP__ */
