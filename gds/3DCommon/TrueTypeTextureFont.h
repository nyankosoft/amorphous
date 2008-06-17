#ifndef  __TrueTypeTextureFont_H__
#define  __TrueTypeTextureFont_H__


#include "3DCommon/TextureFont.h"
#include "Support/2DArray.h"


class CTrueTypeTextureFont;


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
	bool InitFont( const std::string& filename, int resolution,
		           int font_width, int font_height );

//	virtual void SetFontSize(int FontWidth, int FontHeight);

	virtual float GetHorizontalFactor() const { return GetVerticalFactor(); }
	virtual float GetVerticalFactor() const   { return (float)m_FontHeight / (float)m_BaseHeight; }

//	virtual void DrawText( const char* pcStr, const Vector2& vPos, U32 dwColor );

	virtual int GetFontType() const { return FONTTYPE_TRUETYPETEXTURE; }

	friend class CFontTextureLoader;
};


#endif		/*  __TrueTypeTextureFont_H__  */
