#ifndef __BuiltinFonts_HPP__
#define __BuiltinFonts_HPP__


#include <string>
#include <boost/shared_ptr.hpp>
#include "BitstreamVeraSansMono_Bold_256.hpp"
#include "TextureFont.hpp"


namespace amorphous
{


inline const SimpleBitmapFontData *GetBuiltinFontData( const std::string& builtin_font_name )
{
	if( builtin_font_name == "BitstreamVeraSansMono-Bold-256" )
	{
		return &g_BitstreamVeraSansMono_Bold_256;
	}
	else
		return NULL;
}


inline const SimpleBitmapFontData *GetDefaultBuitinFontData()
{
	return GetBuiltinFontData( "BitstreamVeraSansMono-Bold-256" );
}


inline TextureFont* CreateDefaultBuiltinFontRawPtr()
{
	TextureFont *pTexFont = new TextureFont;
	pTexFont->InitFont( GetDefaultBuitinFontData() );
	pTexFont->SetFontSize( 6, 12 );
	return pTexFont;
}


inline boost::shared_ptr<TextureFont> CreateDefaultBuiltinFont()
{
	boost::shared_ptr<TextureFont> pTexFont( CreateDefaultBuiltinFontRawPtr() );
	return pTexFont;
}

} // namespace amorphous



#endif /* __BuiltinFonts_HPP__ */
