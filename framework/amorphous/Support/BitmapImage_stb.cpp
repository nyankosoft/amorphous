#include "BitmapImage_stb.hpp"

// stb single header image library copied from https://github.com/nothings/stb/blob/master/stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"


namespace amorphous
{
using namespace std;


BitmapImage::BitmapImage( int width, int height, int bits_per_pixel )
	:
	m_pStbImage(nullptr),
	m_ImageWidth(0),
	m_ImageHeight(0),
	m_NumChannels(0)
{
	if(bits_per_pixel == 32) m_NumChannels = 4;
	else if(bits_per_pixel == 24) m_NumChannels = 3;
	else m_NumChannels = 4;

	m_pStbImage = (unsigned char *)STBI_MALLOC(width * height * m_NumChannels);

	m_ImageWidth  = width;
	m_ImageHeight = height;
}


//BitmapImage::BitmapImage( int width, int height, int bpp, const RGBAColorType& color )
//:
//m_BitsPerPixel(bpp)
//{
//	m_pStbImage = FreeImage_Allocate( width, height, bpp );
//
//	FillColor( color );
//
//}


void BitmapImage::Release()
{
	if( m_pStbImage )
	{
		stbi_image_free( m_pStbImage );
		m_pStbImage = nullptr;
		m_ImageWidth = 0;
		m_ImageHeight = 0;
		m_NumChannels = 0;
	}
}


BitmapImage::BitmapImage( const BitmapImage& img )
	:
	m_pStbImage(nullptr),
	m_ImageWidth(img.m_ImageWidth),
	m_ImageHeight(img.m_ImageHeight),
	m_NumChannels(img.m_NumChannels)
{
	size_t alloc_size_in_bytes = img.m_ImageWidth * img.m_ImageHeight * img.m_NumChannels;

	m_pStbImage = (unsigned char *)STBI_MALLOC(alloc_size_in_bytes);
	memcpy( m_pStbImage, img.m_pStbImage, alloc_size_in_bytes );
}


// ----------------------------------------------------------


bool BitmapImage::LoadFromFile( const std::string& pathname, int flag )
{
	Release();

	//FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	// check the file signature and deduce its format
	// (the second argument is currently not used by FreeImage)
	//fif = FreeImage_GetFileType( pathname.c_str(), 0 );
	//if(fif == FIF_UNKNOWN)
	//{
	// no signature ?
	// try to guess the file format from the file extension
	//fif = FreeImage_GetFIFFromFilename( pathname.c_str() );
	//}

	//if(fif == FIF_UNKNOWN)
	//{
	//	LOG_PRINT_ERROR( " An unsupported image file format: " + pathname );
	//	return false;
	//}

	// check that the plugin has reading capabilities ...
	//if( !FreeImage_FIFSupportsReading(fif) )
	//{
	//	LOG_PRINT_ERROR( " FreeImage_FIFSupportsReading() failed on the following image file: " + pathname );
	//	return false;
	//	}

	// ok, let's load the file
	//m_pStbImage = FreeImage_Load(fif, pathname.c_str(), flag);
	int channels_in_file = 0;
	int desired_channels = 0;
	unsigned char *img = stbi_load( pathname.c_str(), &m_ImageWidth, &m_ImageHeight, &channels_in_file, desired_channels );

	// unless a bad file format, we are done !
	if( img )
	{
		m_pStbImage = img;
		m_NumChannels = channels_in_file;
		return true;
	}
	else
	{
		LOG_PRINT_ERROR( " Failed to load a file: " + pathname );
		return false;
	}
}


bool BitmapImage::SaveToFile( const std::string& pathname, int flag )
{
	if( !m_pStbImage )
	{
		LOG_PRINT_ERROR( " Has no valid bitmap." );
		return false;
	}

	// try to guess the file format from the file extension
	if( pathname.length() < 5 )
	{
		LOG_PRINT_ERROR( "The image file name should at least 5-character long." );
		return false;
	}

	int ret = 0;
	size_t last_dot_pos = pathname.rfind('.');
	if(last_dot_pos == std::string::npos)
		return false;

	std::string ext = pathname.substr(last_dot_pos);

	const int w = m_ImageWidth;
	const int h = m_ImageHeight;
	const int comp = m_NumChannels;

	int num_channels = m_NumChannels;
	if(ext == ".png")
	{
		// Comments in stb_image_write.h say,
		// For PNG, "stride_in_bytes" is the distance in bytes from the first byte of
		// a row of pixels to the first byte of the next row of pixels.
		const int stride_in_bytes = w * m_NumChannels;

		ret = stbi_write_png(pathname.c_str(), w, h, comp, m_pStbImage, stride_in_bytes );
	}
	else if(ext == ".jpg")
	{
		//if( 4 <= pathname.length()
		// && pathname.find(".jpg") == pathname.length() - 4
		// && bpp == 32 )
		//{
		//	// 32-bit (RGBA) images cannot be saved to JPEG files.
		//	// Give up the alpha and save the bitmap as a 24-bit (RGB) image file.
		//	LOG_PRINT_WARNING( " 32-bit images cannot be saved to JPEG files. Saving the image as 24-bit bitmap without the alpha channel: " + pathname );
		//	return Save32BPPImageTo24JPEGFile(pathname,flag);
		//}
		int quality = 80;
		ret = stbi_write_jpg( pathname.c_str(), w, h, comp, m_pStbImage, quality );
	}
	else
	{
		LOG_PRINT_ERROR( " An unsupported image format: " + pathname );
		return false;
	}
	//ret = stbi_write_bmp(pathname.c_str(), w, h, comp, const void *data);
	//ret = stbi_write_tga(pathname.c_str(), w, h, comp, const void *data);
	//ret = stbi_write_hdr(pathname.c_str(), w, h, comp, const float *data);

	if( ret == 1 )
	{
		LOG_PRINT_VERBOSE( " Saved an image to the file: " + pathname );
		return true;
	}
	else
	{
		LOG_PRINT_ERROR( " Failed to save an image to the file: " + pathname );
		return false;
	}
}


bool BitmapImage::CreateFromImageDataStream( stream_buffer& image_data, const std::string& image_format )
{
	LOG_PRINT_ERROR( "Not implemented." );
	return false;

	//	lock

//	std::vector<char>& mybuffer = image_data.buffer();

//	int sth = 0;

//	int flags = 0;
//	m_pStbImage = (unsigned char *)STBI_MALLOC(width * height * m_NumChannels);

//	if( m_pStbImage )
//	{
		//m_BitsPerPixel = FreeImage_GetBPP( m_pStbImage );
//		return true;
//	}
//	else
//	{
//		LOG_PRINT_ERROR( "xxx() returned nullptr. Image format: " + image_format );
//		return false;
//	}
}


bool BitmapImage::LoadFromMemory( const void *buffer, int size_in_bytes )
{
	LOG_PRINTF_VERBOSE(("Loading an image from memory. Size: %d",size_in_bytes));

	Release();

	int w = 0, h = 0;
	int channels_in_file = 0;
	int desired_channels = 4;
	unsigned char *img = stbi_load_from_memory( (unsigned char *)buffer, size_in_bytes, &w, &h, &channels_in_file, desired_channels );

	if(!img)
	{
		LOG_PRINTF_ERROR(("stbi_load_from_memory returned nullptr."));
		return false;
	}

	LOG_PRINTF_VERBOSE(("channels_in_file: %d, desired_channels (what is actually on memory): %d",channels_in_file,desired_channels));
	
	m_pStbImage   = img;
	m_ImageWidth  = w;
	m_ImageHeight = h;
	m_NumChannels = desired_channels;

	return true;
}


bool BitmapImage::Rescale( int dest_width, int dest_height/*, CImageFilter::Name filter */ )
{
	unsigned char *output_pixels = nullptr;
	if( m_pStbImage )
	{
		int num_strides_in_bytes = 0;
		int w = 0, h = 0;

		int ret = stbir_resize_uint8(
			m_pStbImage, // const unsigned char *input_pixels
			m_ImageWidth,
			m_ImageHeight,
			num_strides_in_bytes, // input_stride_in_bytes
			output_pixels,
			w,
			h,
			num_strides_in_bytes,
			m_NumChannels
		);
		return true;
	}
	else
		return false;
}


bool BitmapImage::FlipVertical()
{
	if( !m_pStbImage )
	{
		LOG_PRINT_ERROR("The source bitmap image is not valid.");
		return false;
	}

	int bytes_per_pixel = m_NumChannels;
	stbi__vertical_flip( m_pStbImage, m_ImageWidth, m_ImageHeight, bytes_per_pixel );
	//return res ? true : false;

	return true;
}


shared_ptr<BitmapImage> BitmapImage::CreateCopy() const
{
	if( !m_pStbImage )
	{
		LOG_PRINT_ERROR( "The source bitmap image is not valid." );
		return shared_ptr<BitmapImage>();
	}

	const int w = GetWidth();
	const int h = GetHeight();

	if( w <= 0 || h <= 0 )
	{
		LOG_PRINT_ERROR( " The source bitmap image does not have a width/height." );
		return shared_ptr<BitmapImage>();
	}

	BitmapImage *img = new BitmapImage( *this );
	if( !img )
	{
		LOG_PRINT_ERROR( " The new operator returned nullptr." );
		return shared_ptr<BitmapImage>();
	}

	shared_ptr<BitmapImage> pCopiedImage( img );
	return pCopiedImage;
}


shared_ptr<BitmapImage> BitmapImage::GetRescaled( int dest_width, int dest_height/*, CImageFilter::Name filter */ ) const
{
	if( dest_width <= 0 || dest_height <= 0 )
	{
		return shared_ptr<BitmapImage>();
	}

	if( m_pStbImage )
	{
		int num_strides_in_bytes = 0;
		unsigned char *dest = (unsigned char *)STBI_MALLOC(dest_width * dest_height * m_NumChannels);
		int ret = stbir_resize_uint8(
			m_pStbImage,
			m_ImageWidth,
			m_ImageHeight,
			num_strides_in_bytes,
			dest,
			dest_width,
			dest_height,
			num_strides_in_bytes,
			m_NumChannels
		);

		shared_ptr<BitmapImage> pScaledImage( new BitmapImage() );
		pScaledImage->m_pStbImage   = dest;
		pScaledImage->m_NumChannels = m_NumChannels;
		pScaledImage->m_ImageWidth  = dest_width;
		pScaledImage->m_ImageHeight = dest_height;
		return pScaledImage;
	}
	else
		return shared_ptr<BitmapImage>();
}


} // namespace amorphous
