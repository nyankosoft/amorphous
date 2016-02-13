#include "UTFFont.hpp"
#include "../TextureGenerators/TextureFillingAlgorithm.hpp"
#include "../Rect.hpp"
#include "Support/FreeTypeAux.hpp"
#include "Support/Profile.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/UTF8/utf8.h"
//#include "amorphous/App/GameWindowManager.hpp"


namespace amorphous
{

using std::string;
using std::vector;
using boost::shared_ptr;

//typedef TextureFont ASCIIFont;


template<class dest_buffer_type, typename BitmapCalc>
inline void DrawBitmap_Gen( FT_Bitmap *bitmap, int left, int top, dest_buffer_type& dest_buffer )
{
	int x,y;//,r;
//	int index;
	int height = bitmap->rows;
	int width = bitmap->width;
	U8 *pBuffer = (U8 *)bitmap->buffer;
	U8 grayscale = 0;
	for( y=0; y<height ; y++ )
	{
		for( x=0; x<width; x++ )
		{
//			dest_buffer.SetGrayscalePixel( left + x, top + y ) = BitmapCalc::CalcPixel( x, y, width, height, pBuffer );
			grayscale = BitmapCalc::CalcPixel( x, y, width, height, pBuffer );
//			U32 argb32 = 0xFF000000 | (grayscale << 16) | (grayscale << 8) | grayscale;
			U32 argb32 = 0x00FFFFFF | (grayscale << 24);
			dest_buffer.SetPixelARGB32( left + x, top + y, argb32 );
		}
	}
}


template<class dest_buffer_type>
inline void DrawBitmap( FT_Bitmap *bitmap, int left, int top, dest_buffer_type& dest_buffer )
{
	switch( bitmap->pixel_mode )
	{
	case FT_PIXEL_MODE_MONO:
		DrawBitmap_Gen<dest_buffer_type,MonoBitmapCalc>( bitmap, left, top, dest_buffer );
		break;
	case FT_PIXEL_MODE_GRAY:
		DrawBitmap_Gen<dest_buffer_type,GrayBitmapCalc>( bitmap, left, top, dest_buffer );
		break;

	default:
		break;
	}
}


/*
class render_target_of_freetype_bitmap
{
	void SetGrayscalePixel( int x, int y, U8 pixel );
public:
};

class texture_render_target_of_freetype_bitmap
{
	LockedTexture* m_pLockedTex;

	void SetGrayscalePixel( int x, int y, U8 pixel )
	{
		m_pLockedTex->SetGrayscalePixel( x, y, pixel );
	}
};

*/


//=========================================================================================
// UTFFont
//=========================================================================================

UTFFont::UTFFont()
{
	InitUTFFontInternal();
}


UTFFont::UTFFont( const std::string& filename, int font_pt, int resolution )

{
	InitUTFFontInternal();
	InitFont( filename, font_pt, resolution );
}


UTFFont::~UTFFont()
{
	Release();
}


void UTFFont::InitUTFFontInternal()
{
//	ASCIIFont::InitInternal();
	TextureFont::InitInternal();

//	m_BaseHeight = 64;

//	m_pTextureLoader = shared_ptr<FontTextureLoader>( new FontTextureLoader(this) );

	m_pFreeTypeLibrary.reset( new FreeTypeLibrary );

	m_Face = NULL;
}


static void RenderTextToBufferAndSetUpRects(
	                    FT_Face& face,
						const std::vector<U32> &utf8_code_points,
						int char_height,
//						array2d<U8>& dest_bitmap_buffer,
						LockedTexture& dest_texture,
//						vector<UTFFont::CharRect>& char_rects,
						C2DRectSet& text_boxes,
						int start_index,
						const Vector2& vTopLeftPos
						)
{
	PROFILE_FUNCTION();
//	LOG_FUNCTION_SCOPE();

	const vector<U32>& text = utf8_code_points;

	float top, left, bottom, right;
	int _top, _left, _bottom, _right;
	FT_GlyphSlot slot = face->glyph; // a small shortcut
//	FT_UInt glyph_index;
	int pen_x, pen_y, n;

	const int num_chars = (int)text.size();

	int error = 0;

//	char_rects.resize( num_chars );

	if( text_boxes.GetNumRects() < start_index + num_chars )
		text_boxes.AddRects( start_index + num_chars - text_boxes.GetNumRects() );

	int img_width  = dest_texture.GetWidth();
	int img_height = dest_texture.GetHeight();
//	int img_width = 1024;
//	dest_bitmap_buffer.resize( img_width, img_width, 0 );

	const int line_height = char_height;

	const int sx = 0;
	const int sy = char_height;
	pen_x = sx;
	pen_y = sy;
	int margin = 4;
	float sum_advance = 0;
	unsigned int row = 0;
	for ( n = 0; n < num_chars; n++ )
	{
		if( text[n] == '\n' )
		{
			// line feed
			sum_advance = 0;
			row += 1;
			continue;
		}

		/* load glyph image into the slot (erase previous one) */
		error = FT_Load_Char( face, text[n], FT_LOAD_RENDER );

		if ( error )
			continue;

		if( slot->bitmap_left < 0 )
		{
			// some chars have negative left offset
			// - add the offset to make sure that x coords of their bounding box 
			//   do not overlap with those of other chars
			pen_x += (int)slot->bitmap_left * (-1);
		}

		// ignore errors

		_left = pen_x + slot->bitmap_left;
		_top  = pen_y - slot->bitmap_top;
		clamp( _top, 0, 4096 );

		left = (float)pen_x / (float)img_width;
		top  = (float)(pen_y - slot->bitmap_top) / (float)img_height;


		// now, draw to our target surface
		DrawBitmap<LockedTexture>( &slot->bitmap, pen_x + slot->bitmap_left, pen_y - slot->bitmap_top, dest_texture );

		// increment pen position
		pen_x += slot->bitmap_left + slot->bitmap.width;//slot->advance.x >> 6;

		right  = (float)pen_x / (float)img_width;
		bottom = (float)pen_y / (float)img_height;

		_right  = _left + slot->bitmap.width;
		_bottom = _top  + slot->bitmap.rows;

		pen_x += margin;

		if( img_width - char_height * 2 < pen_x )
		{
			// line feed
			pen_x = sx;
			pen_y += (int)(char_height * 1.5f); // Not sure if x1.5 is always sufficient
		}

//		DrawRect( dest_bitmap_buffer, RectLTRB( _left, _top, _right, _bottom ), 0x90 );

		UTFFont::CharRect char_rect;
//		UTFFont::CharRect& char_rect = char_rects[n];

		char_rect.tex_min = TEXCOORD2( (float)_left  / (float)img_width, (float)_top    / (float)img_height );
		char_rect.tex_max = TEXCOORD2( (float)_right / (float)img_width, (float)_bottom / (float)img_height );

		int top = char_height - slot->bitmap_top + row * line_height;
		char_rect.rect.vMin = Vector2( (float)slot->bitmap_left,                      (float)top );
		char_rect.rect.vMax = Vector2( (float)slot->bitmap_left + slot->bitmap.width, (float)top + slot->bitmap.rows );

		char_rect.advance = (float)(slot->advance.x >> 6);

		char_rect.rect.vMin.x += sum_advance;
		char_rect.rect.vMax.x += sum_advance;

		sum_advance += char_rect.advance;

		int rect_index = n;
//		text_boxes.SetRectVertexPosition( rect_index, 0, Vector2( sx + italic, sy ) );
//		text_boxes.SetRectVertexPosition( rect_index, 1, Vector2( ex + italic, sy ) );
//		text_boxes.SetRectVertexPosition( rect_index, 2, Vector2( ex,          ey ) );
//		text_boxes.SetRectVertexPosition( rect_index, 3, Vector2( sx,          ey ) );
		text_boxes.SetRectMinMax(
			rect_index,
			char_rect.rect.vMin + vTopLeftPos,
			char_rect.rect.vMax + vTopLeftPos
			);

//		text_boxes.SetTextureCoordMinMax( rect_index, su, sv, eu, ev );
		text_boxes.SetTextureCoordMinMax( rect_index, char_rect.tex_min, char_rect.tex_max );
///		text_boxes.SetTextureCoordMinMax( rect_index, TEXCOORD2(0,0), TEXCOORD2(1,1) );
	}
}


bool UTFFont::InitFont( const std::string& font_file_path, int font_pt, int resolution )
{
	int font_height = font_pt;
	int font_width  = font_pt; // not used

	if( font_file_path.length() <= 4 )
	{
		// The font file name is supposed to be "*.ttf" or "*.otf"
		return false;
	}

//	SetFontSize( font_width, font_height );

	string dot_and_3char_suffix = font_file_path.substr( font_file_path.length() - 4 );

	FreeTypeLibrary& ftlib = *m_pFreeTypeLibrary;

	FT_Face& face = m_Face;

	int error = FT_New_Face( ftlib.GetFTLibrary(), font_file_path.c_str(), 0, &face );

	if( error == FT_Err_Unknown_File_Format )
	{
		LOG_PRINT_ERROR( fmt_string("The font file '%s' could be opened and read, but it appears that its font format is unsupported.",font_file_path.c_str()) );
		return false;
	}
	else if( error )
	{
		LOG_PRINT_ERROR( fmt_string("The font file '%s' could not be opened or read, or simply that it is broken.",font_file_path.c_str()) );
		return false;
	}

	SetFontSize( font_width, font_height );

	// TODO: support non-square texture sizes for the OpenGL mode
	TextureResourceDesc tex;
	tex.Width  = 1024;
	tex.Height = 512;//(GetGraphicsLibraryName() == "OpenGL") ? 1024 : 512;
	tex.Format = TextureFormat::A8R8G8B8;
	tex.MipLevels = 0;
	bool loaded = m_FontTexture.Load( tex );

//	return true;
	return loaded;
}


bool UTFFont::DrawTextToTexture( const std::vector<U32>& utf_text, const Vector2& vTopLeftPos )
{
	// lock texture
	if( !m_FontTexture.GetEntry() )
		return false;

	shared_ptr<TextureResource> pTexture = m_FontTexture.GetEntry()->GetTextureResource();
	if( !pTexture )
		return false;

	bool locked = pTexture->Lock();
	if( !locked )
		return false;

	shared_ptr<LockedTexture> pLockedTexture;
	pTexture->GetLockedTexture( pLockedTexture );
	if( !pLockedTexture )
		return false;

	// Clear the texture with the transparent black color
	pLockedTexture->Clear( SFloatRGBAColor(0,0,0,0) );

	// draw text to texture

//	vector<UTFFont::CharRect>& char_rects = m_vecCharRect;
	RenderTextToBufferAndSetUpRects(
		m_Face,
		utf_text,
		m_FontHeight,
		*pLockedTexture,
//		char_rects,
		m_TextBox,
		m_CacheIndex,
		vTopLeftPos
		);

//	m_CacheIndex = (int)char_rects.size();
	m_CacheIndex = (int)m_TextBox.GetNumRects();

	// unlock texture

	bool unlocked = pTexture->Unlock();

	return unlocked;
}


void UTFFont::SetFontSize(int font_width, int font_height)
{
	TextureFont::SetFontSize( font_width, font_height );

	if( !m_Face )
	{
		// SetFontSize() is call again when the font is initialized.
		return;
	}

	FT_Face& face = m_Face;

	int char_height = font_height;//m_BaseHeight;

	int error = 0;

	// When a new face object is created, all elements are set to 0 during initialization.
	// To populate the structure with sensible values, simply call  FT_Set_Char_Size.
	// Here is an example where the character size is set to 16pt for a 300Å~300dpi device:
	error = FT_Set_Char_Size( face, /* handle to face object */
							  0, /* char_width in 1/64th of points */
							  char_height*64, /* char_height in 1/64th of points */
							  300, /* horizontal device resolution */
							  300 ); /* vertical device resolution */

	if( error != 0 )
	{
		LOG_PRINT_ERROR( "FT_Set_Char_Size() failed." );
	}

	// This function computes the character pixel size that corresponds
	// to the character width and height and device resolutions.
	// However, if you want to specify the pixel sizes yourself,
	// you can simply call  FT_Set_Pixel_Sizes, as in
	int pixel_height = char_height;
	error = FT_Set_Pixel_Sizes( face, /* handle to face object */
		                        0, /* pixel_width */
								pixel_height ); /* pixel_height */

	if( error != 0 )
	{
		LOG_PRINT_ERROR( "FT_Set_Pixel_Sizes() failed." );
	}
}


static inline void ConvertToUTF8CodePoints( const std::string& src_text, std::vector<U32>& utf8_code_points )
{
	string::const_iterator itr = src_text.begin();
	while( itr != src_text.end() )
	{
		U32 cp = utf8::next( itr, src_text.end() );
		utf8_code_points.push_back( cp );
	}

	const int num_utf8_chars = (int)utf8_code_points.size();
	LOG_PRINT_VERBOSE( " num_utf8_chars: " + to_string(num_utf8_chars) );
}


//TextureHandle sg_TestTexture;


void UTFFont::DrawText( const char* pcStr, const Vector2& vPos, U32 color )
{
//	ONCE( sg_TestTexture.Load( "tex1024_yellow.bmp" ) );

	vector<U32> utf8_code_points;
	ConvertToUTF8CodePoints( string(pcStr), utf8_code_points );

	m_vecCharRect.resize( 0 );
	m_CacheIndex = 0;
	m_TextBox.SetNumRects( 0 );

	DrawTextToTexture( utf8_code_points, vPos );

	m_TextBox.SetColor( color );

	SetRenderStatesForTextureFont( AlphaBlend::InvSrcAlpha );

//	TextureHandle orig = m_FontTexture;
//	m_FontTexture = sg_TestTexture;

	DrawCachedText();

//	m_FontTexture = orig;

//	ONCE( m_FontTexture.GetEntry()->GetTextureResource()->SaveTextureToImageFile( "text_rendered_to_texture.dds" ) );

	m_vecCharRect.resize( 0 );
	m_CacheIndex = 0;
}


void UTFFont::CacheText( const char* pcStr, const Vector2& vPos, U32 dwColor )
{
	vector<U32> utf8_code_points;
	ConvertToUTF8CodePoints( string(pcStr), utf8_code_points );

	DrawTextToTexture( utf8_code_points, vPos );
}


} // namespace amorphous
