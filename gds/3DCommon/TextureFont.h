#ifndef  __TEXTUREFONT_H__
#define  __TEXTUREFONT_H__

#include "FontBase.h"

#include "FVF_TLVertex.h"

#include "AlphaBlend.h"
#include "TextureHandle.h"


extern void SetRenderStatesForTextureFont( AlphaBlend::Mode dest_alpha_blend );


class CTextureFont : public CFontBase
{
public:

	enum { NUM_MAX_LETTERS = 256 };

protected:

	std::string m_strTextureFilename;

	CTextureHandle m_FontTexture;

	/// the number of divisions of the texture
	int m_NumTexDivisionsX;
	int m_NumTexDivisionsY;

	TLVERTEX m_avTextBox[NUM_MAX_LETTERS * 6];

	/// used to store a group of text and draw them
	/// with a single DrawPrimitiveUP() call
	int m_CacheIndex;

	float m_fItalic;	///< 1.0f == a letter slant by the width of one char

	void InitInternal();

public:

	CTextureFont();

	CTextureFont( const std::string texture_filename,
		          int font_width, int font_height,
		          int num_tex_divisions_x = 16, int num_tex_divisions_y = 8 );

	virtual ~CTextureFont();

	void Release();

	void Reload();

	/// InitFont() is not made virtual and completely different from that of CFont
	bool InitFont( const std::string texture_filename,
		           int font_width, int font_height,
		           int num_tex_divisions_x = 16, int num_tex_divisions_y = 8 );

	virtual void SetFontSize(int FontWidth, int FontHeight);

//	virtual void DrawText( const char* pcStr, Vector2 vMin, Vector2 vMax);

	virtual void DrawText( const char* pcStr, const Vector2& vPos, U32 dwColor );

	/// push text data to the buffer
	void CacheText( const char* pcStr, const Vector2& vPos, U32 dwColor );

	inline void CacheText( const char* pcStr, const Vector2& vPos )
	{
		CacheText( pcStr, vPos, m_dwFontColor );
	}

	/// draw text in the buffer
	void DrawCachedText();

	/// clear text in the buffer
	inline void ClearCache() { m_CacheIndex = 0; }

	/// could be of some service if you are using fixed function pipeline...
	void SetDefaultTextureStageStates();

	void SetItalic( float italic_weight ) { m_fItalic = italic_weight; }

	virtual int GetFontType() const { return FONTTYPE_TEXTURE; }
};


#endif		/*  __TEXTUREFONT_H__  */
