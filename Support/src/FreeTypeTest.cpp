#include "amorphous/base.hpp"
#include "amorphous/Support/lfs.hpp"
#include "amorphous/Support/Timer.hpp"
#include "amorphous/Support/Profile.hpp"
//#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/FreeTypeAux.hpp"
#include "amorphous/Support/TextFileScanner.hpp"
#include "amorphous/Support/BitmapImage.hpp"
#include "amorphous/Support/UTF8/utf8.h"

using namespace std;


bool LoadTextFromFile( const std::string& src_file, std::string& dest )
{
	CTextFileScanner scanner( src_file );
	if( !scanner.IsReady() )
		return false;

	string text;
	for( ; !scanner.End(); scanner.NextLine() )
	{
		if( scanner.TryScanLine( "text", text ) )
		{
			if( text.length() == 0 )
				continue;

			dest += text;
		}
	}

	return true;
}


/// Sets up char rects as well
void RenderUTF8TextToBuffer( FT_Face& face,
						const std::string &text,
						int char_height,
						C2DArray<U8>& dest_bitmap_buffer//,                 ///< [out] buffer to render the text to
//						vector<CTrueTypeTextureFont::CharRect>& char_rect
						)
{
//	LOG_FUNCTION_SCOPE();

	float top, left, bottom, right;
	int _top, _left, _bottom, _right;
	FT_GlyphSlot slot = face->glyph; // a small shortcut
//	FT_UInt glyph_index;
	int pen_x, pen_y, n;

	const int num_chars = (int)text.size();

	int error = 0;

	vector<U32> utf8_code_points;
	string::const_iterator itr = text.begin();
	while( itr != text.end() )
	{
		U32 cp = utf8::next( itr, text.end() );
		utf8_code_points.push_back( cp );
	}
	const int num_utf8_chars = (int)utf8_code_points.size();

//	char_rect.resize( num_utf8_chars );

	/// Shouldn't we change the character codes?
//	for( size_t i=0; i<text.size(); i++ )
//	{}

	int img_width = 1600;
	int img_height= 256;
	dest_bitmap_buffer.resize( img_width, img_height, 0 );

	int sx = 0;
	int sy = char_height;
	pen_x = sx;
	pen_y = sy;
	int margin = 4;
	for ( n = 0; n < num_utf8_chars; n++ )
	{
		/* load glyph image into the slot (erase previous one) */
		FT_ULong char_code = utf8_code_points[n];
//		FT_ULong char_code = text[n];
		error = FT_Load_Char( face, char_code, FT_LOAD_RENDER );

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
		top  = (float)(pen_y - slot->bitmap_top) / (float)img_width;


		// now, draw to our target surface
		DrawBitmap( &slot->bitmap, pen_x + slot->bitmap_left, pen_y - slot->bitmap_top, dest_bitmap_buffer );

		// increment pen position
		pen_x += slot->bitmap_left + slot->bitmap.width;//slot->advance.x >> 6;

		right  = (float)pen_x / (float)img_width;
		bottom = (float)pen_y / (float)img_width;

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

/*		char_rect[n].tex_min = TEXCOORD2( (float)_left,  (float)_top )    / (float)img_width;
		char_rect[n].tex_max = TEXCOORD2( (float)_right, (float)_bottom ) / (float)img_width;

		char_rect[n].rect.vMin = Vector2( (float)slot->bitmap_left,                      (float)char_height - slot->bitmap_top );
		char_rect[n].rect.vMax = Vector2( (float)slot->bitmap_left + slot->bitmap.width, (float)char_height - slot->bitmap_top + slot->bitmap.rows );

		char_rect[n].advance = (float)(slot->advance.x >> 6);*/
	}
}


bool RenderUTF8TextToBufferToImageFile( const std::string& text,
										const std::string& font_file,
										const int base_char_height
										)
{
//	LOG_FUNCTION_SCOPE();

	const string& ttf_filepath = font_file;//"fonts/ipam.ttf";

	C2DArray<U8> dest_bitmap_buffer;

	CFreeTypeLibrary ftlib;

	FT_Face face;

	int error;
	error = FT_New_Face( ftlib.GetFTLibrary(), ttf_filepath.c_str(), 0, &face );

	if ( error == FT_Err_Unknown_File_Format )
	{
		printf( "The font file could be opened and read, but it appears that its font format is unsupported." );
		return false;
	}
	else if ( error )
	{
		printf( "The font file could not be opened or read, or simply that it is broken." );
		return false;
	}

	// When a new face object is created, all elements are set to 0 during initialization.
	// To populate the structure with sensible values, simply call  FT_Set_Char_Size.
	// Here is an example where the character size is set to 16pt for a 300Å~300dpi device:
	error = FT_Set_Char_Size( face, /* handle to face object */
							  0,                     // char_width in 1/64th of points
							  base_char_height * 64, // char_height in 1/64th of points
							  300,                   // horizontal device resolution
							  300                    // vertical device resolution
							  );


	// This function computes the character pixel size that corresponds
	// to the character width and height and device resolutions.
	// However, if you want to specify the pixel sizes yourself,
	// you can simply call  FT_Set_Pixel_Sizes, as in
	int pixel_height = base_char_height;
	error = FT_Set_Pixel_Sizes( face, /* handle to face object */
		                        0, /* pixel_width */
								pixel_height ); /* pixel_height */

//	string text = " !\"#$%&'()*+,-./0123456789:;<=>?`ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~";

//	vector<CTextureFont::CharRect> m_vecCharRect;
	RenderUTF8TextToBuffer( face, text, base_char_height, dest_bitmap_buffer );//, m_vecCharRect );

	const string img_filepath = "./results/" + lfs::get_leaf(ttf_filepath) + fmt_string("-%02d-",base_char_height) + ".bmp";

	SaveGrayscaleToImageFile( dest_bitmap_buffer, img_filepath );

	return true;
}


int StartFreeTypeTests()
{
	string text;
	bool loaded = LoadTextFromFile( "texts/test.txt", text );
	if( !loaded )
		return -1;

	if( text.length() == 0 )
		return -1;

	const char *fonts[] =
	{
		"fonts/ipag.ttf",
		"fonts/ipagp.ttf",
		"fonts/ipam.ttf",
		"fonts/ipamp.ttf",
		"fonts/cinecaption227.TTF",
		"fonts/GDhwGoJA-OTF107b.otf"
	};

	const int font_sizes[] =
	{
		8,
		12,
		16,
		24,
		32,
		48,
		64
	};
	
/*	CTimer m_Timer;
	m_Timer.Start();

	vector<CPerfRecord>& rec = g_rec;
	rec.reserve( numof(fonts) * numof(font_sizes) );
*/
	bool res = false;
	for( int i=0; i<numof(fonts); i++ )
	{
		for( int j=0; j<numof(font_sizes); j++ )
		{
//			double st = m_Timer.GetTime();

			res = RenderUTF8TextToBufferToImageFile(
				text,
				fonts[i],
				font_sizes[j] );

/*			double t = m_Timer.GetTime() - st;

			rec.push_back( CPerfRecord() );
			rec.back().name = string(fonts[i]);
			rec.back().size = font_sizes[j];
			rec.back().total_time = t;*/
		}
	}

	return 0;
}


int main( int argc, char *argv[] )
{
	StartFreeTypeTests();

	return 0;
}
