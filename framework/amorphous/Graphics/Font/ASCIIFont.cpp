#include "ASCIIFont.hpp"
#include "amorphous/Graphics/Rect.hpp"
#include "amorphous/Graphics/TextureGenerators/TextureFillingAlgorithm.hpp"
#include "Support/lfs.hpp"
#include "Support/FreeTypeAux.hpp"
#include "Support/BitmapImage.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"


namespace amorphous
{

using namespace std;
using namespace boost;


static const std::string gs_FontBasicInfoKeyname = "Base";
static const std::string gs_FontTextureImageKeyname = "Texture";


void DrawRect( array2d<U8>& dest_buffer, const SRect& rect, U8 color )
{
	for( int y=rect.top; y<=rect.bottom; y++ )
	{
		dest_buffer( rect.left,  y ) = color;
		dest_buffer( rect.right, y ) = color;
	}

	for( int x=rect.left; x<=rect.right; x++ )
	{
		dest_buffer( x, rect.top    ) = color;
		dest_buffer( x, rect.bottom ) = color;
	}
}


class FontTextureLoader : public TextureFillingAlgorithm
{
	ASCIIFont *m_pFont;

public:

	FontTextureLoader( ASCIIFont *pFont )
		:
		m_pFont(pFont)
	{}

	void FillTexture( LockedTexture& texture );
};


void FontTextureLoader::FillTexture( LockedTexture& texture )
{
	array2d<U8> dest_bitmap_buffer;
	m_pFont->CreateFontTextureFromTrueTypeFont( dest_bitmap_buffer );

//	texture.Clear( SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 0.0f ) );

	const int w = dest_bitmap_buffer.size_x();
	const int h = dest_bitmap_buffer.size_y();
	for( int y=0; y<h; y++ )
	{
		for( int x=0; x<w; x++ )
		{
			U8 alpha = dest_bitmap_buffer(x,y);
			U32 color = (alpha << 24) | 0x00FFFFFF;
			texture.SetPixelARGB32( x, y, color );
		}
	}

	// Save the image for faster loading from the next time
/*	string filename_minus_ext = m_pFont->m_FontFilepath.substr( 0, m_pFont->m_FontFilepath.length() - 4 );
	CBinaryDatabase<string> db;
	bool db_open = db.Open( ASCIIFont::ms_TextureArchiveDestDirectoryPath + "/" + filename_minus_ext + ".tfd", CBinaryDatabase<string>::DB_MODE_NEW );
	if( db_open )
	{
		TextureFontArchive archive;
		archive.BaseCharHeight = m_pFont->m_BaseHeight;
		archive.vecCharRect    = m_pFont->m_vecCharRect;
		db.AddData( "Base", archive );

		int img_depth = 32; // ARGB, 8bits for each channel
		BitmapImage img( w, h, img_depth, SFloatRGBAColor( 0.0f, 0.0f, 0.0f, 0.0f ) );
		for( int y=0; y<h; y++ )
		{
			for( int x=0; x<w; x++ )
			{
				img.SetAlpha( x, y, dest_bitmap_buffer(x,y) );
			}
		}

		// create the image archive and add it to the db with the key
		ImageArchive img_archive( img );
		db.AddData( gs_FontTextureImageKeyname, img_archive );
	}*/
}


//=========================================================================================
// ASCIIFont
//=========================================================================================

std::string ASCIIFont::ms_TextureArchiveDestDirectoryPath = "./Texture";


ASCIIFont::ASCIIFont()
{
	InitTrueTypeFontInternal();
}


ASCIIFont::ASCIIFont( const std::string& filename,
 		                    int resolution, int font_width, int font_height )

{
	InitTrueTypeFontInternal();
	InitFont( filename, resolution, font_width, font_height );
}


ASCIIFont::~ASCIIFont()
{
	Release();
}


void ASCIIFont::InitTrueTypeFontInternal()
{
	TextureFont::InitInternal();

	m_BaseHeight = 64;

	m_pTextureLoader.reset( new FontTextureLoader(this) );
}


/// Sets up char rects as well
void RenderTextToBuffer( FT_Face& face,
						const std::string &text,
						int char_height,
						array2d<U8>& dest_bitmap_buffer,
						vector<ASCIIFont::CharRect>& char_rect )
{
	LOG_FUNCTION_SCOPE();

	float top, left, bottom, right;
	int _top, _left, _bottom, _right;
	FT_GlyphSlot slot = face->glyph; // a small shortcut
//	FT_UInt glyph_index;
	int pen_x, pen_y, n;

	const int num_chars = (int)text.size();

	int error = 0;

	char_rect.resize( num_chars );

	/// Shouldn't we change the character codes?
//	for( size_t i=0; i<text.size(); i++ )
//	{}

	int img_width = 1024;
	dest_bitmap_buffer.resize( img_width, img_width, 0 );

	int sx = 0;
	int sy = char_height;
	pen_x = sx;
	pen_y = sy;
	int margin = 4;
	for ( n = 0; n < num_chars; n++ )
	{
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

		char_rect[n].tex_min = TEXCOORD2( (float)_left,  (float)_top )    / (float)img_width;
		char_rect[n].tex_max = TEXCOORD2( (float)_right, (float)_bottom ) / (float)img_width;

		char_rect[n].rect.vMin = Vector2( (float)slot->bitmap_left,                      (float)char_height - slot->bitmap_top );
		char_rect[n].rect.vMax = Vector2( (float)slot->bitmap_left + slot->bitmap.width, (float)char_height - slot->bitmap_top + slot->bitmap.rows );

		char_rect[n].advance = (float)(slot->advance.x >> 6);
	}
}


bool ASCIIFont::CreateFontTextureFromTrueTypeFont( array2d<U8>& dest_bitmap_buffer )
{
	LOG_FUNCTION_SCOPE();

	const string& ttf_filepath = m_FontFilepath;

	FreeTypeLibrary ftlib;

	FT_Face face;

	int error;
	error = FT_New_Face( ftlib.GetFTLibrary(), ttf_filepath.c_str(), 0, &face );

	if ( error == FT_Err_Unknown_File_Format )
	{
		LOG_PRINT_ERROR( "The font file could be opened and read, but it appears that its font format is unsupported." );
		return false;
	}
	else if ( error )
	{
		LOG_PRINT_ERROR( "The font file could not be opened or read, or simply that it is broken." );
		return false;
	}

	int char_height = m_BaseHeight;

	// When a new face object is created, all elements are set to 0 during initialization.
	// To populate the structure with sensible values, simply call  FT_Set_Char_Size.
	// Here is an example where the character size is set to 16pt for a 300Å~300dpi device:
	error = FT_Set_Char_Size( face, /* handle to face object */
							  0, /* char_width in 1/64th of points */
							  char_height*64, /* char_height in 1/64th of points */
							  300, /* horizontal device resolution */
							  300 ); /* vertical device resolution */


	// This function computes the character pixel size that corresponds
	// to the character width and height and device resolutions.
	// However, if you want to specify the pixel sizes yourself,
	// you can simply call  FT_Set_Pixel_Sizes, as in
	int pixel_height = char_height;
	error = FT_Set_Pixel_Sizes( face, /* handle to face object */
		                        0, /* pixel_width */
								pixel_height ); /* pixel_height */

	string text = " !\"#$%&'()*+,-./0123456789:;<=>?`ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~";

	RenderTextToBuffer( face, text, char_height, dest_bitmap_buffer, m_vecCharRect );

//	SaveGrayscaleToImageFile( dest_bitmap_buffer, "./results/" + lfs::get_leaf(ttf_filepath) + ".bmp" );

	return true;
}


bool ASCIIFont::InitFont( const std::string& filename,
 		                     int resolution, int font_width, int font_height )
{
	if( filename.length() == 0 )
		return false;

	SetFontSize( font_width, font_height );

	string dot_and_3char_suffix = filename.substr( filename.length() - 4 );

	if( dot_and_3char_suffix == ".ttf"
	 || dot_and_3char_suffix == ".otf" )
	{
		m_BaseHeight = resolution;

		m_FontFilepath = filename;

		// set the string id
		string texture_name = filename + to_string(resolution);

		int mip_levels = 0; // Create complete mipmap chain
		return m_FontTexture.Create( m_pTextureLoader, texture_name, 1024, 1024, TextureFormat::A8R8G8B8, mip_levels );

		// First, create the texture
//		bool tex_created = CreateFontTextureFromTrueTypeFont();
	}
	else if( dot_and_3char_suffix == string(".") + GetTextureFontArchiveExtension() )
	{
		// Assume the user has specified texture font database
		CBinaryDatabase<string> db;
		bool db_open = db.Open( filename, CBinaryDatabase<string>::DB_MODE_APPEND );
		if( db_open )
		{
			TextureFontArchive archive;
			bool loaded = db.GetData( gs_FontBasicInfoKeyname, archive );
			if( loaded )
			{
				m_vecCharRect = archive.vecCharRect;
				m_BaseHeight  = archive.BaseCharHeight;
			}
			else
				return false;

			// Close before loading the texture
			// - The resource manager needs to open the db
			db.Close();

			TextureResourceDesc tex_desc;
			tex_desc.ResourcePath = filename + "::" + gs_FontTextureImageKeyname;
			return m_FontTexture.Load( tex_desc );
		}
		else
			return false;
	}
	else
	{
		// load as image file for font texture
		bool tex_loaded = m_FontTexture.Load( filename );
		if( !tex_loaded )
			return false;

		// load character info
		string charset_archive_filepath( filename );
		lfs::change_ext( charset_archive_filepath, "tfc" );

		TextureFontArchive archive;
		bool archive_loaded = archive.LoadFromFile( charset_archive_filepath );
		if( !archive_loaded )
			return false;

		m_vecCharRect = archive.vecCharRect;
		m_BaseHeight  = archive.BaseCharHeight;
		
		return true;
	}

	return false;
}


bool ASCIIFont::SaveTextureAndCharacterSet( const std::string& texture_filepath )
{
	m_FontTexture.SaveTextureToImageFile( texture_filepath );

	string charset_filepath = texture_filepath;
	lfs::change_ext( charset_filepath, "tfc" );

	TextureFontArchive tex_font_archive;
	tex_font_archive.BaseCharHeight = m_BaseHeight;
	tex_font_archive.vecCharRect    = m_vecCharRect;
	bool saved = tex_font_archive.SaveToFile( charset_filepath );

	return saved;
}


bool ASCIIFont::SaveTextureFontArchive( const std::string& pathname )
{
	string temporary_texture_pathname = "temporary_font_texture.png";
	bool tex_saved = m_FontTexture.SaveTextureToImageFile( temporary_texture_pathname );
	if( !tex_saved )
		return false;

	TextureFontArchive tex_font_archive;
	tex_font_archive.BaseCharHeight = m_BaseHeight;
	tex_font_archive.vecCharRect    = m_vecCharRect;
//	bool image_loaded = tex_font_archive.TextureImage.LoadFromFile( temporary_texture_pathname );
//	if( !image_loaded )
//		return false;
//
//	bool saved = tex_font_archive.SaveToFile( pathname );
//
//	return saved;

	CBinaryDatabase<string> db;
	bool db_open = db.Open( pathname, CBinaryDatabase<string>::DB_MODE_NEW );
	if( !db_open )
		return false;

	ImageArchive ia;
	bool image_loaded = ia.LoadImageFile( temporary_texture_pathname );
	if( !image_loaded )
		return false;

	bool basic_info_added = db.AddData( gs_FontBasicInfoKeyname,    tex_font_archive );
	bool texture_added    = db.AddData( gs_FontTextureImageKeyname, ia );

	db.Close();

	return (basic_info_added && texture_added);
}


} // namespace amorphous
