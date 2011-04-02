#ifndef __D3DXFont_HPP__
#define __D3DXFont_HPP__


#include "../../Font/FontBase.hpp"
#include <d3dx9.h>


class CD3DXFont : public CFontBase
{
	std::string m_strFontName;

	LPD3DXFONT m_pD3DXFont;

	// font width, height(in logical units) and color are declared
	// in the base class 'CD3DXFontBase'

//	bool CreateFont( const char* pcFontName, int width, int height );

public:

	CD3DXFont() : m_pD3DXFont(NULL) {}

	CD3DXFont( const std::string font_name, int font_width, int font_height, int type_flag = 0 );

	virtual ~CD3DXFont();

	virtual void Release();

	/// load font after releasing
	virtual void Reload();

	int InitFont( const std::string font_name, int font_width, int font_height, int type_flag = 0 );

	/// SetFontSize() of CD3DXFont class need to destroy and recreate font object
	/// every time the font size is changed, and not intended to be called 
	/// repeatedly in realtime.
	/// application should not call this function every frame.
	virtual void SetFontSize(int width, int height);

	virtual void DrawText( const char *str, const Vector2& vPos, U32 color );

	inline LPD3DXFONT GetD3DXFont() { return m_pD3DXFont; }

	virtual int GetFontType() const { return FONTTYPE_D3DX; }
};



#endif  /*  __D3DXFont_HPP__  */
