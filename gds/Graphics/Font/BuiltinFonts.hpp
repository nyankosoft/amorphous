#ifndef __BuiltinFonts_HPP__
#define __BuiltinFonts_HPP__


#include <string>
#include "BitstreamVeraSansMono_Bold_256.hpp"


inline const CSimpleBitmapFontData *GetBuiltinFontData( const std::string& builtin_font_name )
{
	if( builtin_font_name == "BitstreamVeraSansMono-Bold-256" )
	{
		return &g_BitstreamVeraSansMono_Bold_256;
	}
	else
		return NULL;
}


#endif /* __BuiltinFonts_HPP__ */
