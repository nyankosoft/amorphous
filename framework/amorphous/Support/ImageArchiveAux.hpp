#ifndef  __amorphous_ImageArchive_HPP__
#define  __amorphous_ImageArchive_HPP__


#include "BitmapImage.hpp"
#include "ImageArchive.hpp"
#include "StringAux.hpp"


namespace amorphous
{


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


inline const char *ToImageExtension( ImageArchive::ImageFormat img_fmt )
{
	switch(img_fmt)
	{
	case ImageArchive::IMGFMT_PNG:   return "png";
	case ImageArchive::IMGFMT_JPEG:  return "jpg";
	case ImageArchive::IMGFMT_TGA:   return "tga";
	case ImageArchive::IMGFMT_BMP24: return "bmp";
	case ImageArchive::IMGFMT_BMP32: return "bmp";
//	case ImageArchive::IMGFMT_ : return FIF_DDS,
//	case ImageArchive::IMGFMT_ : return FIF_PPM,
//	case ImageArchive::IMGFMT_ : return FIF_DIB,
//	case ImageArchive::IMGFMT_ : return FIF_HDR,       ///< high dynamic range formats
//	case ImageArchive::IMGFMT_ : return FIF_PFM,       ///
	default:
		LOG_PRINTF_ERROR(( "An unsupported image format: %d", (int)img_fmt ));
		return "";
	}

	return "";
}


inline bool LoadBitmapImageFromImageArchive( ImageArchive& src_img_archive, BitmapImage& dest_img )
{
	const char *image_format = ToImageExtension(src_img_archive.m_Format);

	if( strlen(image_format) == 0 )
		return false;

	bool res = dest_img.CreateFromImageDataStream( src_img_archive.m_Buffer, image_format );

	if( !res )
		return false;

	return true;
}


inline boost::shared_ptr<BitmapImage> CreateBitmapImageFromImageArchive( ImageArchive& src_img_archive )
{
	using boost::shared_ptr;

	shared_ptr<BitmapImage> pImage( new BitmapImage );

	bool res = LoadBitmapImageFromImageArchive( src_img_archive, *pImage );

	if( !res )
		return shared_ptr<BitmapImage>();

	return pImage;
}


} // namespace amorphous


#endif  /*  __amorphous_ImageArchive_HPP__  */
