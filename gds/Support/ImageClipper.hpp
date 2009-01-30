#ifndef __ImageClipper_H__
#define __ImageClipper_H__


#include "Support/BitmapImage.hpp"
#include "Support/thread_starter.hpp"
#include "Graphics/Rect.hpp"
using namespace Graphics;


class CImageClipper : public thread_class
{
	SRect m_DestRegion;

	std::string m_DestFilepath;

	boost::shared_ptr<CBitmapImage> m_pBitmapImage;

public:

	CImageClipper( SRect& dest_region, const std::string& dest_filepath, boost::shared_ptr<CBitmapImage> pBitmapImage )
		:
	m_DestRegion(dest_region),
	m_DestFilepath(dest_filepath),
	m_pBitmapImage(pBitmapImage)
	{}

	virtual ~CImageClipper() {}

	inline void run();
};


//----------------------- inline implementations -----------------------

inline void CImageClipper::run()
{
	if( !m_pBitmapImage )
		return;

	// TODO: Add CBitmapImage::GetBPP()
	int bits_per_pixel = FreeImage_GetBPP( m_pBitmapImage->GetFBITMAP() );

	const int dest_w = m_DestRegion.GetWidth();
	const int dest_h = m_DestRegion.GetHeight();
	CBitmapImage img( dest_w, dest_h, bits_per_pixel );

	int src_height = m_pBitmapImage->GetHeight();

	int x, y;
	int sx = m_DestRegion.left;
	int sy = m_DestRegion.top;
	U8 r, g, b, a;
	a = 0xFF;
	RGBQUAD quad;
	for( x=0; x<dest_w; x++)
	{
		for( y=0; y<dest_h; y++)
		{
			// Error - fix this
//			m_pBitmapImage->GetPixel( sx + x, sy + y, r, g, b, a );

			FreeImage_GetPixelColor( m_pBitmapImage->GetFBITMAP(),
				sx + x,
				src_height - ( sy + y ) - 1,
				&quad );
			r = quad.rgbRed;
			g = quad.rgbGreen;
			b = quad.rgbBlue;

			// Error - fix this
//			img.SetPixel( x, y, r, g, b, a );

			FreeImage_SetPixelColor( img.GetFBITMAP(), x, dest_h - y - 1, &quad );
		}
	}

	img.SaveToFile( m_DestFilepath );
}


#endif  /*  __ImageClipper_H__  */
