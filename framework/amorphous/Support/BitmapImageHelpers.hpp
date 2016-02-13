#ifndef __amorphous_BitmapImageHelpers_HPP__
#define __amorphous_BitmapImageHelpers_HPP__


#include "BitmapImage.hpp"
#include "array2d.hpp"


namespace amorphous
{
using boost::shared_ptr;


//template<class RGBColorType>
//inline bool SaveToImageFile( const array2d<RGBColorType>& texel, const std::string& filepath )
//{
//	int x,y;
//	int width  = texel.size_x();
//	int height = texel.size_y();
//	const int depth = 24;
//
//	BitmapImage img( width, height, depth );
//
//	for( y=0; y<height ; y++ )
//	{
//		for( x=0; x<width; x++ )
//		{
//			img.SetPixel( x, y, texel(x,y) );
//		}
//	}
//
//	return img.SaveToFile( filepath );
//}


template<class RGBAColorType>
inline bool SaveToImageFile( const array2d<RGBAColorType>& texel, const std::string& filepath )
{
	int x,y;
	int width  = texel.size_x();
	int height = texel.size_y();
	const int depth = 32;

	BitmapImage img( width, height, depth );

	for( y=0; y<height ; y++ )
	{
		for( x=0; x<width; x++ )
		{
			img.SetPixel( x, y, texel(x,y) );
		}
	}

	return img.SaveToFile( filepath );
}


inline bool SaveGrayscaleToImageFile( const array2d<U8>& texel, const std::string& filepath )
{
	int x,y;
	int width  = texel.size_x();
	int height = texel.size_y();
	const int depth = 24;

	BitmapImage img( width, height, depth );

	for( y=0; y<height ; y++ )
	{
		for( x=0; x<width; x++ )
		{
			img.SetGrayscalePixel( x, y, texel(x,y) );
		}
	}

	return img.SaveToFile( filepath );
}


} // namespace amorphous


#endif /* __amorphous_BitmapImage_HPP__ */
