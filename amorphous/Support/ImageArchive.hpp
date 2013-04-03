#ifndef  __ImageArchive_H__
#define  __ImageArchive_H__


#include <vector>
#include <string>
#include "SerializableStream.hpp"


namespace amorphous
{


/**
 - Creates an image archive from an image file
   - Uses an FreeImage library to retrieve the width and height of the image

*/
class ImageArchive : public SerializableStream
{
public:

	enum ImageFormat
	{
		IMGFMT_INVALID,
		IMGFMT_BMP24,
		IMGFMT_BMP32,
		IMGFMT_JPEG,
		IMGFMT_PNG,
		IMGFMT_TGA,
		IMGFMT_C8A8,
		IMGFMT_R32G32B32A32,
		IMGFMT_DDS,          ///< Direct Draw Surface
		NUM_IMAGE_FORAMTS
	};

public:

	ImageFormat m_Format;

public:

	inline ImageArchive();

	/// Creates an image archive from the given image file
	/// and stores the image to the internal buffer
	inline ImageArchive( const std::string& image_filename );

//	bool LoadImage_FloatRGBA( vector<SFloatRGBAColor>& dest_buffer );

	inline bool LoadFromFile( const std::string& image_filename );

	/// returns true on success
	inline bool SetFormatFromFileExtension( const std::string& image_ext );

	inline bool IsValid();

	inline virtual void Serialize( IArchive& ar, const unsigned int version );
};


// ============================ inline implementations ============================

inline ImageArchive::ImageArchive()
:
m_Format(IMGFMT_INVALID)
{}


inline ImageArchive::ImageArchive( const std::string& image_filename )
:
m_Format(IMGFMT_INVALID)
{
	LoadFromFile( image_filename );
}


inline bool ImageArchive::LoadFromFile( const std::string& image_filename )
{
	// get the suffix from the filename
	size_t dot_pos = image_filename.rfind(".");
	if( dot_pos == std::string::npos )
		return false;

	// set image format
	bool res = SetFormatFromFileExtension( image_filename.substr( dot_pos + 1, image_filename.length() ) );
	if( !res )
		return false;

	// load image data to buffer
	return SerializableStream::LoadBinaryStream(image_filename);
}


/// TODO: do non-case-sensitive comparison
inline bool ImageArchive::SetFormatFromFileExtension( const std::string& image_ext )
{
	if( image_ext == "bmp" )       m_Format = IMGFMT_BMP24; // TODO: support 32-bit image
	else if ( image_ext == "jpg" ) m_Format = IMGFMT_JPEG;
	else if ( image_ext == "tga" ) m_Format = IMGFMT_TGA;
	else if ( image_ext == "png" ) m_Format = IMGFMT_PNG;
	else if ( image_ext == "dds" ) m_Format = IMGFMT_DDS;
//	else if ( image_ext == "" )    m_Format = ;
//	else if ( image_ext == "" )    m_Format = ;
	else
	{
		m_Format = IMGFMT_INVALID;
		return false;
	}

	return true;
}


inline bool ImageArchive::IsValid()
{
	return (m_Format != IMGFMT_INVALID) && ( 0 < m_Buffer.buffer().size() );
}


inline void ImageArchive::Serialize( IArchive& ar, const unsigned int version )
{
	SerializableStream::Serialize( ar, version );

	ar & (int&)m_Format;

/*	if( version < 1 )
	{
		ar & m_Width;
		ar & m_Height;
		ar & m_Depth;

		std::vector<unsigned char> vecData;
		ar & vecData;
	}*/
}

} // namespace amorphous



#endif  /*  __ImageArchive_H__  */
