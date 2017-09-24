#ifndef __BuiltinImage_HPP__
#define __BuiltinImage_HPP__


#include "BitmapImage.hpp"
#include "ImageArchiveAux.hpp"


namespace amorphous
{


class CBuiltinImage
{
public:
	const char *format;
	const int width;
	const int height;
	const U32 *data;
	const uint data_size;

	// Commented out because the following compiler error occurs on vc10 express
	// when the default ctor is defined:
	// "non-aggregates cannot be initialized with initializer list"
//	CBuiltinImage() : format(""), width(0), height(0), data(NULL), data_size(0) {}

	bool IsValid() const
	{
		if( format
		 && 0 < strlen(format)
		 && 0 < width
		 && 0 < height
		 && data
		 && 0 < data_size )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};


inline bool LoadBuiltinImage( const CBuiltinImage& src, BitmapImage& dest )
{
	// Create an image archive from the specified builtin image.
	ImageArchive img_archive;
	img_archive.SetFormatFromFileExtension( src.format );
	img_archive.m_Buffer.buffer().resize( src.data_size );
	memcpy( &(img_archive.m_Buffer.buffer()[0]), src.data, src.data_size );

	return LoadBitmapImageFromImageArchive( img_archive, dest );
}


inline std::shared_ptr<BitmapImage> LoadBuiltinImage( const CBuiltinImage& src )
{
	std::shared_ptr<BitmapImage> pBitmapImage( new BitmapImage );
	bool loaded = LoadBuiltinImage( src, *pBitmapImage );
	if( loaded )
		return pBitmapImage;
	else
		return std::shared_ptr<BitmapImage>();
}


} // namespace amorphous



#endif /* __BuiltinImage_HPP__ */
