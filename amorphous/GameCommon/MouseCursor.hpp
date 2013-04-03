#ifndef __MouseCursor_SINGLETON_H__
#define __MouseCursor_SINGLETON_H__


#include "amorphous/Support/Singleton.hpp"
#include "amorphous/base.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/Rect.hpp"
#include <string>


namespace amorphous
{


class MouseCursor
{
	static singleton<MouseCursor> m_obj;

	TextureHandle m_Texture[4];
	C2DRect m_Rect[4];

	bool m_bDisplay;

	int m_CurrentCursorIndex;

	int m_PosX, m_PosY;

public:

	static MouseCursor* Get() { return m_obj.get(); }

	MouseCursor() : m_CurrentCursorIndex(0), m_bDisplay(true), m_PosX(0), m_PosY(0) { m_Rect[0] = C2DRect(0,0,0,0,0xFFFFFFFF); }

	inline void Show()	{ m_bDisplay = true; }
	inline void Hide()	{ m_bDisplay = false; }

	void Load( int index, const SRect& rect, const std::string& tex_filename, U32 color = 0xFFFFFFFF )
	{
		Load( index, rect.left, rect.top, rect.right, rect.bottom, tex_filename, color );
	}

	void Load( int index, int l, int t, int r, int b, const std::string& tex_filename, U32 color = 0xFFFFFFFF )
	{
		if( index < 0 || 4 <= index )
			return;

		m_Rect[index] = C2DRect( l, t, r, b, color );
		m_Texture[index].Load( tex_filename );

		// use the entire region of the texture for cursor rect
		m_Rect[index].SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
	}

	void UpdateCursorPosition( int x, int y )
	{
		m_PosX = x;
		m_PosY = y;
	}

	void Draw()
	{
		if( !m_bDisplay )
			return;

		C2DRect& src_rect = m_Rect[m_CurrentCursorIndex];

		C2DRect offset_rect = src_rect;
		offset_rect.Translate( m_PosX, m_PosY );
		offset_rect.Draw( m_Texture[m_CurrentCursorIndex] );
	}

	void ReleaseGraphicsResources()
	{
		for( int i=0; i<4; i++ )
		{
			m_Texture[i].Release();
		}
	}
};


inline MouseCursor& GetMouseCursor()
{
	return (*(MouseCursor::Get()));
}


} // amorphous



#endif  /*  __MouseCursor_SINGLETON_H__  */
