#ifndef  __UTFFont_HPP__
#define  __UTFFont_HPP__


#include "TextureFont.hpp"
#include "Support/FreeTypeAux.hpp"


/**
 A font class that support the rendering of UTF-8 text
  - Created from TrueType font (*.ttf)
*/
class CUTFFont : public CTextureFont
{
private:

	std::string m_FontFilepath;

//	boost::shared_ptr<CFontTextureLoader> m_pTextureLoader;

	boost::shared_ptr<CFreeTypeLibrary> m_pFreeTypeLibrary;

	FT_Face m_Face;

private:

	void InitUTFFontInternal();

	bool DrawTextToTexture( const std::vector<U32>& utf_text, const Vector2& vTopLeftPos );

public:

	CUTFFont();

	CUTFFont( const std::string& filename, int font_pt = 32, int resolution = 1 );

	virtual ~CUTFFont();

//	void Release();

//	void Reload();

	/// InitFont() is not made virtual and completely different from that of CFont
	/// \param filename [in] font file. Filetype could be one of the following
	/// - .ttf TrueType font
	/// - .otf OpenType font
	/// - .tfd ???
	/// \param font_pt [in] font size
	/// \param resolution [in] reserved
	bool InitFont( const std::string& filename, int font_pt = 32, int resolution = 1 );

	virtual void SetFontSize(int font_width, int font_height);

//	virtual float GetHorizontalFactor() const { return GetVerticalFactor(); }
//	virtual float GetVerticalFactor() const   { return (float)m_FontHeight / (float)m_BaseHeight; }

	virtual void DrawText( const char* pcStr, const Vector2& vPos, U32 color );

//	int GetTextWidth( const char *text ) const;

	/// push text data to the buffer
	void CacheText( const char* pcStr, const Vector2& vPos, U32 color );

	virtual int GetFontType() const { return FONTTYPE_UTF; }

	friend class CFontTextureLoader;
};


#endif		/*  __UTFFont_HPP__  */
