#ifndef  __TextFileScannerExtensions_HPP__
#define  __TextFileScannerExtensions_HPP__


#include "gds/Support/TextFileScanner.hpp"
#include "gds/3DMath/Vector2.hpp"
#include "gds/3DMath/Vector3.hpp"
#include "gds/Graphics/FloatRGBColor.hpp"
#include "gds/Graphics/FloatRGBAColor.hpp"
#include "gds/Graphics/Rect.hpp"
using namespace Graphics;


inline void conv_to_x( std::vector<std::string>& src, int& index, Vector2& dest )
{
	if( 2 <= (int)src.size() - index )
	{
		dest.x = (float)atof( src[index].c_str() );
		dest.y = (float)atof( src[index+1].c_str() );
		index += 2;
	}
}


inline void conv_to_x( std::vector<std::string>& src, int& index, Vector3& dest )
{
	if( 3 <= (int)src.size() - index )
	{
		dest.x = (float)atof( src[index].c_str() );
		dest.y = (float)atof( src[index+1].c_str() );
		dest.z = (float)atof( src[index+2].c_str() );
		index += 3;
	}
}


inline void conv_to_x( std::vector<std::string>& src, int& index, SFloatRGBColor& dest )
{
	if( 3 <= (int)src.size() - index )
	{
		dest.red   = (float)atof( src[index].c_str() );
		dest.green = (float)atof( src[index+1].c_str() );
		dest.blue  = (float)atof( src[index+2].c_str() );
		index += 3;
	}
}


inline void conv_to_x( std::vector<std::string>& src, int& index, SFloatRGBAColor& dest )
{
	if( 4 <= (int)src.size() - index )
	{
		dest.red   = (float)atof( src[index].c_str() );
		dest.green = (float)atof( src[index+1].c_str() );
		dest.blue  = (float)atof( src[index+2].c_str() );
		dest.alpha = (float)atof( src[index+3].c_str() );
		index += 4;
	}
}


inline void conv_to_x( std::vector<std::string>& src, int& index, SRect& dest )
{
	if( 4 <= (int)src.size() - index )
	{
		dest.left   = atoi( src[index].c_str() );
		dest.top    = atoi( src[index+1].c_str() );
		dest.right  = atoi( src[index+2].c_str() );
		dest.bottom = atoi( src[index+3].c_str() );
		index += 4;
	}
}



#endif		/*  __TextFileScannerExtensions_HPP__  */
