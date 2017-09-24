#ifndef __MouseCursor_SINGLETON_H__
#define __MouseCursor_SINGLETON_H__


#include "amorphous/Support/Singleton.hpp"
#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/FloatRGBAColor.hpp"
#include "amorphous/Graphics/Rect.hpp"
#include <string>


namespace amorphous
{


class MouseCursor
{
	static singleton<MouseCursor> m_obj;

	std::shared_ptr<GraphicsElement> m_pCursor;

	bool m_bDisplay;

	int m_CurrentCursorIndex;

	int m_PosX, m_PosY;

public:

	static MouseCursor* Get() { return m_obj.get(); }

	MouseCursor() : m_CurrentCursorIndex(0), m_bDisplay(true), m_PosX(0), m_PosY(0) {}// m_Rect[0] = C2DRect(0,0,0,0,0xFFFFFFFF); }

	void Show();

	void Hide();

	void CreateDefault( GraphicsElementManager& mgr );

	void Load( GraphicsElementManager& mgr, int index, const SRect& rect, const std::string& tex_filename, const SFloatRGBAColor& color = SFloatRGBAColor::White() );

	void Load( GraphicsElementManager& mgr, int index, int l, int t, int r, int b, const std::string& tex_filename, const SFloatRGBAColor& color = SFloatRGBAColor::White() )
	{
		Load( mgr, index, l, t, r, b, tex_filename, color );
	}

	void UpdateCursorPosition( int x, int y );

	void ReleaseGraphicsResources( GraphicsElementManager& mgr );
};


inline MouseCursor& GetMouseCursor()
{
	return (*(MouseCursor::Get()));
}


} // amorphous



#endif  /*  __MouseCursor_SINGLETON_H__  */
