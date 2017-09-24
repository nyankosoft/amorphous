#include "FontFactory.hpp"
#include "TextureFont.hpp"
#include "ASCIIFont.hpp"
#include "UTFFont.hpp"
#include "BuiltinFonts.hpp"
#include "amorphous/Support/SafeDelete.hpp"


namespace amorphous
{

using namespace std;


FontBase* FontFactory::CreateFontRawPtr( FontBase::FontType type )
{
	switch( type )
	{
	case FontBase::FONTTYPE_TEXTURE:
		return new TextureFont;
	case FontBase::FONTTYPE_TRUETYPETEXTURE:
		return new TrueTypeTextureFont;
	case FontBase::FONTTYPE_UTF:
		return new UTFFont;
//	case FontBase::FONTTYPE_DIRECT3D:
//		return new CD3DFont;
	default:
		LOG_PRINT_ERROR( " An unsupported font type." );
		return nullptr;
	}
}


FontBase* FontFactory::CreateFontRawPtr( const std::string& font_name, int font_width, int font_height )
{
	string ext;
	size_t pos = font_name.rfind( "." );
	if( pos != string::npos && 4 <= font_name.length() && pos <= font_name.length() - 2 )
		ext = font_name.substr( pos + 1 );

	if( ext == "ia"
	 || ext == "png"
	 || ext == "dds"
	 || ext == "bmp"
	 || ext == "tga"
	 || ext == "jpg" )
	{
		return new TextureFont( font_name, font_width, font_height );
	}
	else if( ext == TrueTypeTextureFont::GetTextureFontArchiveExtension() )
	{
		return new TrueTypeTextureFont( font_name, font_width, font_height );
	}
	else if( font_name.find( "BuiltinFont::" ) == 0 )
	{
		const string builtin_font_name = font_name.substr( strlen("BuiltinFont::") );
		TextureFont *pFont = new TextureFont;
		bool initialized = pFont->InitFont( GetBuiltinFontData( builtin_font_name ) );

		if( initialized )
		{
			pFont->SetFontSize( font_width, font_height );
			return pFont;
		}
		else
		{
			SafeDelete( pFont );
			return nullptr;
		}
	}
	else if( 4 < font_name.length() )
	{
		const string dot_and_ext = font_name.substr(font_name.length() - 4);
		if( dot_and_ext == ".ttf"
		 || dot_and_ext == ".otf" )
		{
			// Consider font_name as a filename
			UTFFont *pUTFFont = new UTFFont;
			pUTFFont->InitFont( font_name );
			return pUTFFont;
		}
		else
			return nullptr;
	}
	else
		return nullptr;

//	TextureFont *pTexFont = new TextureFont;
//	pTexFont->InitFont( font_name, 16, 32 );
//	return pTexFont;

	return nullptr;
}


FontBase* FontFactory::CreateFontRawPtr( FontBase::FontType type, const string& font_name, int font_width, int font_height )
{
	LOG_PRINT_ERROR( " Not implemented." );
	return nullptr;
}


} // namespace amorphous
