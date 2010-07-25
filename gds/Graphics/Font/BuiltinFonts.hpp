#ifndef __BuiltinFonts_HPP__
#define __BuiltinFonts_HPP__


#include <string>
#include <boost/shared_ptr.hpp>
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


inline const CSimpleBitmapFontData *GetDefaultBuitinFontData()
{
	return GetBuiltinFontData( "BitstreamVeraSansMono-Bold-256" );
}


inline CTextureFont* CreateDefaultBuiltinFontRawPtr()
{
	CTextureFont *pTexFont = new CTextureFont;
	pTexFont->InitFont( GetDefaultBuitinFontData() );
	pTexFont->SetFontSize( 6, 12 );
	return pTexFont;
}


inline boost::shared_ptr<CTextureFont> CreateDefaultBuiltinFont()
{
	boost::shared_ptr<CTextureFont> pTexFont( CreateDefaultBuiltinFontRawPtr() );
	return pTexFont;
}


#endif /* __BuiltinFonts_HPP__ */
