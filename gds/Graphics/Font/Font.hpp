#ifndef __FONT_H__
#define __FONT_H__

#include "FontBase.h"

#include <d3d9.h>
#include <d3dx9.h>

#include <string>

class CFont : public CFontBase
{
	std::string m_strFontName;

	LPD3DXFONT m_pD3DXFont;

	// font width, height(in logical units) and color are declared
	// in the base class 'CFontBase'

//	bool CreateFont( const char* pcFontName, int width, int height );

public:

	CFont() : m_pD3DXFont(NULL) {}

	CFont( const std::string font_name, int font_width, int font_height, int type_flag = 0 );

	virtual ~CFont();

	virtual void Release();

	/// load font after releasing
	virtual void Reload();

	int InitFont( const std::string font_name, int font_width, int font_height, int type_flag = 0 );

	/// SetFontSize() of CFont class need to destroy and recreate font object
	/// every time the font size is changed, and not intended to be called 
	/// repeatedly in realtime.
	/// application should not call this function every frame.
	virtual void SetFontSize(int width, int height);

	virtual void DrawText( const char *str, const Vector2& vPos, U32 color );

	inline LPD3DXFONT GetD3DXFont() { return m_pD3DXFont; }

	virtual int GetFontType() const { return FONTTYPE_NORMAL; }
};

#endif  /*  __FONT_H__  */
