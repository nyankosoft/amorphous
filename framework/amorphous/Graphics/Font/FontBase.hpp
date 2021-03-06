#ifndef __FONTBASE_H__
#define __FONTBASE_H__


#include "amorphous/base.hpp"
#include "amorphous/3DMath/Vector2.hpp"
#include "amorphous/Graphics/FloatRGBAColor.hpp"
#include "amorphous/Graphics/AlphaBlend.hpp"

#include <cstring>
#include <string>


namespace amorphous
{


/**
 About font colors
 Options:
 1) uniform color
   - same color for all the chars
 2) uniform corner colors
   - different colors for each corner of the chars
   - same corner colors for all the chars
 3) individual colors
   - different colors for chars

   availability           D3DXFont  TextureFont
   -----------------------------------------------------------
   uniform colors           yes         yes
   uniform corner colors     no         yes?
   individua colors          no         yes?

*/


class FontBase
{
protected:

	/// font width & height in logical units
	int m_FontWidth;
	int m_FontHeight;

	U32 m_dwFontColor;

	int m_TypeFlag;

	AlphaBlend::Mode m_DestAlphaBlend;

public:

	enum FontType
	{
		FONTTYPE_TEXTURE,
		FONTTYPE_TRUETYPETEXTURE,
		FONTTYPE_UTF,
		FONTTYPE_D3DX,
		NUM_FONTTYPES
	};

	enum TypeFlag
	{
		BOLD    = (1 << 0),
		ITALIC  = (1 << 1),
		SHADOW  = (1 << 2),
	};

	FontBase() : m_FontWidth(1), m_FontHeight(1), m_dwFontColor(0xFFFFFFFF), m_TypeFlag(0), m_DestAlphaBlend(AlphaBlend::InvSrcAlpha) {}

	virtual ~FontBase() {};

	virtual void Release() {};

	virtual void Reload() = 0;

//	virtual int InitFont();

	inline int GetFontWidth() const { return m_FontWidth; }
	inline int GetFontHeight() const { return m_FontHeight; }
	inline void GetFontSize( int& w, int& h ) const { w = GetFontWidth(); h = GetFontHeight(); }

	virtual void SetFontSize(int width, int height) = 0;

	inline void SetFontColor( U32 color ) { m_dwFontColor = color; }

	inline void SetFontColor( const SFloatRGBAColor& color ) { m_dwFontColor = color.GetARGB32(); }

	void SetDestAlphaBlendMode( AlphaBlend::Mode mode ) { m_DestAlphaBlend = mode; }

	virtual void DrawText( const char *text, const Vector2& vPos, U32 color ) = 0;

	/// \param rotation_angle [in] rotation angle in degrees
	virtual void DrawText( const char *text, const Vector2& vPos, const Vector2& vPivotPoint, float rotation_angle, U32 color ) {}

	inline void DrawText( const char *text, const Vector2& vPos )
	{
		DrawText( text, vPos, m_dwFontColor );
	}

	inline void DrawText( const char *text, int x, int y, U32 color )
	{
		DrawText( text, Vector2((float)x, (float)y), color );
	}

	inline void DrawText( const char *text, int x, int y )
	{
		DrawText( text, x, y, m_dwFontColor );
	}


	inline void DrawText( const std::string& text, const Vector2& vPos, U32 color )
	{
		DrawText( text.c_str(), vPos, color );
	}

	inline void DrawText( const std::string& text, const Vector2& vPos )
	{
		DrawText( text.c_str(), vPos, m_dwFontColor );
	}

	inline void DrawText( const std::string& text, int x, int y, U32 color )
	{
		DrawText( text.c_str(), Vector2((float)x, (float)y), color );
	}

	inline void DrawText( const std::string& text, int x, int y )
	{
		DrawText( text.c_str(), x, y, m_dwFontColor );
	}

	inline void DrawText(
		const std::string& text,
		const Vector2& vPos,
		const Vector2& vPivotPoint,
		float rotation_angle,
		const SFloatRGBAColor& color
		)
	{
		DrawText( text.c_str(), vPos, vPivotPoint, rotation_angle, color.GetARGB32() );
	}

	inline void DrawText(
		const std::string& text,
		const Vector2& vPos,
		const Vector2& vPivotPoint,
		float rotation_angle
		)
	{
		DrawText( text.c_str(), vPos, vPivotPoint, rotation_angle, m_dwFontColor );
	}

	virtual int GetTextWidth( const char *text ) const
	{
		if( !text )
			return 0;
		else
			return (int)strlen(text) * GetFontWidth();
	}

	void SetFlags( U32 flags ) { m_TypeFlag = flags; }

	U32 SetFlags() const { return m_TypeFlag; }

	virtual void SetShadowColor( const SFloatRGBAColor& shadow_color ) {}

	virtual void SetItalic( float italic_weight ) {}

	virtual int GetFontType() const = 0;
};

} // namespace amorphous



#endif  /*  __FONTBASE_H__  */
