#ifndef  __TrueTypeTextureFont_H__
#define  __TrueTypeTextureFont_H__


#include "3DCommon/TextureFont.h"
#include "Support/2DArray.h"

#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/BinaryDatabase.h"
using namespace GameLib1::Serialization;


class CTrueTypeTextureFont;


/**
  - archived resource of ANSI texture font
    - filename suffix: ".tfa"
    - Created when a texture font is initialized for the first time
	  - Subsequent init call of the same font will try to load the font
	    from the texture font archive, hopefully reducing the init time
		- i.e. Instance of this class works as a cache of font texture resource.

  Rationale to have texture font archive class
  - to support feature to initialize font texture quickly
    - no archived resource -> need to
	  - render the glyphs to buffer.
	  - copy the buffer content to memory of the texture resource
	  - usually,
	    - color channel(rgb): full brightness
		- alpha cahnnel: grayscale values of font

		=========================================================
		Cannot load texture from image archive directly!!!
		Need to be (binary db filepath)::(keyname)
		=========================================================

  - Why a separate archive class? What about deriving font class from IArchiveObjectBase?
    - CFontBase would need to be an archive object to avoid mulitple inheritance. 
*/
class CTextureFontArchive : public IArchiveObjectBase
{
public:

	int BaseCharHeight;

	std::vector<CTextureFont::CharRect> vecCharRect;

//	CImageArchive TextureImage;

public:

	CTextureFontArchive() : BaseCharHeight(64) {}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & BaseCharHeight & vecCharRect;// & TextureImage;
	}
};


class CFontTextureLoader : public CTextureLoader
{
	CTrueTypeTextureFont *m_pFont;

public:

	CFontTextureLoader( CTrueTypeTextureFont *pFont )
		:
	m_pFont(pFont)
	{}

	void FillTexture( CLockedTexture& texture );
};


/**
 A texture font created from TrueType font (*.ttf)
  - Supports only the ANSI chars
  - Crates a texture that stores the rendered ANSI chars
  - Saves the texture to disk to use the texture font by simply loading it
    from the next time

*/
class CTrueTypeTextureFont : public CTextureFont
{
private:

	std::string m_FontFilepath;

	boost::shared_ptr<CFontTextureLoader> m_pTextureLoader;

	static std::string ms_TextureArchiveDestDirectoryPath;

private:

	void InitTrueTypeFontInternal();

	/// Creates a bitmap that contains ANSI characters
	bool CreateFontTextureFromTrueTypeFont( C2DArray<U8>& dest_bitmap_buffer );

public:

	CTrueTypeTextureFont();

	CTrueTypeTextureFont( const std::string& filename,
		          int resolution = 64, int font_width = 0, int font_height = 32 );

	virtual ~CTrueTypeTextureFont();

//	void Release();

//	void Reload();

	/// InitFont() is not made virtual and completely different from that of CFont
	/// \param height font height
	/// \param width ignored for now. width is determined from the height
	/// \param filename font file. Filetype could be one of the following
	/// - .ttf TrueType font
	/// - .otf OpenType font
	/// - .tfd ???
	bool InitFont( const std::string& filename, int resolution,
		           int font_width, int font_height );

	static void SetTextureArchiveDestDirectoryPath( const std::string& dir_path ) { ms_TextureArchiveDestDirectoryPath = dir_path; }

//	virtual void SetFontSize(int FontWidth, int FontHeight);

	virtual float GetHorizontalFactor() const { return GetVerticalFactor(); }
	virtual float GetVerticalFactor() const   { return (float)m_FontHeight / (float)m_BaseHeight; }

//	virtual void DrawText( const char* pcStr, const Vector2& vPos, U32 dwColor );

	virtual int GetFontType() const { return FONTTYPE_TRUETYPETEXTURE; }

	friend class CFontTextureLoader;
};


#endif		/*  __TrueTypeTextureFont_H__  */
