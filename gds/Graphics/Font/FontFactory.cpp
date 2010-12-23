#include "FontFactory.hpp"
#include "TextureFont.hpp"
#include "TrueTypeTextureFont.hpp"
#include "UTFFont.hpp"
#include "BuiltinFonts.hpp"
#include "../../Support/SafeDelete.hpp"

using namespace std;


CFontBase* CFontFactory::CreateFontRawPtr( CFontBase::FontType type )
{
	switch( type )
	{
	case CFontBase::FONTTYPE_TEXTURE:
		return new CTextureFont;
	case CFontBase::FONTTYPE_TRUETYPETEXTURE:
		return new CTrueTypeTextureFont;
	case CFontBase::FONTTYPE_UTF:
		return new CUTFFont;
//	case CFontBase::FONTTYPE_DIRECT3D:
//		return new CD3DFont;
	default:
		LOG_PRINT_ERROR( " An unsupported font type." );
		return NULL;
	}
}


CFontBase* CFontFactory::CreateFontRawPtr( const std::string& font_name, int font_width, int font_height )
{
	if( font_name.find(".ia")  != string::npos
	 || font_name.find(".dds") != string::npos
	 || font_name.find(".bmp") != string::npos
	 || font_name.find(".tga") != string::npos
	 || font_name.find(".jpg") != string::npos )
	{
		return new CTextureFont( font_name, font_width, font_height );
	}
	else if( font_name.find( "BuiltinFont::" ) == 0 )
	{
		const string builtin_font_name = font_name.substr( strlen("BuiltinFont::") );
		CTextureFont *pFont = new CTextureFont;
		bool initialized = pFont->InitFont( GetBuiltinFontData( builtin_font_name ) );

		if( initialized )
		{
			pFont->SetFontSize( font_width, font_height );
			return pFont;
		}
		else
		{
			SafeDelete( pFont );
			return NULL;
		}
	}
	else if( 4 < font_name.length() )
	{
		const string dot_and_ext = font_name.substr(font_name.length() - 4);
		if( dot_and_ext == ".ttf"
		 && dot_and_ext == ".otf" )
		{
			// Consider font_name as a filename
			CUTFFont *pUTFFont = new CUTFFont;
			pUTFFont->InitFont( font_name );
			return pUTFFont;
		}
		else
			return NULL;
	}
	else
		return NULL;

//	CTextureFont *pTexFont = new CTextureFont;
//	pTexFont->InitFont( font_name, 16, 32 );
//	return pTexFont;

	return NULL;
}


CFontBase* CFontFactory::CreateFontRawPtr( CFontBase::FontType type, const string& font_name, int font_width, int font_height )
{
	LOG_PRINT_ERROR( " Not implemented." );
	return NULL;
}
