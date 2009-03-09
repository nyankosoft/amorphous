#ifndef __ImageSplitter_H__
#define __ImageSplitter_H__


#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include "Support/BitmapImage.hpp"
#include "Graphics/Rect.hpp"
using namespace Graphics;


class CSplitImageFilepathPrinter
{
public:

	virtual std::string Print( int index ) = 0;
};


// Use tbb?
class CImageSplitter
{
	int m_NumSplitsX;
	int m_NumSplitsY;

	int m_DestImageWidth;
	int m_DestImageHeight;

	boost::shared_ptr<CBitmapImage> m_pBitmapImage;

	std::string m_BaseDestFilepath;

	CSplitImageFilepathPrinter *m_pSplitImageFilepathPrinter;

public:

	CImageSplitter(
		int num_splits_x,
		int num_splits_y,
		int dest_image_width,
		int dest_image_height,
		const std::string& base_dest_filepath,
		boost::shared_ptr<CBitmapImage> pBitmapImage,
		CSplitImageFilepathPrinter *pDestFilepathPrinter )
		:
	m_NumSplitsX(num_splits_x),
	m_NumSplitsY(num_splits_y),
	m_DestImageWidth(dest_image_width),
	m_DestImageHeight(dest_image_height),
	m_BaseDestFilepath(base_dest_filepath),
	m_pBitmapImage(pBitmapImage),
	m_pSplitImageFilepathPrinter(pDestFilepathPrinter)
	{}

	virtual ~CImageSplitter() {}

	inline void operator() ( const tbb::blocked_range<int>& r ) const;

	std::string CreateSubdividedTextureFilepath( int index );

	inline void Clip( int index ) const;
};


class CSGSplitTextureFilepathPrinter : public CSplitImageFilepathPrinter
{
	std::string m_SrcFilepath;

public:

	CSGSplitTextureFilepathPrinter( std::string& src_filepath )
		:
	m_SrcFilepath(src_filepath)
	{}

	virtual std::string Print( int index )
	{
		char dest[1024];
		sprintf( dest, "%s%03d", m_SrcFilepath.c_str(), index );
		return string(dest);
	}
};

//----------------------- inline implementations -----------------------


inline void CImageSplitter::Clip( int index ) const
{
	int dest_tex_width  = m_DestImageWidth;
	int dest_tex_height = m_DestImageHeight;

	int num_color_bits = 24;

	CBitmapImage dest_img( dest_tex_width, dest_tex_height, num_color_bits );
	U8 r=0,g=0,b=0,a=0;
	int x,y,offset_x,offset_y;

	offset_x = (index % m_NumSplitsX) * dest_tex_width;
	offset_y = (index / m_NumSplitsY) * dest_tex_height;
	for( y=0; y<dest_tex_width; y++ )
	{
		for( x=0; x<dest_tex_width; x++ )
		{
			m_pBitmapImage->GetPixel( offset_x + x, offset_y + y, r, g, b );
			dest_img.SetPixel( x, y, r, g, b );
		}
	}

//	g_Log.Print( "saving image: %s", dest_filename.c_str() );

	// save the split texture into file
	dest_img.SaveToFile( m_pSplitImageFilepathPrinter->Print( index ) );

	// save output texture filename
//		m_vecOutputTextureFilename.push_back( dest_filename );
}


inline void CImageSplitter::operator() ( const tbb::blocked_range<int>& r ) const
{
	for( int I = r.begin(); I != r.end(); ++I )
	{
		Clip( I );
	}
}


#endif  /*  __ImageSplitter_H__  */
