#ifndef  __ImageArchive_H__
#define  __ImageArchive_H__


#include <vector>
#include <string>

#include "ImageStone.h"

#include "Support/fnop.h"
#include "Support/Log/DefaultLog.h"

#include "Support/Serialization/ArchiveObjectBase.h"
using namespace GameLib1::Serialization;


class CImageArchive : public IArchiveObjectBase
{
public:

	enum eImageFormat
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

	int m_Format;

	int m_Width;

	int m_Height;

	int m_Depth;

	std::vector<unsigned char> m_vecData;

public:

	inline CImageArchive();

	inline CImageArchive( const std::string& image_filename );

//	bool LoadImage_FloatRGBA( vector<SFloatRGBAColor>& dest_buffer );

	/// returns true on success
	inline bool SetFormatFromFileExtension( const std::string& image_ext );

	inline bool IsValid();

	inline virtual void Serialize( IArchive& ar, const unsigned int version );
};


inline CImageArchive::CImageArchive()
:
m_Format(IMGFMT_INVALID),
m_Width(0),
m_Height(0),
m_Depth(0)
{}


inline CImageArchive::CImageArchive( const std::string& image_filename )
:
m_Format(IMGFMT_INVALID),
m_Width(0),
m_Height(0),
m_Depth(0)
{
	// get the suffix from the filename
	size_t dot_pos = image_filename.rfind(".");
	if( dot_pos == std::string::npos )
		return;

	// set image format
	bool res = SetFormatFromFileExtension( image_filename.substr( dot_pos + 1, image_filename.length() ) );
	if( !res )
		return;

	// retrieve width and height
	FCObjImage src_img;
	if( !src_img.Load( image_filename.c_str() ) )
	{
		g_Log.Print( WL_ERROR, "cannot load image: %s", image_filename.c_str() );
		return;
	}

	m_Width  = src_img.Width();
	m_Height = src_img.Height();
	m_Depth  = 0;

	FILE *fp = fopen( image_filename.c_str(), "rb" );
	if( !fp )
		return;

	int img_size = fnop::get_filesize(image_filename);

	m_vecData.resize( img_size );

	fread( &m_vecData[0], img_size, 1, fp );

	fclose(fp);
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
	return (m_Format != IMGFMT_INVALID) && ( 0 < m_Width ) && ( 0 < m_Height ) && ( 0 < m_vecData.size() );
}


inline void CImageArchive::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_Format;
	ar & m_Width;
	ar & m_Height;
	ar & m_Depth;
	ar & m_vecData;
}


#endif  /*  __ImageArchive_H__  */
