
#ifndef  __MULTIPASSTEXFONT_H__
#define  __MULTIPASSTEXFONT_H__

#include "Font.h"

//#include <vector>
//using namespace std;

#include "3DCommon/FVF_TLVertex.h"
#include "3DCommon/TextureRenderTarget.h"


class CMultipassTexFont : public CFont
{
	TLVERTEX m_TexRectVertex[4];

	static CTextureRenderTarget m_TexRenderTarget;

	/// font size used by ID3DXFont object in the base CFont class
	int m_BaseFontWidth;
	int m_BaseFontHeight;

	static int ms_RefCount;

public:

	virtual void Release();

	virtual void Reload();

	CMultipassTexFont();

	~CMultipassTexFont();

	int InitFont( const char* pcFontName,
				  int base_font_width,
				  int base_font_height,
		          int font_width,
				  int font_height );

	/// set font size of the ID3DXFontObject.
	/// not the actual size of the font displayed on the final render target
	void SetBaseFontSize( int width, int height );

	/// unlike the base class CFont, there is no release/recreation of font object.
	/// the font size is adjusted in the rendering routine
	void SetFontSize( int width, int height );

	void DrawText( char *str, const D3DXVECTOR2& vPos, DWORD color = D3DCOLOR_XRGB(255,255,255) );

//	void DrawText( const char *str, const D3DXVECTOR2& vPos, DWORD color = D3DCOLOR_XRGB(255,255,255) );

};


#endif		/*  __MULTIPASSTEXFONT_H__  */
