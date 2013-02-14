#ifndef __ImageSplitter_H__
#define __ImageSplitter_H__


#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include "BitmapImage.hpp"
#include "lfs.hpp"


namespace amorphous
{


class TBBImageSplitterImpl;


class SplitImageFilepathPrinter
{
public:

	virtual std::string Print( int index ) = 0;
};


class StdSplitImageFilepathPrinter : public SplitImageFilepathPrinter
{
	std::string m_SrcFilepath;

public:

	StdSplitImageFilepathPrinter( std::string& src_filepath )
		:
	m_SrcFilepath(src_filepath)
	{}

	std::string Print( int index )
	{
		std::string dest = m_SrcFilepath;
		lfs::insert_before_extension( dest, fmt_string( "_%03d", index ) );
		return dest;
	}
};


class ImageSplitter
{
	int m_NumSplitsX;
	int m_NumSplitsY;

	boost::shared_ptr<CBitmapImage> m_pBitmapImage;

	std::string m_BaseDestFilepath;

	SplitImageFilepathPrinter *m_pSplitImageFilepathPrinter;

	inline void Clip( int index ) const;

public:

	ImageSplitter(
		int num_splits_x,
		int num_splits_y,
		const std::string& base_dest_filepath,
		const std::string& src_image_filepath,
		SplitImageFilepathPrinter *pDestFilepathPrinter )
		:
	m_NumSplitsX(num_splits_x),
	m_NumSplitsY(num_splits_y),
	m_BaseDestFilepath(base_dest_filepath),
	m_pSplitImageFilepathPrinter(pDestFilepathPrinter)
	{
		m_pBitmapImage.reset( new CBitmapImage() );
		bool loaded = m_pBitmapImage->LoadFromFile( src_image_filepath );
		if( !loaded )
		{
			LOG_PRINT_ERROR( "Cannot load the source image file: " + src_image_filepath );
		}
	}

	ImageSplitter(
		int num_splits_x,
		int num_splits_y,
		const std::string& base_dest_filepath,
		boost::shared_ptr<CBitmapImage> pBitmapImage,
		SplitImageFilepathPrinter *pDestFilepathPrinter )
		:
	m_NumSplitsX(num_splits_x),
	m_NumSplitsY(num_splits_y),
	m_BaseDestFilepath(base_dest_filepath),
	m_pBitmapImage(pBitmapImage),
	m_pSplitImageFilepathPrinter(pDestFilepathPrinter)
	{
	}

	virtual ~ImageSplitter() {}

//	inline void operator() ( const tbb::blocked_range<int>& r ) const;

	std::string CreateSubdividedTextureFilepath( int index );

	inline void SplitMT();

	inline void SplitST();

	/// TBBImageSplitterImpl needs to call ImageSplitter::Clip()
	friend class TBBImageSplitterImpl;
};


class TBBImageSplitterImpl
{
	const ImageSplitter *m_pSplitter;

public:

	TBBImageSplitterImpl( const ImageSplitter *pSplitter )
		:
	m_pSplitter(pSplitter)
	{}

	inline void operator() ( const tbb::blocked_range<int>& r ) const;
};

//----------------------- inline implementations -----------------------


inline void ImageSplitter::Clip( int index ) const
{
	if( !m_pBitmapImage )
		return;

	int dest_tex_width  = m_pBitmapImage->GetWidth()  / m_NumSplitsX;
	int dest_tex_height = m_pBitmapImage->GetHeight() / m_NumSplitsY;

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


inline void ImageSplitter::SplitMT()
{
	tbb::parallel_for( tbb::blocked_range<int>(
		0,
		m_NumSplitsX * m_NumSplitsY
		/*grainsize*/ ),
		TBBImageSplitterImpl(this) );
}


inline void ImageSplitter::SplitST()
{
	const int num_images = m_NumSplitsX * m_NumSplitsY;
	for( int i=0; i<num_images; i++ )
	{
		Clip( i );
	}
}


inline void TBBImageSplitterImpl::operator() ( const tbb::blocked_range<int>& r ) const
{
	for( int I = r.begin(); I != r.end(); ++I )
	{
		m_pSplitter->Clip( I );
	}
}

} // amorphous



#endif  /*  __ImageSplitter_H__  */
