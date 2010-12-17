#ifndef __BuiltinImage_HPP__
#define __BuiltinImage_HPP__


#include "BitmapImage.hpp"


class CBuiltinImage
{
public:
	const char *format;
	const int width;
	const int height;
	const U32 *data;
	const uint data_size;

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


inline bool LoadBuiltinImage( const CBuiltinImage& src, CBitmapImage& dest )
{
	// Create an image archive from the specified builtin image.
	CImageArchive img_archive;
	img_archive.SetFormatFromFileExtension( src.format );
	img_archive.m_Buffer.buffer().resize( src.data_size );
	memcpy( &(img_archive.m_Buffer.buffer()[0]), src.data, src.data_size );

	return dest.CreateFromImageArchive( img_archive );
}


inline boost::shared_ptr<CBitmapImage> LoadBuiltinImage( const CBuiltinImage& src )
{
	boost::shared_ptr<CBitmapImage> pBitmapImage( new CBitmapImage );
	bool loaded = LoadBuiltinImage( src, *pBitmapImage );
	if( loaded )
		return pBitmapImage;
	else
		return boost::shared_ptr<CBitmapImage>();
}



#endif /* __BuiltinImage_HPP__ */
