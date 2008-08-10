#ifndef  __ImageArchive_H__
#define  __ImageArchive_H__


#include <vector>
#include <string>

#include "Support/stream_buffer.h"
#include "Support/fnop.h"
//#include "Support/Log/DefaultLog.h"


/**
 - Creates an image archive from an image file
   - Uses an FreeImage library to retrieve the width and height of the image

*/
class CImageArchive : public stream_buffer
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
		NUM_IMAGE_FORAMTS
	};

public:

	ImageFormat m_Format;

public:

	inline CImageArchive();

	/// Creates an image archive from the given image file
	/// and stores the image to the internal buffer
	inline CImageArchive( const std::string& image_filename );

//	bool LoadImage_FloatRGBA( vector<SFloatRGBAColor>& dest_buffer );

	/// returns true on success
	inline bool SetFormatFromFileExtension( const std::string& image_ext );

	inline bool IsValid();

	inline virtual void Serialize( IArchive& ar, const unsigned int version );
};


// ============================ inline implementations ============================

inline CImageArchive::CImageArchive()
:
m_Format(IMGFMT_INVALID)
{}


inline CImageArchive::CImageArchive( const std::string& image_filename )
:
m_Format(IMGFMT_INVALID)
{
	// get the suffix from the filename
	size_t dot_pos = image_filename.rfind(".");
	if( dot_pos == std::string::npos )
		return;

	// set image format
	bool res = SetFormatFromFileExtension( image_filename.substr( dot_pos + 1, image_filename.length() ) );
	if( !res )
		return;

	// load image data to buffer
	stream_buffer::LoadBinaryStream(image_filename);
}


/// TODO: do non-case-sensitive comparison
inline bool CImageArchive::SetFormatFromFileExtension( const std::string& image_ext )
{
	if( image_ext == "bmp" )       m_Format = IMGFMT_BMP24; // support 32-bit image
	else if ( image_ext == "jpg" ) m_Format = IMGFMT_JPEG;
	else if ( image_ext == "tga" ) m_Format = IMGFMT_TGA;
	else if ( image_ext == "png" ) m_Format = IMGFMT_PNG;
//	else if ( image_ext == "" )    m_Format = ;
//	else if ( image_ext == "" )    m_Format = ;
//	else if ( image_ext == "" )    m_Format = ;
	else
	{
		m_Format = IMGFMT_INVALID;
		return false;
	}

	return true;
}


inline bool CImageArchive::IsValid()
{
	return (m_Format != IMGFMT_INVALID) && ( 0 < m_buffer.size() );
}


inline void CImageArchive::Serialize( IArchive& ar, const unsigned int version )
{
	stream_buffer::Serialize( ar, version );

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


#endif  /*  __ImageArchive_H__  */
